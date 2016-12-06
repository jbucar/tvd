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

#include "tcp.h"
#include "../event.h"
#include "../../lua.h"
#include <util/id/ident.h>
#include <boost/lexical_cast.hpp>

namespace player {
namespace event {
namespace tcp {

int postEvent( lua_State * st, bool /*isOut*/, int eventPos ) {
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::tcp] Invalid event module" );
	}

	const char *value = lua::getField( st, eventPos, "type" );
	if (!value) {
		return luaL_error( st, "[event::tcp] Invalid type" );
	}
	else if (strcmp(value,"connect") == 0) {
		const char *host = lua::getField( st, eventPos, "host" );
		if (!host) {
			return luaL_error( st, "[event::tcp] Invalid host" );
		}

		const char *port = lua::getField( st, eventPos, "port" );
		if (!port) {
			return luaL_error( st, "[event::tcp] Invalid port" );
		}

		EventImpl event;
		event["class"] = "tcp";
		event["type"] =	"connect";
		event["host"] = host;
		event["port"] = port;

		util::id::Ident id;
		
		value = lua::getField( st, eventPos, "timeout" );
		if (value) {
			id = module->connect( host, port, boost::lexical_cast<util::DWORD>(value) );
		} else {
			id = module->connect( host, port );
		}
		
		if (util::id::isValid(id)) {
			event["connection"] = boost::lexical_cast<std::string>(id->getID());
		} else {
			event["error"] = "Connection failed";
		}

		module->dispatchIn( &event );

	} else if (strcmp(value,"disconnect") == 0) {
		value = lua::getField( st, eventPos, "connection" );
	
		if (!value) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
		
		bool res = module->disconnect(boost::lexical_cast<util::id::ID_TYPE>(value));
		if (!res) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
	}
	else if (strcmp(value,"data") == 0) {
		bool result;
		
		//	Get connection
		value = lua::getField( st, eventPos, "connection" );
		if (!value) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
		util::id::ID_TYPE conn=boost::lexical_cast<util::id::ID_TYPE>(value);

		//	Get value
		value = lua::getField( st, eventPos, "value" );
		if (!value) {
			return luaL_error( st, "[event::tcp] Invalid value" );
		}
		std::string val=value;

		//	Get timeout
		value = lua::getField( st, eventPos, "timeout" );
		if (value) {
			result = module->send( conn, val, boost::lexical_cast<util::DWORD>(value) );
		} else {
			result = module->send( conn, val );
		}

		if (!result) {
			return luaL_error( st, "[event::tcp] Error, cannot send data: value=%s", val.c_str() );
		}
	}
	else {
		return luaL_error( st, "[event::tcp] Invalid type: type=%s", value );
	}

	return 1;
}

void onDataReceived( Module *module, const std::string &val, int socketID) {
	EventImpl event;
	event["class"] = "tcp";
	event["type"] =	"data";
	event["connection"] = boost::lexical_cast<std::string>(socketID);
	event["value"] = val;
	module->dispatchIn( &event );
}

}
}
}
