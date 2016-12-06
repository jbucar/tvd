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

#include "cache.h"
#include <util/pool/pool.h>
#include <util/buffer.h>
#include <util/log.h>
#include <algorithm>
#include <boost/foreach.hpp>

namespace tuner {
namespace psi {

//	TODO: Review and test cases!

Cache::Cache( util::pool::Pool *pool )
	: _pool(pool)
{
}

Cache::~Cache()
{
}

bool Cache::load() {
	clear();
	return true;
}

void Cache::unload() {
	clear();
}

void Cache::setNetwork( size_t nIndex ) {
	_currentNetwork = nIndex;
}

void Cache::clear() {
	for (size_t net=0; net<_cache.size(); net++) {
		clear( net );
	}
	_cache.clear();
}

void Cache::clear( size_t nIndex ) {
	impl::CacheItemType &data = _cache[nIndex];
	BOOST_FOREACH( const impl::CacheItemType::value_type &bItem, data ) {
		_pool->free( bItem.second );
	}
	data.clear();
}

util::Buffer *Cache::get( ID pid ) {
	util::Buffer *tmp=NULL;

	impl::CacheType::const_iterator it=_cache.find( _currentNetwork );
	if (it != _cache.end()) {
		const impl::CacheItemType &data = (*it).second;
		impl::CacheItemType::const_iterator iit=data.find( pid );
		if (iit != data.end()) {
			tmp=iit->second;
		}
	}

	LDEBUG( "psi::Cache", "Get from cache: pid=%04x, ptr=%p", pid, tmp );

	return tmp;
}

void Cache::put( ID pid, BYTE *ptr, int size ) {
	util::Buffer *buf = _pool->alloc();
	if (buf) {
		LDEBUG( "psi::Cache", "Store in cache: pid=%04x, size=%d, bufSize=%d",
			pid, size, buf->length() );

		buf->copy( ptr, size );
		_cache[_currentNetwork][pid] = buf;
	}
}

}
}

