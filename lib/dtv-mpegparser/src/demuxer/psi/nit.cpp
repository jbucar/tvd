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
#include "nit.h"
#include "psi.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {

Nit::Nit( Version version, ID networkID, bool isActualNetwork, desc::Descriptors &descriptors, std::vector<ElementaryInfo> &elements )
{
	_version     = version;
	_networkID   = networkID;
	_descriptors = descriptors;
	_elements    = elements;
	_isActual    = isActualNetwork;
}

Nit::~Nit( void )
{
}

//  Getters
Version Nit::version() const {
	return _version;
}

ID Nit::networkID() const {
	return _networkID;
}

desc::Descriptors &Nit::descriptors() {
	return _descriptors;
}

std::vector<Nit::ElementaryInfo> &Nit::elements() {
	return _elements;
}

ID Nit::getNetwork( ID ts ) const {
	BOOST_FOREACH( Nit::ElementaryInfo info, _elements ) {
		if (info.tsID == ts) {
			return info.nitID;
		}
	}
	return NIT_ID_RESERVED;
}

bool Nit::actual() const {
	return _isActual;
}

void Nit::show() const {
	LDEBUG( "Nit", "NIT: ver=%04x, nitID=%04x, Network Descriptors=%s",
		version(), networkID(), _descriptors.show().c_str() );

	BOOST_FOREACH( const Nit::ElementaryInfo &info, _elements ) {
		LDEBUG( "Nit", "\telement: tsID=%04x, nitID=%04x, TS Descriptors=%s",
			info.tsID, info.nitID, info.descriptors.show().c_str() );
	}
}

}
