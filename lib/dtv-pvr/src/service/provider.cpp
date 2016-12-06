/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "provider.h"
#include <util/settings/settings.h>
#include <util/log.h>

namespace pvr {
namespace service {

Provider::Provider( const std::string &name, const std::string &version )
	: _name(name), _version(version), _enabled(false)
{
}

Provider::~Provider()
{
}

bool Provider::start() {
	//	Load config
	_enabled = defaultState();
	settings()->get( storeName(), _enabled );

	LDEBUG( "Service", "Start provider: name=%s, state=%d", _name.c_str(), _enabled );
	//	start if necesary
	if (_enabled) {
		return doStart();
	}
	return true;
}

void Provider::stop() {
	if (_enabled) {
		doStop();
	}
}

bool Provider::isEnabled() const {
	return _enabled;
}

void Provider::enable( bool val ) {
	if (val != _enabled) {
		LDEBUG( "Service", "Enable provider: name=%s, val=%d",
			_name.c_str(), val );

		_enabled = val;
		settings()->put( storeName(), val );
		settings()->commit();

		if (val) {
			doStart();
		}
		else {
			doStop();
		}
	}
}

void Provider::resetConfig() {
	enable( defaultState() );
}

const std::string &Provider::name() const {
	return _name;
}

const std::string &Provider::version() const {
	return _version;
}

bool Provider::doStart() {
	return true;
}

void Provider::doStop() {
}

const std::string Provider::storeName() const {
	return _name + ".enabled";
}

bool Provider::defaultState() const {
	return false;
}

//	Getters
util::Settings *Provider::settings() const {
	return NULL;
}

}
}

