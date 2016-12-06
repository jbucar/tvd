/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "../util.h"
#include "../../../src/provider/provider.h"
#include "../../../src/provider/broadcast/frontend.h"
#include "../../../src/provider/filter.h"
#include "../../../src/service/service.h"
#include "../../../src/service/extension/extension.h"
#include "../../../src/demuxer/psi/psidemuxer.h"
#include <util/buffer.h>
#include <util/task/dispatcherimpl.h>
#include <boost/bind.hpp>

typedef std::vector<bool> ReadyStatus;
typedef std::map<tuner::ID,ReadyStatus> ServicesStatus;

class OkExtension : public tuner::Extension {
public:
	OkExtension( void ) {}
	virtual ~OkExtension( void ) {}

	typedef boost::function<void (tuner::Service *)> OnReady;

	virtual void onStart( void ) { _start.push_back( true ); }
	virtual void onStop( void ) { _start.push_back( false ); }

	//	Service provider
	virtual void onReady( bool isReady ) { _ready.push_back( isReady ); }

	//	Services Notifications
	void notifyOnServiceReady( const OnReady &callback ) { _callback = callback; }
	virtual void onServiceReady( tuner::Service *srv, bool ready )   { _srvReady[srv->id()].push_back( ready ); if (ready && !_callback.empty()) _callback(srv); }
	virtual void onServiceStarted( tuner::Service *srv, bool started ) { _srvStart[srv->id()].push_back( started ); }

	ReadyStatus   _init;
	ReadyStatus   _start, _ready;
	ServicesStatus _srvStart, _srvReady;
	OnReady _callback;

protected:
	virtual bool onInitialize() { _init.push_back(true); return true; }
	virtual void onFinalize() { _init.push_back(false); }
};

class OkFilter : public tuner::Filter {
public:
	OkFilter( tuner::PSIDemuxer *demux ) : tuner::Filter( demux->pid(), demux->timeout() ) {}
	virtual ~OkFilter( void ) {}

	virtual bool initialized() const { return false; }
	virtual bool initialize( void ) { return true; }
	virtual bool start( void ) { return true; }
};

class InitFailFilter : public OkFilter {
public:
	InitFailFilter( tuner::PSIDemuxer *demux ) : OkFilter( demux ) {}
	virtual bool initialize( void ) { return false; }
};

class StartFailFilter : public OkFilter {
public:
	StartFailFilter( tuner::PSIDemuxer *demux ) : OkFilter( demux ) {}
	virtual bool start() { return false; }
};

class OkTuner : public tuner::broadcast::Frontend {
public:
	OkTuner() {}
	virtual ~OkTuner() {}

	//	Initialization
	virtual bool initialize() {
		return loadConfig();
	}

	virtual void finalize() {
		cleanConfig();
	}

protected:
	virtual bool start( tuner::broadcast::IsdbtConfig * /*chConfig*/ ) {
		return true;
	}

};


class LockTuner : public OkTuner {
public:
	LockTuner() {}
	virtual ~LockTuner() {}

	virtual bool getStatus( tuner::status::Type &st ) const {
		st.isLocked=true;
		return true;
	}
};

class StartFailTuner : public OkTuner {
	virtual bool start( size_t /*nIndex*/ ) { return false; }
};

class OkProvider : public tuner::Provider {
public:
	OkProvider();
	virtual ~OkProvider();

	size_t stopCount;
	util::task::DispatcherImpl *_myDisp;

	virtual void probeSection( tuner::ID pid, const char *file ) {
		util::Buffer *buf = getNetworkBuffer();
		ASSERT_TRUE( buf ? true : false );

		util::Buffer *sec = test::getSection( file, buf );
		ASSERT_TRUE( sec ? true : false );

		enqueue( pid, sec );
	}

	virtual void probeTimeout( tuner::ID pid ) {
		enqueue( pid, NULL );
	}

	tuner::impl::state::type state() const {
		return getState();
	}

protected:
	virtual tuner::Frontend *createFrontend() const { return new OkTuner(); }
	virtual void stopNetwork( void ) { stopCount++; }
	virtual tuner::Filter *createFilter( tuner::pes::FilterParams * /*params*/ ) { return NULL; }
	virtual tuner::Filter *createFilter( tuner::PSIDemuxer *demux ) {
		OkFilter *filter = createFilterEx( demux );
		if (filter) {
			_filters[demux->pid()] = filter;
		}
		return filter;
	}
	virtual OkFilter *createFilterEx( tuner::PSIDemuxer *demux ) { return new OkFilter( demux ); }
	virtual int maxFilters() const { return 100; }

private:
	std::map<tuner::ID,OkFilter *> _filters;
};

template<class T>
class TunerProvider : public OkProvider {
protected:
	virtual tuner::Frontend *createFrontend() const { return new T(); }
};

class NotifyProvider : public OkProvider {
public:
	NotifyProvider( void ) {}
	virtual ~NotifyProvider() {}

	virtual void probeSection( tuner::ID pid, const char *file ) {
		OkProvider::probeSection( pid, file );

		//	Wait process data
		test::wait( test::flags::filter_process );
		_myDisp->runAll();
	}

	virtual void probeTimeout( tuner::ID pid ) {
		enqueue( pid, NULL );

		//	Wait process data
		test::wait( test::flags::filter_process );
		_myDisp->runAll();
	}

protected:
	virtual void processSectionBuffer( tuner::PSIDemuxer *demux, util::Buffer *buf ) {
		demux->process( buf );
		test::notify( test::flags::filter_process );
	}
};

class CreateFailProvider : public OkProvider {
protected:
	virtual OkFilter *createFilterEx( tuner::PSIDemuxer * /*demux*/ ) { return NULL; }
};

class MaxFilterProvider : public OkProvider {
protected:
	virtual int maxFilters() const { return 1; }
};

template<class T>
class FilterProvider : public OkProvider {
protected:
	virtual OkFilter *createFilterEx( tuner::PSIDemuxer *demux ) { return new T(demux); }
};

class EngineFailProvider : public OkProvider {
protected:
	virtual bool startEngineFilter( void ) { return false; }
};

class StartNetworkFailProvider : public OkProvider {
protected:
	virtual tuner::Frontend *createFrontend() const { return new StartFailTuner(); }
};

class ATestDemuxer : public tuner::PSIDemuxer  {
public:
	ATestDemuxer( tuner::ID pid ) : PSIDemuxer( pid ) {}

	virtual tuner::ID tableID() const { return 0; }
	virtual util::WORD maxSectionBytes() const { return 4096; }
	virtual util::DWORD bitRate() const { return 100; }
	virtual bool syntax() const { return true; }

protected:
	virtual util::DWORD frequency() const { return 0; }
};

class TestDemuxer : public ATestDemuxer {
public:
	TestDemuxer( tuner::ID pid ) : ATestDemuxer( pid ) { started=0; pushed=0; }

	virtual void push( util::BYTE * /*tsPayload*/, size_t /*len*/, bool start ) { if (start) started++; else pushed++; }

	int started;
	int pushed;
};

class CountTestDemuxer : public ATestDemuxer {
public:
	CountTestDemuxer( tuner::ID pid, int &count ) : ATestDemuxer( pid ), _count(count) {}
	virtual void push( util::BYTE * /*tsPayload*/, size_t /*len*/, bool /*start*/ ) { _count++; }

protected:
	int &_count;
};

class Test2Demuxer : public TestDemuxer {
public:
	Test2Demuxer( tuner::Provider *prov, tuner::ID pid ) : TestDemuxer( pid ) { _prov = prov; }
	virtual void push( util::BYTE * /*tsPayload*/, size_t /*len*/, bool /*start*/ ) { _prov->stopFilter( pid() ); }
	tuner::Provider *_prov;
};

