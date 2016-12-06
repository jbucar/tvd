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
#include "sdt.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {

Sdt::Sdt( Version version, ID tsID, ID nitID, bool actualTS, std::vector<Service> &services )
{
	_version  = version;
	_tsID     = tsID;
	_nitID    = nitID;
	_actual   = actualTS;
	_services = services;
}

Sdt::~Sdt( void )
{
}

//  Getters
Version Sdt::version() const {
	return _version;
}

ID Sdt::tsID() const {
	return _tsID;
}

ID Sdt::nitID() const {
	return _nitID;
}

bool Sdt::actual() const {
	return _actual;
}

const std::vector<Sdt::Service> &Sdt::services() const {
	return _services;
}

void Sdt::show() const {
	LDEBUG( "Sdt", "SDT: version=%04x, tsID=%04x, nitID=%04x, actual=%d", _version, _tsID, _nitID, _actual );
	BOOST_FOREACH( Sdt::Service srv, _services ) {
		LDEBUG( "Sdt", "\tserviceID=%04x eit (schedule=%d,present&following=%d), status=%x, ca=%d, Descriptors=%s",
			srv.serviceID, srv.eitSchedule, srv.eitPresentFollowing, srv.status, srv.caMode, srv.descriptors.show().c_str() );
	}
}

}
