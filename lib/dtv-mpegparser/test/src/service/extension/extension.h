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
#pragma once

#include "../../util.h"
#include "../../../../src/service/servicemanager.h"
#include "../../../../src/service/service.h"
#include "../../../../src/service/extension/extension.h"
#include "../../../../src/resourcemanager.h"
#include <util/buffer.h>

namespace util {
namespace task {
	class DispatcherImpl;
}
}

class OkServiceManager : public tuner::ServiceManager {
public:
	OkServiceManager( tuner::ResourceManager *resMgr, tuner::ID ts=0xb, tuner::ID nitID=0xb );
	virtual ~OkServiceManager();

	void add( tuner::Service *srv ) { addService( srv ); }
	void cleanAll();
	void ready( bool ready ) { if (ready) { setReady(); } else { expire(); } }
	void startSrv( tuner::Service *srv ) { srv->state( tuner::service::state::ready ); startService( srv ); }
	void readySrv( tuner::Service *srv, bool isReady=true ) {
		if (isReady) {
			setServiceReady( srv );
		} else {
			expireService( srv, false );
		}
	}
	void stopSrv( tuner::Service *srv ) { stopService( srv ); }
	void probe( tuner::ID pid, const char *file );

	virtual util::task::Dispatcher *dispatcher();
	virtual tuner::ID currentTS() const { return _ts; }
	virtual tuner::ID currentNit() const { return _nitID; }
	virtual bool startFilter( tuner::PSIDemuxer *demux );
	virtual bool startFilter( tuner::pes::FilterParams * /*params*/ ) { return true; }
	virtual std::string streamPipe( void ) { return ""; }
	virtual void stopFilter( tuner::ID pid );

private:
	util::task::DispatcherImpl *_disp;
	tuner::ID _ts;
	tuner::ID _nitID;
	std::map<tuner::ID,tuner::PSIDemuxer *> _demuxs;
};

class FilterFailServiceMgr : public OkServiceManager {
public:
	FilterFailServiceMgr( tuner::ResourceManager *resMgr );
	virtual ~FilterFailServiceMgr( void );
	virtual bool startFilter( tuner::PSIDemuxer *demux );
};

class ServiceManagerTest : public testing::Test {
public:
	ServiceManagerTest();
	virtual ~ServiceManagerTest();

	virtual void SetUp();
	virtual void TearDown();

	OkServiceManager *mgr() { return _mgr; }

	void initMgr( tuner::Extension *ext );
	void finMgr( tuner::Extension *ext );

	void startSrv( tuner::Service *srv );
	void stopSrv( tuner::Service *srv );

protected:
	virtual OkServiceManager *createManager( tuner::ResourceManager *res );

private:
	OkServiceManager *_mgr;
};

