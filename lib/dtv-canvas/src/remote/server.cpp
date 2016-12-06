/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "server.h"
#include "memory.h"
#include "../canvas.h"
#include "../surface.h"
#include "../system.h"
#include <util/task/dispatcher.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/types.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace canvas {
namespace remote {

Server::Server()
{
	_exit = true;
	_bitmapMem = NULL;
	_dirtyMem = NULL;
}

Server::~Server()
{
	DTV_ASSERT(!_bitmapMem);
	DTV_ASSERT(!_dirtyMem);
}

bool Server::initialize( const Size &size, int zIndex ) {
	LINFO("remote::Server", "Initialize: layer size=(%d,%d)", size.w, size.h);
	_size = size;

	if (!init( zIndex )) {
		LERROR( "remote::Server", "Cannot initialize" );
		return false;
	}

	_dirtyMem = Memory::createServer(CANVAS_SHARED_DIRTY, CANVAS_DIRTY_SIZE);
	if (!_dirtyMem->initialize()) {
		LERROR( "remote::Server", "Cannot initialize dirty region memory" );
		DEL(_dirtyMem);
		return false;
	}

	_bitmapMem = Memory::createServer(CANVAS_SHARED_MEMORY, _size.w*_size.h*4);
	if (!_bitmapMem->initialize()) {
		LERROR( "remote::Server", "Cannot initialize bitmap memory" );
		DEL(_dirtyMem);
		DEL(_bitmapMem);
		return false;
	}

	//	Register into dispatcher
	disp()->registerTarget( this, "canvas::remote::Server" );	

	// Start render thread
	_exit=false;
	_thread = boost::thread( boost::bind( &Server::renderThread, this ) );

	return true;
}

bool Server::init( int /*zIndex*/ ) {
	return true;
}

void Server::finalize() {
	LINFO("remote::Server", "Finalize");

	//	Un-register into dispatcher
	disp()->unregisterTarget( this );		

	if (!_exit) {
		_exit=true;
		_bitmapMem->post();
		_thread.join();
	}

	if (_bitmapMem) {
		_bitmapMem->finalize();
		DEL( _bitmapMem );
	}

	if (_dirtyMem) {
		_dirtyMem->finalize();
		DEL( _dirtyMem );
	}

	fin();
}

void Server::fin() {
}

void Server::renderThread() {
	LINFO("remote::Server", "Render thread begin");
	bool exit=false;
	while (!exit) {
		_bitmapMem->wait();
		if (_exit) {
			exit=true;
		}
		else {
			//	Enqueue into canvas thread
			disp()->post( this, boost::bind(&Server::renderTask,this) );
		}
	}

	LINFO("remote::Server", "Render thread end");
}

void Server::renderTask() {
	std::vector<Rect> dirty;

	LDEBUG("remote::Server", "Do render");

	//	Lock bitmap
	void *imgMem = _bitmapMem->lock();

	//	Lock dirty regions
	unsigned char *dirtyMem = static_cast<unsigned char*>( _dirtyMem->lock() );

	util::DWORD size=0;
	memcpy(&size, dirtyMem, sizeof(size));

	if (size) {
		// Get dirty regions
		int pos=sizeof(Rect);
		for (util::DWORD i=0; i<size; i++) {
			Rect rect;
			memcpy(&rect, dirtyMem+pos, sizeof(Rect));
			pos += sizeof(Rect);
			dirty.push_back( rect );
		}

		// Clean dirty regions
		size = 0;
		memcpy(dirtyMem, &size, sizeof(size));

		//	Render
		render( dirty, imgMem, _bitmapMem->size() );
	}

	_dirtyMem->unlock();
	_bitmapMem->unlock();
}

const canvas::Size &Server::size() const {
	return _size;
}

}
}

