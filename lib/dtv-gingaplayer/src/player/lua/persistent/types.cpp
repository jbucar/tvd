/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "types.h"
#include <util/log.h>
#include <stdexcept>

namespace player {
namespace persistent {
namespace dbdata {

typedef struct {
	dbdata::type type;
	const char *name;
} DBDataType;

#define DO_ENUM_DBDATA_NAMES(n,v) { dbdata::n, #v },
static DBDataType types[] = {
	DBDATA_TYPES_LIST(DO_ENUM_DBDATA_NAMES)
	{ dbdata::LAST, NULL }
};
#undef DO_ENUM_DBDATA_NAMES

type getType( const std::string &name ) {
	if (!name.empty()) {
		int i=0;
		while (types[i].name) {
			if (name == types[i].name) {
				return types[i].type;
			}
			i++;
		}
	}
	LERROR("lua::persistent::types", "Cannot get type from string. Invalid type name!");
	throw new std::runtime_error("Cannot get type from string. Invalid type name!");
}

const char *getName( type dbDataType ) {
	if (dbDataType > dbdata::LAST) {
		LERROR("lua::persistent::types", "Cannot get type name. Invalid type!");
		throw new std::runtime_error("Cannot get type name. Invalid type!");
	}
	return types[dbDataType].name;
}

} // end namespace dbdata
} // end namespace persistent
} // end namespace player
