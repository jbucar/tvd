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
#include <canvas/types.h>
#include <util/keydefs.h>

#define KEY_GROUPS_LIST(fnc) \
	fnc(numeric_keys)	\
	fnc(colored_keys)	\
	fnc(selection_keys)	\
	fnc(cursor_keys)	\
	fnc(interactive_keys)

namespace player {
namespace keys {

#define DO_ENUM_KEY_GROUPS(t) t,
enum type {
	unknown,
	KEY_GROUPS_LIST(DO_ENUM_KEY_GROUPS)
	LAST
};
#undef DO_ENUM_KEY_GROUPS

const char *getName( type group );
type fromName( const std::string &groupName );

void addKeys( keys::type group );
util::key::type translateKey( util::key::type key );

canvas::input::KeyReservationChangedSignal &onReservationChanged();
void notifyReservationChanged( util::key::Keys keys );

void reserveKeys( const std::string &groups );
util::key::Keys reservedKeys();

void init();
void fin();

}	//	namespace keys
}
