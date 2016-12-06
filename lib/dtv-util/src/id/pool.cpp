/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "pool.h"
#include "../log.h"
#include <boost/foreach.hpp>
#include <stdexcept>

namespace util {
namespace id {

Pool::Pool( const std::string &name )
	: _name(name)
{
	_first = 0;
	_last = _first;
	_max = 32768;
}

Pool::Pool( const std::string &name, ID_TYPE first, ID_TYPE max )
	: _name(name)
{
	_first = first;
	_last = first;
	_max = max;
}

Pool::~Pool()
{
	if (_ids.size() > 0) {
		LWARN("id::Pool", "Pool(%s), some ids are not returned: count=%d", _name.c_str(), _ids.size());
		BOOST_FOREACH( const ID_TYPE &val, _ids ) {
			LDEBUG("id::Pool", "\tID=%ld", val );
		}
	}
}

struct IDFinder {
	IDFinder( ID_TYPE id ) : _id(id) {}

	bool operator()( ID_TYPE id ) const  {
		return id == _id;
	}

	ID_TYPE _id;
};
	
Ident Pool::alloc() {
	ID_TYPE id;

	_mutex.lock();
	if (_last > _max) {
		id = -1;
		for (ID_TYPE i=_first; i<_max; i++) {
			ListOfID::const_iterator it=std::find_if(
				_ids.begin(),
				_ids.end(),
				IDFinder( i )
			);
			if (it == _ids.end()) {
				id = i;
				break;
			}
		}

		if (id == -1) {
			_mutex.unlock();
			throw std::runtime_error("Cannot allocate identifier, no more identifiers available");
		}
	}
	else {
		id = _last;
		_last++;
	}

	_ids.push_back( id );

	_mutex.unlock();

	LTRACE("id::Pool", "Pool(%s), Alloc id: id=%ld", _name.c_str(), id);
	return Ident( new IdentType( (Pool *)this, id ) );
}

void Pool::free( ID_TYPE id ) {
	_mutex.lock();
	ListOfID::iterator it=std::find_if(
		_ids.begin(),
		_ids.end(),
		IDFinder( id )
	);
	if (it != _ids.end()) {
		LTRACE("id::Pool", "Pool(%s), Free id: id=%ld", _name.c_str(), id );
		_ids.erase( it );
	}
	_mutex.unlock();	
}

}
}
