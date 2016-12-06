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

#include "ncl.h"
#include "../../lua.h"

namespace player {
namespace event {
namespace ncl {

int postEvent( lua_State *st, bool isOut, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::ncl] Invalid event module" );		
	}

	//	Get type of event
	const char* value = lua::getField( st, eventPos, "type" );
	if (!value) {
		return luaL_error( st, "[event::ncl] Invalid type field in event" );
	}
	nclEvtType::type type = getNclEventType( value );
	if (type == nclEvtType::unknown) {
		return luaL_error( st, "[event::ncl] Invalid type: type=%s\n", value );		
	}

	//	Get action
	value = lua::getField( st, eventPos, "action" );
	if (!value) {
		return luaL_error( st, "[event::ncl] Invalid action field in event" );
	}
	evtAction::type action = getEventAction( value );
	if (action == evtAction::unknown){
		return luaL_error( st, "[event::ncl] Invalid action: action=%s", value );
	}

	switch (type) {
		case nclEvtType::presentation: {
			std::string label = "";
			value = lua::getField( st, eventPos, "label" );
			if (value) {
				label=value;
			}

			if (isOut){
				module->dispatchOut( nclEvtType::presentation, action, label, "" );
			}else{
				dispatchPresentation( module, action, label );
			}

			break;
		}
		case nclEvtType::attribution: {
			const char *name = lua::getField( st, eventPos, "name" );
			if (!name) {
				return luaL_error( st, "[event::ncl] Invalid name field in event" );
			}

			value = lua::getField( st, eventPos, "value" );
			if (!value) {
				return luaL_error( st, "[event::ncl] Invalid value field in event" );
			}

			if (isOut){
				module->dispatchOut( nclEvtType::attribution, action, name, value );
			}else{
				dispatchAttribution( module, name, action, value );
			}

			break;
		}
		case nclEvtType::selection: {
			value = lua::getField( st, eventPos, "label" );
			if (!value) {
				return luaL_error( st, "[event::ncl] Invalid label field in event" );
			}
			module->dispatchOut( nclEvtType::selection, action, value, "" );
			break;
		}
		default:
			break;
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchPresentation( Module *module, evtAction::type action, const std::string &label ) {
	EventImpl event;
	event["class"] = "ncl";
	event["type"] = "presentation";
	event["label"] = label;
	event["action"] = getActionName( action );
	module->dispatchIn( &event );
}

void dispatchAttribution( Module *module, const std::string &name, evtAction::type action, const std::string &value ) {
	EventImpl event;
	event["class"] = "ncl";
	event["type"] = "attribution";
	event["name"] = name;
	event["action"] = getActionName( action );
	event["value"] = value;
	module->dispatchIn( &event );	
}	

}
}
}
