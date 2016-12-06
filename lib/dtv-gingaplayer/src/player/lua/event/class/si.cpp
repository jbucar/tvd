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

#include "si.h"
#include "../event.h"
#include "../../lua.h"
#include <canvas/system.h>

namespace player {
namespace event {
namespace si {

int postEvent( lua_State *st, bool /*isOut*/, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::si] Invalid event module" );
	}

	//	Get type of event
	const char* value = lua::getField( st, eventPos, "type" );
	if (!value) {
		return luaL_error( st, "[event::si] Invalid type field in event" );
	}
	siEvtType::type type = getSiEventType( value );
	if (type == siEvtType::unknown) {
		return luaL_error( st, "[event::si] Invalid type: type=%s\n", value );
	}

	dispatchSi( module, type);
	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchSi( Module *module, siEvtType::type type ) {
	EventImpl event;
	event["class"] = "si";
	event["type"] = getSiTypeName( type );
//	event["data"] = TODO
	module->dispatchIn( &event );
}

}
}
}
