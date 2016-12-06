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
#include "applicationid.h"
#include <boost/lexical_cast.hpp>

namespace tuner {
namespace app {

ApplicationID::ApplicationID()
{
	_orgID = 0;
	_appID = 0;
}

ApplicationID::ApplicationID( util::DWORD orgID, ID appID )
{
	_orgID = orgID;
	_appID = appID;
}

ApplicationID::~ApplicationID()
{
}

ApplicationID &ApplicationID::operator=( const ApplicationID &other ) {
	_orgID = other._orgID;
	_appID = other._appID;
	return (*this);
}

bool ApplicationID::operator==( const ApplicationID &other ) const {
	return _orgID == other._orgID && (_appID == other._appID ||
		_appID == 0xFFFF ||
		other._appID == 0xFFFF ||
		(_appID == 0xFFFE && other.isSigned()) ||
		(other._appID == 0xFFFE && isSigned()));
}

DWORD ApplicationID::orgID() const {
	return _orgID;
}

ID ApplicationID::appID() const {
	return _appID;
}

bool ApplicationID::makeID( const std::string &dotID, ApplicationID &id ) {
	size_t index=dotID.find( "." );
	if (index == std::string::npos) {
		return false;
	}


	util::DWORD orgID;
	{
		std::string tmp = dotID.substr( 0, index );
		if (tmp.empty()) {
			return false;
		}
		orgID = boost::lexical_cast<util::DWORD>(tmp);
	}

	ID appID;
	{
		std::string tmp = dotID.substr( index+1 );
		if (tmp.empty()) {
			return false;
		}
		appID = boost::lexical_cast<ID>(tmp);
	}

	id = ApplicationID( orgID, appID );
	return true;
}

std::string ApplicationID::asString() const {
	std::string id = boost::lexical_cast<std::string>( orgID() );
	id += ".";
	id += boost::lexical_cast<std::string>( appID() );
	return id;
}

bool ApplicationID::isUnsigned() const {
	return _appID <= 0x3FFF;
}

bool ApplicationID::isSigned() const {
	return _appID >= 0x4000 && _appID <= 0x7FFF;
}

bool ApplicationID::isWildcard() const {
	return _appID == 0xFFFe || _appID == 0xFFFF;
}

}
}
