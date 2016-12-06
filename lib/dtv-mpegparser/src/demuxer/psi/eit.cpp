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

#include "eit.h"
#include "psi.h"
#include <util/mcr.h>
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {

Eit::Eit( Version version, ID tsID, ID nitID, ID serviceID, ID tableID, ID lastTableID, eit::Events &events )
{
	_events.swap( events );
	_version     = version;
	_tsID        = tsID;
	_tableID     = tableID;
	_lastTableID = lastTableID;
	_serviceID   = serviceID;
	_nitID       = nitID;
}

Eit::~Eit( void )
{
	CLEAN_ALL( eit::Event *, _events );
}

//  Getters
bool Eit::isPresentFollowing() const {
	return _tableID == PSI_TID_EIT_PF_ACTUAL || _tableID == PSI_TID_EIT_PF_OTHER;
}

bool Eit::isActual() const {
	return _tableID == PSI_TID_EIT_PF_ACTUAL
	|| (_tableID >= PSI_TID_EIT_SBF_ACTUAL && _tableID <= PSI_TID_EIT_SEL_ACTUAL);
}

Version Eit::version() const {
	return _version;
}

ID Eit::lastTableID() const {
	return _lastTableID;
}

ID Eit::tableID() const {
	return _tableID;
}

ID Eit::serviceID() const {
	return _serviceID;
}

ID Eit::tsID() const {
	return _tsID;
}

ID Eit::nitID() const {
	return _nitID;
}

const eit::Events &Eit::events() const {
	return _events;
}

void Eit::show() const {
	LDEBUG( "EIT", "tableID=%04x, serviceID=%04x, tsID=%04x, nitID=%04x, version=%04x, events=%d",
		_tableID, _serviceID, _tsID, _nitID, _version, _events.size() );

	BOOST_FOREACH( eit::Event *ev, _events ) {
		if (ev) {
			LDEBUG( "EIT", "\tid=%04x, start=%s, duration=%s, status=%02x, caMode=%d, descs=%s",
				ev->eventID, ev->startTime.asString().c_str(), ev->duration.asString().c_str(),
				ev->status, ev->caMode, ev->descs.show().c_str() );
		}
	}
}

}

