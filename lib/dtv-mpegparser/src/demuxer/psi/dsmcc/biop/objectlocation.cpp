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
#include "objectlocation.h"
#include <util/string.h>

namespace tuner {
namespace dsmcc {
namespace biop {

ObjectLocation::ObjectLocation( void ) {
	keyID      = INVALID_KEY;
	carouselID = DWORD(-1);
	moduleID   = WORD(-1);
}

ObjectLocation::ObjectLocation( const ObjectLocation &other )
{
	keyID      = other.keyID;
	carouselID = other.carouselID;
	moduleID   = other.moduleID;
}

ObjectLocation &ObjectLocation::operator=( const ObjectLocation &other ) {
	keyID      = other.keyID;
	carouselID = other.carouselID;
	moduleID   = other.moduleID;
	return *this;
}

bool ObjectLocation::operator==( const ObjectLocation &other ) {
	return keyID == other.keyID && carouselID == other.carouselID && moduleID == other.moduleID;
}

std::string ObjectLocation::asString() const {
	return util::format( "carouselID=%08x, moduleID=%04x, key=%08x", 
		carouselID, moduleID, keyID );
}

}
}
}
