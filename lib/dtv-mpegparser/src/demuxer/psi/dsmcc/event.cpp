/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "event.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {

Event::Event( const std::string &url, BYTE tag, const std::vector<EventName> &events )
	: _url( url ), _events( events )
{
	_tag = tag;
}

Event::~Event( void )
{
}

BYTE Event::tag() const {
	return _tag;
}

const std::string &Event::url() const {
	return _url;
}

bool Event::includes( const std::string &name, WORD &eventID ) const {
	std::vector<EventName>::const_iterator it=_events.begin();
	while (it != _events.end()) {
		if ((*it).name == name) {
			eventID = (*it).id;
			return true;
		}
		it++;
	}
	return false;
}

void Event::show( void ) const {
	LDEBUG( "dsmcc::Event", "Event: url=%s, tag=%02x, events=%d",
		_url.c_str(), _tag, _events.size() );

	BOOST_FOREACH( const EventName &name, _events ) {
		dsmcc::show( name );
	}
}

}
}
