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

#include "keys.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace player {
namespace keys {

namespace impl {

class Singleton {
public:
	Singleton() {}
	~Singleton() {}

	canvas::input::KeyReservationChangedSignal onKeyReservationChanged;
	util::key::Keys keys;

	static void init() {
		fin();
		_obj = new impl::Singleton();
	}

	static void fin() {
		DEL(_obj);
	}

	static Singleton *ptr() {
		DTV_ASSERT(_obj);
		return _obj;
	}

private:
	static Singleton *_obj;
};

Singleton *Singleton::_obj = NULL;

}

void init() {
	impl::Singleton::init();
}

void fin() {
	impl::Singleton::fin();
}

typedef struct {
	type groupType;
	const char *name;
} KeyGroupType;

#define DO_ENUM_KEY_GROUPS_NAMES(n) { n, #n },
static KeyGroupType keyGroupsNames[] = {
	{ unknown, "unknown" },
	KEY_GROUPS_LIST(DO_ENUM_KEY_GROUPS_NAMES)
	{ LAST, NULL }
};
#undef DO_ENUM_KEY_GROUPS_NAMES

const char *getName( type group ) {
	if (group > LAST) {
		group=unknown;
	}
	return keyGroupsNames[group].name;
}

type fromName( const std::string &groupName ) {
	if (!groupName.empty()) {
		int i=0;
		while (keyGroupsNames[i].name) {
			if (groupName == keyGroupsNames[i].name) {
				return keyGroupsNames[i].groupType;
			}
			i++;
		}
	}
 	return unknown;
}

canvas::input::KeyReservationChangedSignal &onReservationChanged() {
	return impl::Singleton::ptr()->onKeyReservationChanged;
}

void notifyReservationChanged( util::key::Keys keys ) {
	impl::Singleton::ptr()->onKeyReservationChanged( keys );
}

util::key::Keys reservedKeys() {
	return impl::Singleton::ptr()->keys;
}

void reserveKeys( const std::string &groups ) {
	impl::Singleton::ptr()->keys.clear();

	std::vector<std::string> tokens;
	boost::split( tokens, groups, boost::is_any_of( "," ) );

	BOOST_FOREACH( std::string &groupName, tokens ) {
		boost::trim(groupName);
		keys::type group = keys::fromName( groupName );
		if ( group != unknown ) {
			addKeys( group );
		} else {
			LERROR("Player::settings", "invalid key group, name=%s", groupName.c_str() );
		}
	}
	if (impl::Singleton::ptr()->keys.size() > 0) {
		keys::notifyReservationChanged( impl::Singleton::ptr()->keys );
	}
}

void addKeys( keys::type group ) {
	LDEBUG("Player::settings", "adding key group: %s", keys::getName( group ));
	switch (group) {
		case numeric_keys: {
			impl::Singleton::ptr()->keys.insert( util::key::number_0 );
			impl::Singleton::ptr()->keys.insert( util::key::number_1 );
			impl::Singleton::ptr()->keys.insert( util::key::number_2 );
			impl::Singleton::ptr()->keys.insert( util::key::number_3 );
			impl::Singleton::ptr()->keys.insert( util::key::number_4 );
			impl::Singleton::ptr()->keys.insert( util::key::number_5 );
			impl::Singleton::ptr()->keys.insert( util::key::number_6 );
			impl::Singleton::ptr()->keys.insert( util::key::number_7 );
			impl::Singleton::ptr()->keys.insert( util::key::number_8 );
			impl::Singleton::ptr()->keys.insert( util::key::number_9 );
			break;
		}
		case colored_keys: {
			impl::Singleton::ptr()->keys.insert( util::key::f1 );
			impl::Singleton::ptr()->keys.insert( util::key::f2 );
			impl::Singleton::ptr()->keys.insert( util::key::f3 );
			impl::Singleton::ptr()->keys.insert( util::key::f4 );
			impl::Singleton::ptr()->keys.insert( util::key::red );
			impl::Singleton::ptr()->keys.insert( util::key::green );
			impl::Singleton::ptr()->keys.insert( util::key::yellow );
			impl::Singleton::ptr()->keys.insert( util::key::blue );
			break;
		}
		case selection_keys: {
			impl::Singleton::ptr()->keys.insert( ::util::key::ok );
			impl::Singleton::ptr()->keys.insert( ::util::key::back );
			impl::Singleton::ptr()->keys.insert( ::util::key::exit );
			impl::Singleton::ptr()->keys.insert( ::util::key::enter );
			impl::Singleton::ptr()->keys.insert( ::util::key::backspace );
			impl::Singleton::ptr()->keys.insert( ::util::key::escape );
			break;
		}
		case cursor_keys: {
			impl::Singleton::ptr()->keys.insert( ::util::key::cursor_left );
			impl::Singleton::ptr()->keys.insert( ::util::key::cursor_right );
			impl::Singleton::ptr()->keys.insert( ::util::key::cursor_up );
			impl::Singleton::ptr()->keys.insert( ::util::key::cursor_down );
			break;
		}
		case interactive_keys: {
			addKeys( colored_keys );
			addKeys( selection_keys );
			addKeys( cursor_keys );
			break;
		}
		default:
			break;
	};
}

util::key::type translateKey( util::key::type key ) {
	switch(key) {
		case util::key::f1:
			key = util::key::red;
			break;
		case util::key::f2:
			key = util::key::green;
			break;
		case util::key::f3:
			key = util::key::yellow;
			break;
		case util::key::f4:
			key = util::key::blue;
			break;
		case util::key::escape:
			key = util::key::exit;
			break;
		case util::key::backspace:
			key = util::key::back;
			break;
		case util::key::enter:
			key = util::key::ok;
			break;
		default:
			break;
	};
	return key;
}

}
}
