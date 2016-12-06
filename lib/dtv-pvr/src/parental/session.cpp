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

#include "session.h"
#include "control.h"
#include <util/settings/settings.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

//	Settings keys
#define SESSION_PASS       "session.pass"
#define SESSION_EXPIRATION "session.timeExpiration"
#define SESSION_SEX        "session.control.sex"
#define SESSION_VIOLENCE   "session.control.violence"
#define SESSION_DRUGS      "session.control.drugs"
#define SESSION_AGE        "session.control.age"

namespace pvr {
namespace parental {

/**
 * Construye un objeto @c pvr::control::Service.
 */
Session::Session( util::Settings *s )
{
	DTV_ASSERT(s);

	_settings = s;
	_expireInSeconds = -1;	//	Never expire
	_expired = true;
	_restrictSex = _restrictViolence = _restrictDrugs = false;
	_restrictAge = 0;

	//	Load configuration
	loadConfig();
}

/**
 * Destruye la instancia de la clase @c pvr::control::Service.
 */
Session::~Session() {
}

//	Configuration
void Session::loadConfig() {
	{	//	Setup password
		std::string key;
		settings()->get( SESSION_PASS, key );
		pass( key );
	}

	{	//	Setup time expireation
		int time = -1;
		settings()->get( SESSION_EXPIRATION, time );
		timeExpiration( time );
	}

	{	//	Load sex restriction
		bool restrict = false;
		_settings->get( SESSION_SEX, restrict );
		restrictSex( restrict );
	}

	{	//	Load drugs restriction
		bool restrict = false;
		_settings->get( SESSION_DRUGS, restrict );
		restrictDrugs( restrict );
	}

	{	//	Load violence restriction
		bool restrict = false;
		_settings->get( SESSION_VIOLENCE, restrict );
		restrictViolence( restrict );
	}

	{	//	Load age restriction
		int restrict = 0;
		_settings->get( SESSION_AGE, restrict );
		restrictAge( restrict );
	}

	expire();
}

void Session::resetConfig() {
	pass( "" );
	timeExpiration( -1 );
}

/**
 * Retorna si el control parental está activo o no, esto depende de que la contraseña de control haya sido configurada.
 * @return True si el control parental está activo, false caso contrario.
 */
bool Session::isEnabled() const {
	return _pass.empty() ? false : true;
}

Session::EnabledChangedSignal &Session::onEnabledChanged() {
	return _enabledChanged;
}

/**
 * Para que la sesión esté activa tiene que estar habilitada, además de no estar bloqueada, lo cual significa que no debe haber
 * transcurrido más de @em x milisegundos desde que se comprobó el PIN. Esos @em x milisegundos son configurables
 * mediante el método @c timeExpiration.
 * @return True si la sesión está bloqueada, false caso contrario.
 */
bool Session::isBlocked() {
	if (isEnabled()) {
		return isSessionExpired();
	}
	return false;
}

/**
 * @return Señal que se emite cuándo la session se activa o expira.
 */
Session::ActiveChangedSignal &Session::onActiveChanged() {
	return _activeChanged;
}

void Session::expire() {
	if (!_expired) {
		LDEBUG( "Session", "Expire session!" );
		_expired = true;
		if (isEnabled()) {
			_activeChanged( false );
		}
	}
}

void Session::unblockSession() {
	_lastValidation = bpt::second_clock::local_time();
	if (_expired) {
		_expired = false;
		if (isEnabled()) {
			LDEBUG( "Session", "Unblock session!" );
			_activeChanged( true );
		}
	}
}

bool Session::isSessionExpired() {
	if (!_expired) {
		if (_expireInSeconds != -1) {
			bpt::ptime t1 = bpt::second_clock::local_time();
			if (t1 != _lastValidation) {
				bpt::time_duration dur;
				if (t1 < _lastValidation) {
					dur = _lastValidation-t1;
				}
				else {
					dur = t1-_lastValidation;
				}
				if (dur.total_seconds() >= _expireInSeconds) {
					expire();
				}
			}
		}
	}
	LTRACE( "Session", "Session is expired: result=%d", _expired );
	return _expired;
}

/**
 * Constata que el pin sea igual a @em pass, si ésto es así se activa la sesión. Si la sesión está activa y el pin no es
 * igual a @em pass la sesión es cerrada.
 * @param pass String con el pin a chequear.
 * @return True si el usuario se ha autenticado con éxito, false caso contrario.
 */
bool Session::check( const std::string &pass ) {
	if (_pass == pass) {
		unblockSession();
		return true;
	}
	else if (!_expired) {	//	Close session
		expire();
	}
	return false;
}

#define RESET_RESTRICTION(restriction, set, value) \
	restriction = value; \
	settings()->put( SESSION_##set, value ); \

/**
 * Configura la contraseña para la autenticación de sesión.
 * @param pass Nueva contraseña.
 */
void Session::pass( const std::string &pass ) {
	DTV_ASSERT( _pass.empty() || !isBlocked() );

	bool oldState = isEnabled();
	if (_pass != pass) {
		//	Save
		_pass = pass;
		settings()->put( SESSION_PASS, _pass );
		settings()->commit();

		bool newState = isEnabled();
		if (oldState != newState) {
			if (!newState) { //	Disabled?
				//	Reset parental control
				RESET_RESTRICTION(_restrictSex, SEX, false)
				RESET_RESTRICTION(_restrictViolence, VIOLENCE, false)
				RESET_RESTRICTION(_restrictDrugs, DRUGS, false)
				RESET_RESTRICTION(_restrictAge, AGE, 0)
			}
			else {	//	Enabled?
				_expired = false;
				unblockSession();
			}

			_enabledChanged( newState );
		}
	}
}

/**
 * Configura la duración de la sesión.
 * @param expireInSeconds Nuevo tiempo de duración de la sesión.
 */
void Session::timeExpiration( int expireInSeconds ) {
	if (expireInSeconds != _expireInSeconds) {
		_expireInSeconds = expireInSeconds;
		settings()->put( SESSION_EXPIRATION, _expireInSeconds );
		settings()->commit();
	}
}

/**
 * @return El tiempo en segundos hasta que expira la sesión.
 */
int Session::timeExpiration() {
	return _expireInSeconds;
}

//	Parental control
#define SET_RESTRICTION(var,set)	  \
	if (var != restrict) { \
		DTV_ASSERT(!isBlocked()); \
		var = restrict; \
		_settings->put( SESSION_##set, restrict ); \
		_settings->commit(); \
		_lastValidation = bpt::second_clock::local_time(); \
	}

void Session::restrictSex( bool restrict ) {
	SET_RESTRICTION(_restrictSex,SEX);
}

bool Session::isSexRestricted() const {
	return _restrictSex;
}

void Session::restrictViolence( bool restrict ) {
	SET_RESTRICTION(_restrictViolence,VIOLENCE);
}

bool Session::isViolenceRestricted() const {
	return _restrictViolence;
}

void Session::restrictDrugs( bool restrict ) {
	SET_RESTRICTION(_restrictDrugs,DRUGS);
}

bool Session::isDrugsRestricted() const {
	return _restrictDrugs;
}

void Session::restrictAge( int restrict ) {
	SET_RESTRICTION(_restrictAge,AGE);
}

int Session::ageRestricted() const {
	return _restrictAge;
}

bool Session::isAllowed( Control *c ) {
	bool ret = true;
	if (isBlocked()) {
		ret &= !(_restrictSex && c->hasSex());
		ret &= !(_restrictViolence && c->hasViolence());
		ret &= !(_restrictDrugs && c->hasDrugs());

		ret &= !_restrictAge || (_restrictAge > c->age());
	}
	return ret;
}

util::Settings *Session::settings() const {
	return _settings;
}

}
}

