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
#include "util.h"
#include "../../src/demuxer/psi/psi.h"
#include "../../src/service/service.h"
#include <util/buffer.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

namespace test {

namespace fs = boost::filesystem;

static std::string getDepot() {
	const char *root=getenv( "DEPOT" );
	return root ? root : "/";
}

std::string getTestRoot() {
	fs::path rootPath(getDepot());
	rootPath /= "lib";
	rootPath /= "dtv-mpegparser";
	rootPath /= "test";
	return rootPath.string();
}

util::Buffer *getSection( const char *file, util::Buffer *buf/*=NULL*/ ) {
	util::Buffer *tmp = NULL;
	fs::path rootPath(getTestRoot());
	rootPath /= "ts";
	rootPath /= file;
	FILE *f = fopen( rootPath.string().c_str(),"r+b");
	if (f) {
		tmp = (buf) ? buf : new util::Buffer( TSS_PRI_MAX_BYTES );
		int bytes = ::fread( tmp->data(), 1, tmp->capacity(), f );
		tmp->resize( bytes );
		fclose( f );
	}
	return tmp;
}

void counter( int *count ) {
	(*count)++;
}

boost::condition_variable _cWakeup;
boost::mutex _mutex;
std::map<util::DWORD,int> _flags;

void notifyCounter( int *count ) {
	(*count)++;
	notify( flags::task_running );
}

void clean()  {
	_mutex.lock();
	_flags.clear();
	_mutex.unlock();
}

void notify( flags::type flag ) {
	//printf( "[notify] flag=%x\n", flag );

	_mutex.lock();
	_flags[flag]++;
	_mutex.unlock();
	_cWakeup.notify_all();
}

void wait( flags::type flag ) {
	//printf( "[wait] flag=%x\n", flag );
	boost::unique_lock<boost::mutex> lock( _mutex );
	while (!_flags[flag]) {
		_cWakeup.wait( lock );
	}
	_flags[flag]--;
}


tuner::Pmt *createPmt( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, std::vector<tuner::ElementaryInfo> elems ) {
	//	Contruct Service
	tuner::desc::Descriptors descs;
	return new tuner::Pmt( srvPID, 1, srvID, pcrPID, descs, elems );
}

void addStream( std::vector<tuner::ElementaryInfo> &elems, util::BYTE type, tuner::ID pid ) {
	tuner::ElementaryInfo elem;
	elem.streamType = type;
	elem.pid = pid;
	elems.push_back( elem );
}

void addStream( std::vector<tuner::ElementaryInfo> &elems, util::BYTE type, tuner::ID pid, util::BYTE tag ) {
	tuner::ElementaryInfo elem;
	elem.streamType = type;
	elem.pid = pid;

	util::BYTE streamIdentifier[] = { 0x52, 0x1, tag };
	elem.descriptors.addDescriptor( streamIdentifier, 3 );

	elems.push_back( elem );
}

void addTypeB( std::vector<tuner::ElementaryInfo> &elems, tuner::ID pid, util::BYTE tag ) {
	tuner::ElementaryInfo elem;
	elem.streamType = PSI_ST_TYPE_B;
	elem.pid = pid;
	util::BYTE streamIdentifier[] = { 0x52, 0x1, tag };
	elem.descriptors.addDescriptor( streamIdentifier, 3 );
	elems.push_back( elem );
}

tuner::Service *createService( tuner::ID srvID, tuner::ID srvPID, tuner::Pmt *pmt ) {
	tuner::ServiceID id(1, 1, srvID);
	tuner::Service *srv = new tuner::Service( 8, id, srvPID );
	boost::shared_ptr<tuner::Pmt> ptr(pmt);
	srv->process( ptr );
	return srv;
}

tuner::Service *createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	std::vector<tuner::ElementaryInfo> elems;
	addTypeB( elems, ocPID, ocTAG );
	tuner::Pmt *pmt = createPmt( srvID, srvPID, pcrPID, elems );
	return test::createService( srvID, srvPID, pmt );
}

}
