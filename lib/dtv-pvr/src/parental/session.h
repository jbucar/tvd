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

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>

namespace util {
	class Settings;
}

namespace pvr {
namespace parental {

namespace bpt = boost::posix_time;
class Control;

/**
 * Esta clase tiene como finalidad configurar los parámetros de control parental para que luego los canales y programas
 * exhibidos se ajusten a ellos.
 */
class Session {
public:
	explicit Session( util::Settings *s );
	virtual ~Session();

	//	Enable session
	void pass( const std::string &pass );
	bool isEnabled() const;
	typedef boost::signals2::signal<void (bool)> EnabledChangedSignal;
	EnabledChangedSignal &onEnabledChanged();

	//	Check session/expiration
	bool check( const std::string &pass );
	bool isBlocked();
	void expire();
	typedef boost::signals2::signal<void (bool)> ActiveChangedSignal;
	ActiveChangedSignal &onActiveChanged();

	//	Time expiration
	int timeExpiration();
	void timeExpiration( int expireInSeconds );

	//	Parental control
	bool isAllowed( Control * );
	void restrictSex( bool restrict );
	bool isSexRestricted() const;
	void restrictViolence( bool restrict );
	bool isViolenceRestricted() const;
	void restrictDrugs( bool restrict );
	bool isDrugsRestricted() const;
	void restrictAge( int age );
	int ageRestricted() const;

	//	Aux methods
	void resetConfig();

protected:
	void unblockSession();
	bool isSessionExpired();

	//	Storage
	void loadConfig();
	util::Settings *settings() const;

private:
	util::Settings *_settings;
	std::string _pass;
	bool _expired;
	int _expireInSeconds;
	bpt::ptime _lastValidation;
	EnabledChangedSignal _enabledChanged;
	ActiveChangedSignal _activeChanged;
	bool _restrictSex, _restrictViolence, _restrictDrugs;
	int _restrictAge;
};

}
}

