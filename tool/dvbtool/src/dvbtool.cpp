#include "dvbtool.h"
#include "generated/config.h"
#include <mpegparser/demuxer/psi/rawdemuxer.h>
#include <mpegparser/demuxer/psi/dsmcc/event.h>
#include <mpegparser/demuxer/psi/dsmcc/dsmccdemuxer.h>
#include <mpegparser/demuxer/psi/dsmcc/streameventdemuxer.h>
#include <mpegparser/demuxer/psi/dsmcc/objectcarouselfilter.h>
#include <mpegparser/demuxer/psi/dsmcc/downloadfilter.h>
#include <mpegparser/demuxer/psi/dsmcc/module/memorydata.h>
#include <mpegparser/provider/provider.h>
#include <mpegparser/resourcemanager.h>
#include <util/io/dispatcher.h>
#include <util/task/dispatcher.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>

CREATE_TOOL(dvbtool,DVBTOOL);

#define PSI_CALLBACK(m,pid) boost::bind(&dvbtool::m,this,pid,_1)

namespace bfs = boost::filesystem;

dvbtool::dvbtool()
{
	_io = NULL;
	_provider = NULL;
}

dvbtool::~dvbtool()
{
}

void dvbtool::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties( cmd );
	registerProp( "network", 'n', "Network to tune", (size_t)0, cmd);
	registerProp( "pid", 'p', "Filter pid", -1, cmd);
	registerProp( "pidscan", 's', "PID scan", false, cmd);	

	registerProp( "signal", 'r', "Signal rate statistics", false, cmd);

	registerProp( "dsmcc-object", 'o', "Object carousel download pid", -1, cmd);
	registerCfg( "dsmcc-object-tag", "Object carousel tag", -1, true);
	registerCfg( "dsmcc-object-root", "Object carousel root", "/tmp/object_carousel",true);

	registerProp( "dsmcc-update", 'u', "Update download pid", -1, cmd);
	// registerToolProp( "play", 'p', "Play channel", 0, cmd);

	registerCfg( "write-section", "Write section to files", false, true );
}

int dvbtool::run( util::cfg::cmd::CommandLine &cmd ) {
	LINFO( "dvbtool", "Run" );

	//	Initialize io
	_io = util::io::Dispatcher::create();
	if (!_io || !_io->initialize()) {
		DEL(_io);
		LERROR( "pvr", "Cannot create/initialize io dispatcher" );
		return -1;
	}
	disp()->registerTarget( this, "dvbtool" );

	//	Create provider
	_provider=tuner::Provider::create();
	if (!_provider) {
		_io->finalize();
		DEL(_io);
		LERROR( "dvbtool", "Cannot create dvb provider" );
		return -1;
	}
	_provider->setDispatcher( disp() );

	//	Start provider
	if (!_provider->start()) {
		DEL(_provider);
		_io->finalize();
		DEL(_io);
		LERROR( "dvbtool", "Cannot start dvb provider" );
		return -1;
	}

	{	//	Set network
		size_t nitID = getCfg<size_t>("network");
		_provider->setNetwork( nitID );
	}

	if (cmd.isSet("pid")) {
		int pid = getCfg<int>("pid");
		disp()->post( this, boost::bind(&dvbtool::runFilterPID,this,pid) );
	}
	else if (cmd.isSet( "signal" )) {
		showSignal();
	}
	else if (cmd.isSet( "pidscan" )) {
		disp()->post( this, boost::bind(&dvbtool::runPIDScan,this) );
	}
	else if (cmd.isSet( "dsmcc-object" )) {
		disp()->post( this, boost::bind(&dvbtool::downloadObjectCarousel,this) );
	}
	else if (cmd.isSet( "dsmcc-update" )) {
		int pid = getCfg<int>("dsmcc-update");
		disp()->post( this, boost::bind(&dvbtool::downloadUpdate,this,pid) );
	}

	//	Run IO
	_io->run();

	//	Stop provider
	_provider->stop();
	disp()->unregisterTarget( this );

	DEL(_provider);

	//	Finalize IO
	_io->finalize();
	DEL(_io);

	return 0;
}

void dvbtool::showSignal() {
	tuner::status::Type st;
	_provider->status( st );
	printf( "Status: lock(%d), signal(%d), rate(%d)\n", st.isLocked, st.signal, st.snr );

	_io->addTimer( 500, boost::bind(&dvbtool::showSignal,this) );	
}

void dvbtool::onFilterPSISection( int pid, const boost::shared_ptr<util::Buffer> &buf ) {
	bool writeSection = getCfg<bool>("write-section");
	if (writeSection) {
		static int i=0;
		bfs::path tmp = bfs::temp_directory_path();
		tmp /= util::format( "section_%04x_%02x", pid, i++ );
		const std::string fileName(tmp.string());
		FILE *f = fopen( fileName.c_str(), "w+b");
		if (f) {
			fwrite( buf->data(), buf->length(), 1, f );
			fclose( f );
			printf( "Write section: file=%s\n", fileName.c_str() );
		}
		else {
			printf( "Error tratando de escribir archivo: file=%s\n", fileName.c_str() );
		}
	}
	else {
		printf( "Read secion: pid=%04x, data=%s\n", pid, buf->asHexa().c_str() );
	}
}

void dvbtool::runFilterPID( int pid ) {
	if (!filterPID( pid, PSI_CALLBACK(onFilterPSISection,pid) )) {
		LERROR( "dvbtool", "Cannot start PSI filter: pid=%04x", pid );
		_io->exit();
	}
}

//	OC download
void dvbtool::onMounted( const boost::shared_ptr<tuner::dsmcc::ObjectCarousel> &oc ) {
	const std::string &rootPath = getCfg<std::string>("tool.dvbtool.dsmcc-object-root");
	//	Rename rootDirectory to applicationsPath/ApplicationName
	bfs::rename( oc->root(), rootPath );
	printf( "On Object Carousel mounted: root=%s, events=%zu\n", rootPath.c_str(), oc->events().size() );
	_io->exit();
}

void dvbtool::downloadObjectCarousel() {
	int pid = getCfg<int>("dsmcc-object");
	int tag = getCfg<int>("dsmcc-object-tag");

	tuner::ResourceManager *resMgr = new tuner::ResourceManager(
		"/tmp/oc",
		1000,
		1000,
		(100*1024*1024)
	);

	//	Initialize pool of memory data
	tuner::dsmcc::MemoryData::initialize( resMgr->memoryblocks(), 4096 );

	//	Create DSMCC
	tuner::dsmcc::DSMCCDemuxer *dsmcc=new tuner::dsmcc::DSMCCDemuxer( pid, resMgr );

	//	Create object carousel and setup
	tuner::dsmcc::ObjectCarouselFilter *oc = new tuner::dsmcc::ObjectCarouselFilter( tag, dsmcc );
	oc->onMounted( boost::bind(&dvbtool::onMounted, this, _1) );

	//	Start filter
	if (!_provider->startFilter( dsmcc )) {
		LWARN("app::Application", "cannot download application. Filter cannot be started: pid=%04x", pid);
	}
}

//	Download
void dvbtool::onDownload( const boost::shared_ptr<tuner::dsmcc::Download> &download ) {
	printf( "Download received: %zu\n", download->size() );
}

void dvbtool::downloadUpdate( int pid ) {
	tuner::ResourceManager *resMgr = new tuner::ResourceManager(
		"/tmp/oc",
		1000,
		1000,
		(100*1024*1024)
	);

	//	Initialize pool of memory data
	tuner::dsmcc::MemoryData::initialize( resMgr->memoryblocks(), 4096 );

	//	Create DSMCC
	tuner::dsmcc::DSMCCDemuxer *dsmcc=new tuner::dsmcc::DSMCCDemuxer( pid, resMgr );

	//	Create temporary root directory
	std::string root = resMgr->mkTempFileName( "oc_%%%%%%" );

	//	Create and setup downloader filter
	tuner::dsmcc::DownloadFilter *df = new tuner::dsmcc::DownloadFilter( 0xFF819282, 0x06ff0001, dsmcc );
	df->onDownload( boost::bind(&dvbtool::onDownload,this,_1) );

	//	Start filter
	if (!_provider->startFilter( dsmcc )) {
		LWARN("app::Application", "cannot download application. Filter cannot be started: pid=%04x", pid);
	}
}

//	PID Scan
void dvbtool::onPidScanCallback( int pid, const boost::shared_ptr<util::Buffer> & /*buf*/ ) {
	printf( "Read secion: pid=%04x\n", pid );
	processPID( pid );
}

void dvbtool::onPidTimeout( int pid ) {
	processPID( pid );
}

void dvbtool::processPID( int pid ) {
	_provider->stopFilter( pid );
	if (_pidScanLast < TS_PID_NULL) {
		scanPID( _pidScanLast );
		_pidScanLast++;
	}
}

void dvbtool::scanPID( int pid ) {
	LDEBUG( "dvbtool", "Scan pid: pid=%04x", pid );
	tuner::RawDemuxer *psi = new tuner::RawDemuxer(pid,3000,false);
	psi->onParsed( PSI_CALLBACK(onPidScanCallback,pid) );
	psi->onTimeout( boost::bind(&dvbtool::onPidTimeout,this,_1) );
	if (!_provider->startFilter( psi )) {
		LERROR( "dvbtool", "Cannot start PSI filter: pid=%04x", pid );
		_io->exit();
	}
}

void dvbtool::runPIDScan() {
	//	TODO: Export maxFilters
	for (_pidScanLast=0; _pidScanLast<16; _pidScanLast++) {
		scanPID( _pidScanLast );
	}
}

//	Aux
bool dvbtool::filterPID( int pid, const PSICallback &callback ) {
	tuner::RawDemuxer *psi = new tuner::RawDemuxer(pid,3000,false);
	psi->onParsed( callback );
	return _provider->startFilter( psi );
}

util::task::Dispatcher *dvbtool::disp() const {
	DTV_ASSERT(_io);
	return _io->dispatcher();
}


