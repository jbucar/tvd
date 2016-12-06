#include "ginga.h"
#include "application.h"
#include <mpegparser/demuxer/ts/demuxer.h>
#include <mpegparser/demuxer/ts/demuxerimpl.h>
#include <mpegparser/provider/ts/provider.h>
#include <mpegparser/provider/ts/data/frontend.h>
#include <mpegparser/service/serviceprovider.h>
#include <mpegparser/service/extension/application/extension.h>
#include <mpegparser/resourcemanager.h>
#include <util/buffer.h>
#include <util/task/dispatcherimpl.h>
#include <boost/filesystem.hpp>

#define MEMORY_BLOCKS       500

//	DSMCC resources for downloader
#define MAX_MODULES         1000
#define MAX_MODULE_SIZE     (8*1024*1024)	//	8MB

//	DSMCC resources for applications
#define RAMDISK_MAX_MODULES     1000
#define RAMDISK_MAX_MODULE_SIZE (4*1024*1024)	//	4MB

namespace lifia {

namespace fs = boost::filesystem;

static int WINAPI ts_callback( unsigned char *data, int iLength, void *ptr );

class MyFrontend : public tuner::ts::Frontend {
public:
	MyFrontend ( tuner::demuxer::ts::DemuxerImpl *demux, Wrapper *wrapper ) : tuner::ts::Frontend(demux), _wrapper(wrapper) {}
	virtual ~MyFrontend ( void ) {}

	//	Network getters
	virtual size_t getCount() const { return 20; }
	virtual std::string getName( size_t /*nIndex*/ ) const { return "dummy"; }
	virtual size_t find( const std::string & /*name*/ ) const { return -1; }

	virtual bool getStatus( tuner::status::Type &st ) const {
		memset(&st,0,sizeof(st));
		st.isLocked = true;
		return true;
	}

	//	Network operations
	virtual bool start( size_t /*nIndex*/ ) {
		_wrapper->setupTSCallback( ts_callback, this, _wrapper->user_ptr );
		return true;
	}

	virtual void stop() {
		_wrapper->setupTSCallback( NULL, NULL, _wrapper->user_ptr );
	}

	void onData( unsigned char *data, int len ) {
		util::Buffer *buf = demux()->allocBuffer();
		assert(buf);
		buf->copy( (const char *)data, len );
		demux()->pushData( buf );
	}

	virtual std::string startImpl( size_t /*nIndex*/ ){ return ""; }
	virtual void stopImpl(){}
	virtual void reader( const std::string /*net*/ ){}

private:
	Wrapper *_wrapper;
};

static int WINAPI ts_callback( unsigned char *data, int iLength, void *ptr ) {
	MyFrontend  *prov = (MyFrontend  *)ptr;
	prov->onData( data, iLength );
	return 0;
}

class MyProvider : public tuner::ts::Provider {
public:
	MyProvider( Wrapper *wrapper ) : _wrapper(wrapper) {}

protected:
	//	Tuner methods
	virtual tuner::Frontend *createFrontend() const {
		return new MyFrontend( _tsDemuxer, _wrapper );
	}

private:
	Wrapper *_wrapper;
};

class MyDispatcher : public util::task::DispatcherImpl {
public:
	MyDispatcher( Wrapper *wrapper ) : _wrapper(wrapper) {}

	virtual void post( util::task::Target target, const util::task::Type &task ) {
		util::task::DispatcherImpl::post( target, task );
		_wrapper->notify( _wrapper->user_ptr );
	}

private:
	boost::mutex _mutex;

	Wrapper *_wrapper;
};

Ginga::Ginga( Wrapper *wrapper )
{
	_freq = -1;
	_srvID = -1;

	//	Create dispatcher
	_dispatcher = new MyDispatcher(wrapper);

	//	Create provider
	_provider = new MyProvider(wrapper);
	_provider->setDispatcher( _dispatcher );

	//	Start provider
	if (!_provider->start()) {
		throw "Cannot start provider";
	}

	//	Get temporary directory
	fs::path tmp = fs::temp_directory_path();
	tmp /= "lifia_ginga";

	//	Create service provider
	tuner::ResourceManager *resMgr = new tuner::ResourceManager( tmp.string(), MEMORY_BLOCKS, RAMDISK_MAX_MODULES, RAMDISK_MAX_MODULE_SIZE );
	_services = new tuner::ServiceProvider( _provider, resMgr );
	_services->initialize();

	{	//	Create and setup application controller
		_app = new AppController( wrapper, _dispatcher );
		//	Attach extension
		_services->attach( _app->initialize() );
		//	Scan ncl directory for applications
		_app->extension()->scan( "ncl", 3 );
		//	TODO: Setup output resolution
		_app->setOutputResolution( 720, 576 );
	}
}

Ginga::~Ginga()
{
	stop();
	_provider->stop();
	_services->finalize();
	delete _services;
	delete _provider;
	delete _dispatcher;
}

void Ginga::onNotification() {
	_dispatcher->runAll();
}

bool Ginga::dispatchKey( util::key::type key, bool isUp ) {
	return _app->sendKey( key, isUp );
}

void Ginga::startApplication( int id ) {
	_app->startApp( id );
}

void Ginga::getApplications( std::vector<std::string> &apps ) {
	_app->getApplications( apps );
}

void Ginga::play( int freq, int srvID ) {
	startFrequency( freq );
	startService( srvID );
}

void Ginga::stop() {
	stopServices();
	stopFrequency();
	_app->extension()->stopAll();
}

void Ginga::startFrequency( int freq ) {
	if (freq != _freq) {
		stopFrequency();

		if (_provider->setNetwork( 0 )) {
			_services->start();
		}

		_freq = freq;
	}
}

void Ginga::stopFrequency() {
	_services->stop();
	_freq = -1;
}

void Ginga::startService( int serviceID ) {
	if (serviceID != _srvID) {
		stopServices();

		_services->startService( serviceID );
		_srvID = serviceID;
	}
}

void Ginga::stopServices( void ) {
	_services->stopServices();
	_srvID = -1;
}

}	//	namespace ginga
