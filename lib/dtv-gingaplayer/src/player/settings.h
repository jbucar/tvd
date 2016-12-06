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

#pragma once

#include <string>

namespace player {
namespace settings {

class Module;

namespace prefix {
#define PREFIX_LIST(fnc) \
	fnc( system, "system" ) \
	fnc( user, "user" ) \
	fnc( defaults, "default" ) \
	fnc( service, "service" ) \
	fnc( si, "si" ) \
	fnc( channel, "channel" ) \
	fnc( shared, "shared" ) \

#define DO_ENUM_PREFIX(t, s) t,
enum type
{
	unknown = 0,
	PREFIX_LIST(DO_ENUM_PREFIX)
	LAST
};
#undef DO_ENUM_PREFIX
}

void load( void );
void unload( void );

void addListener( Module *module );
void delListener( Module *module );

void setProperty( const std::string &name, const std::string &value, bool init=false );
const std::string getProperty( const std::string &name );

}
}
