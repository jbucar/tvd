/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "basicpipe.h"
#include <util/pool/circularpool.h>
#include <util/buffer.h>

namespace tuner {
namespace stream {

BasicPipe::BasicPipe( const std::string &name, int maxCount, int blockSize )
{
	_pool = new util::pool::CircularPool<util::Buffer *>( name, maxCount, blockSize );
}

BasicPipe::~BasicPipe()
{
	reset();
	delete _pool;
}

void BasicPipe::notify( util::DWORD mask ) {
	_pool->notify_all( mask );
}

void BasicPipe::enable( bool value ) {
	_pool->enable( value );
}

void BasicPipe::exit() {
	notify( mask::exit );
}

void BasicPipe::reset() {
	_pool->removeAll();
}

util::Buffer *BasicPipe::get( util::DWORD &mask ) {
	util::Buffer *tmp=NULL;
	_pool->get( tmp, mask );
	return tmp;
}

util::Buffer *BasicPipe::popBuffer() {
	util::DWORD mask;
	util::Buffer *tmp=NULL;
	if (_pool->get( tmp, mask )) {
		return tmp;
	}
	else {
		return NULL;
	}
}

//	Buffer allocation routines
util::Buffer *BasicPipe::alloc() {
	return _pool->alloc();
}

void BasicPipe::free( util::Buffer *buf ) {
	_pool->free( buf );
}

//	Buffer enqueue
void BasicPipe::push( util::Buffer *buf ) {
	_pool->put( buf );
}

}
}

