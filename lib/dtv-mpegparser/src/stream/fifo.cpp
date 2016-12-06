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

#include "fifo.h"
#include <util/resourcepool.h>
#include <util/buffer.h>
#include <util/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace tuner {
namespace stream {

Fifo::Fifo()
{
	_pool = new util::pool::ResourcePool( "stream.fifo", 1000, 4096 );
	::mkfifo( "/tmp/fifo_prueba", S_IRUSR| S_IWUSR );
	_fd = open( "/tmp/fifo_prueba", O_APPEND | O_WRONLY );
	DTV_ASSERT( _fd > 0 );
}

Fifo::~Fifo()
{
	delete _pool;
	close( _fd );
}

//	Buffer allocation routines
util::Buffer *Fifo::alloc() {
	return _pool->alloc();
}

void Fifo::free( util::Buffer *buf ) {
	_pool->free( buf );
}

//	Buffer enqueue
void Fifo::push( util::Buffer *buf ) {
	if (write( _fd, buf->buffer(), buf->length() ) != buf->length()) {
		LWARN( "stream::fifo", "Cannot write buffer content" );
	}
	free( buf );
}

}
}

