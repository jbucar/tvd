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
#include "demuxerimpl.h"
#include "../ts.h"
#include <util/pool/circularpool.h>
#include <util/assert.h>
#include <util/buffer.h>
#include <util/log.h>
#include <boost/foreach.hpp>

#define EXIT_MASK 0x01

namespace tuner {
namespace demuxer {
namespace ts {

/**
 * Constructor base.
 * @param count Indica la cantidad máxima de buffers a guardar en el pool.
 */
DemuxerImpl::DemuxerImpl( int count )
{
	//	Create ts pool
	_pool = new util::pool::CircularPool<util::Buffer *>( "ts.demuxer", 1000, TS_PACKET_SIZE * count );
}

/**
 * Destructor base.
 */
DemuxerImpl::~DemuxerImpl()
{
	delete _pool;
}

/**
 * Inicia el demultiplexado del transport stream.
 */
void DemuxerImpl::start() {
	_pool->enable(true);
	_thread = boost::thread( boost::bind( &DemuxerImpl::tsParser, this ) );
}

/**
 * Detiene el demultiplexado del transport stream.
 */
void DemuxerImpl::stop() {
	//	Disable pool
	_pool->enable(false);
	
	//	Signal and join thread to exit
	_pool->notify_all( EXIT_MASK );
	_thread.join();

	//	Reset demux
	reset();
}

/**
 * Aloca un buffer en el pool de datos.
 * @return Una nueva instancia de @c Buffer recién alocada.
 */
util::Buffer *DemuxerImpl::allocBuffer() {
	return _pool->alloc();
}

/**
 * Libera un buffer del pool de datos.
 * @param buf La instancia de @c Buffer a liberar.
 */
void DemuxerImpl::freeBuffer( util::Buffer *buf ) {
	_pool->free( buf );
}

/**
 * Inserta el buffer para ser demultiplexado.
 * @param buf Buffer con datos para demultiplexar.
 */
void DemuxerImpl::pushData( util::Buffer *buf ) {
	_pool->put( buf );
}

void DemuxerImpl::tsParser() {
	bool exit=false;
	util::DWORD mask;
	util::Buffer *buf=NULL;

	LINFO( "DemuxerImpl", "Begin parser thread" );

	while (!exit) {
		if (_pool->get( buf, mask )) {
			//	Parse buffer
			parse( buf );

			//	Returns buffer to pool
			freeBuffer( buf );
		}

		if (mask & EXIT_MASK) {
			exit=true; 
		}
	}

	LINFO( "DemuxerImpl", "End parser thread" );
}

}
}
}

