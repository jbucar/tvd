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

#include "ident.h"
#include "pool.h"
#include "../assert.h"
#include <stdlib.h>

namespace util {
namespace id {

bool isValid( const Ident &id ) {
	return id.use_count() > 0 && id->isValid();
}

IdentType::IdentType( Pool *pool, ID_TYPE id )
	: _pool(pool), _id( id )
{
	if (id >= 0) { DTV_ASSERT(pool); }
}

IdentType::IdentType( const IdentType &other )
{
	copy( other );
}

IdentType::~IdentType()
{
	reset();
}

void IdentType::reset() {
	if (_id >= 0) {
		DTV_ASSERT(_pool);
		_pool->free( _id );
		_id = -1;
	}
}

bool IdentType::isValid() const {
	return _id >= 0;
}

void IdentType::copy( const IdentType &other ) {
	reset();
	_pool = other._pool;
	_id = other._id;
}

}
}

