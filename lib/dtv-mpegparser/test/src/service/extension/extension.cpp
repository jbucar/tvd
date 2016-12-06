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
#include "extension.h"
#include "../../util.h"
#include "../../../../src/demuxer/psi/psidemuxer.h"
#include "../../../../src/demuxer/psi/dsmcc/module/memorydata.h"
#include <util/task/dispatcherimpl.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fs = boost::filesystem;

ServiceManagerTest::ServiceManagerTest( void )
{
	_mgr = NULL;
}

ServiceManagerTest::~ServiceManagerTest( void )
{
}

OkServiceManager::OkServiceManager( tuner::ResourceManager *resMgr, tuner::ID ts/*=0xb*/, tuner::ID nitID/*=0xb*/ )
	: tuner::ServiceManager( resMgr )
{
	_disp = new util::task::DispatcherImpl();
	_ts = ts;
	_nitID = nitID;
	tuner::dsmcc::MemoryData::initialize( 100, 4096 );
}

OkServiceManager::~OkServiceManager( void )
{
	delete _disp;
	tuner::dsmcc::MemoryData::cleanup();
}

void OkServiceManager::cleanAll() {
	tuner::ServiceManager::Services &sers = services();
	BOOST_FOREACH( tuner::Service *srv, sers ) {
		delete srv;
	}
	sers.clear();
}

util::task::Dispatcher *OkServiceManager::dispatcher() {
	return _disp;
}

void OkServiceManager::probe( tuner::ID pid, const char *file ) {
	std::map<tuner::ID,tuner::PSIDemuxer *>::const_iterator it=_demuxs.find( pid );
	if (it != _demuxs.end()) {
		util::Buffer *sec = test::getSection( file );
		ASSERT_TRUE(sec ? true : false );
		_demuxs[pid]->process( sec );
		delete sec;
	}
}

bool OkServiceManager::startFilter( tuner::PSIDemuxer *demux ) {
	std::map<tuner::ID,tuner::PSIDemuxer *>::iterator it=_demuxs.find( demux->pid() );
	if (it == _demuxs.end()) {
		_demuxs[demux->pid()] = demux;
		return true;
	}
	return false;
}

void OkServiceManager::stopFilter( tuner::ID pid ) {
	std::map<tuner::ID,tuner::PSIDemuxer *>::iterator it=_demuxs.find( pid );
	if (it != _demuxs.end()) {
		delete (*it).second;
		_demuxs.erase( it );
	}
}

FilterFailServiceMgr::FilterFailServiceMgr( tuner::ResourceManager *resMgr )
	: OkServiceManager( resMgr )
{
}

FilterFailServiceMgr::~FilterFailServiceMgr( void )
{
}

bool FilterFailServiceMgr::startFilter( tuner::PSIDemuxer *demux ) {
	delete demux;
	return false;
}

OkServiceManager *ServiceManagerTest::createManager( tuner::ResourceManager *res ) {
	return new OkServiceManager( res );
}

void ServiceManagerTest::SetUp() {
	fs::path tmp = fs::temp_directory_path();
	tmp /= "mpegparser-test";
	tuner::ResourceManager *resMgr = new tuner::ResourceManager( tmp.string(), 100, 100, 4096 );
	_mgr = createManager( resMgr );
	ASSERT_TRUE(_mgr ? true : false);
}

void ServiceManagerTest::TearDown() {
	mgr()->ready( false );
	mgr()->stop();
	delete _mgr;
}

void ServiceManagerTest::initMgr( tuner::Extension *ext ) {
	mgr()->attach( ext );
	mgr()->initialize();
	mgr()->start();
	mgr()->ready( true );
}

void ServiceManagerTest::finMgr( tuner::Extension *ext ) {
	mgr()->detach( ext );
	mgr()->ready( false );
	mgr()->stop();
	mgr()->finalize();
}

void ServiceManagerTest::startSrv( tuner::Service *srv ) {
	mgr()->add( srv );
	mgr()->readySrv( srv );
	mgr()->startSrv( srv );
}

void ServiceManagerTest::stopSrv( tuner::Service *srv ) {
	mgr()->stopSrv( srv );
	mgr()->readySrv( srv, false );
}

TEST_F( ServiceManagerTest, constructor ) {
}


