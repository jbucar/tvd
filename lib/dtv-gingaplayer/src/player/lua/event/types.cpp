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

namespace player {
namespace event {

#define DO_ENUM_TYPE_STRING( t ) #t,
static const char* nclTypes[] = {
    "NULL",
    NCL_TYPE_LIST(DO_ENUM_TYPE_STRING)
    "LAST"
};

static const char* siTypes[] = {
    "NULL",
    SI_TYPE_LIST(DO_ENUM_TYPE_STRING)
    "LAST"
};
#undef DO_ENUM_TYPE_STRING

#define DO_ENUM_ACTION_STRING( a ) #a,
static const char* actions[] = {
    "NULL",
    ACTION_LIST(DO_ENUM_ACTION_STRING)
    "LAST"
};

static const char* siTypeNames[] = {
    "NULL",
    SI_TYPE_LIST(DO_ENUM_ACTION_STRING)
    "LAST"
};
#undef DO_ENUM_PROPERTY_STRING

nclEvtType::type getNclEventType( const char *name ) {
	for (int i=0; i<nclEvtType::LAST; i++) {
		if (!strncmp( nclTypes[i], name, strlen(nclTypes[i]) )) {
			return (nclEvtType::type)i;
		}
	}
	return nclEvtType::unknown;
}

siEvtType::type getSiEventType( const char *name ) {
	for (int i=0; i<siEvtType::LAST; i++) {
		if (!strncmp( siTypes[i], name, strlen(siTypes[i]) )) {
			return (siEvtType::type)i;
		}
	}
	return siEvtType::unknown;
}

evtAction::type getEventAction( const char *name ){
	for (int i=0; i<evtAction::LAST; i++) {
		if (!strncmp( actions[i], name, strlen(actions[i]) )) {
			return (evtAction::type)i;
		}
	}
	return evtAction::unknown;
}

const char *getActionName( evtAction::type action ) {
	if (action > evtAction::LAST) {
		action=evtAction::unknown;
	}
	return actions[action];
}

const char *getSiTypeName( siEvtType::type type ) {
if (type > siEvtType::LAST) {
		type=siEvtType::unknown;
	}
	return siTypeNames[type];
}
}
}
