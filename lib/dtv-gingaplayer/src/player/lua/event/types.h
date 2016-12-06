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

#include <boost/function.hpp>
#include <string>
#include <map>

namespace player {
namespace event {

//	NCL class event types
#define NCL_TYPE_LIST(fnc)	\
	fnc( presentation )	\
	fnc( selection )	\
	fnc( attribution )

//	SI class event types
#define SI_TYPE_LIST(fnc)	\
	fnc( services )	\
	fnc( mosaic )	\
	fnc( epg )		\
	fnc( time )

namespace nclEvtType {
#define DO_ENUM_TYPE(t) t,
enum type {
	unknown = 0,
	NCL_TYPE_LIST(DO_ENUM_TYPE)
	LAST
};
#undef DO_ENUM_TYPE
}

namespace siEvtType {
#define DO_ENUM_TYPE(t) t,
enum type {
	unknown = 0,
	SI_TYPE_LIST(DO_ENUM_TYPE)
	LAST
};
#undef DO_ENUM_TYPE
}

//	Event action
#define ACTION_LIST(fnc)	\
	fnc( start )	\
	fnc( stop )		\
	fnc( abort )	\
	fnc( pause )	\
	fnc( resume )

namespace evtAction {
#define DO_ENUM_ACTION(a) a,
enum type {
	unknown = 0,
	ACTION_LIST(DO_ENUM_ACTION)
	LAST
};
#undef DO_ENUM_ACTION
}

//	Event
typedef std::map<std::string, std::string> EventImpl;

//	Callbacks
typedef boost::function<void(
	nclEvtType::type type,
	evtAction::type action,
	const std::string &parameter,
	const std::string &value)> Callback;

typedef boost::function<void ( EventImpl * )> InputCallback;

class Module;

//	getters
nclEvtType::type getNclEventType( const char *name );
evtAction::type getEventAction( const char *name );
siEvtType::type getSiEventType( const char *name );
const char *getActionName( evtAction::type action );
const char *getSiTypeName( siEvtType::type type );

}
}
