/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "inputmanager.h"
#include "canvas.h"
#include "system.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

namespace canvas {

namespace input {

class Listener {
public:
	Listener( ListenerID id, const Callback &callback, const util::key::Keys &keys, int priority )
		: _id( id ), _callback( callback ), _keys( keys ), _priority( priority ), _enabled(true)
	{
		DTV_ASSERT(!_callback.empty());
	}
	virtual ~Listener() {}

	ListenerID id() const { return _id; }
	void priority( int priority ) { _priority = priority; }
	int priority() const { return _priority; }
	const util::key::Keys &keys() const { return _keys; }
	void reserveKeys( const util::key::Keys &keys ) { _keys = keys; }
	bool enabled() const { return _enabled; }
	void enable( bool en ) { _enabled = en; }
	bool canDispatch( util::key::type key, int priority ) const { return enabled() && priority <= this->priority() && hasReserved(key); }
	bool dispatchKeyDown( util::key::type key, int priority ) {
		if (canDispatch( key, priority )) {
			_dispatched.insert( key );
			_callback( key, false );
			return true;
		}
		return false;
	}
	bool dispatchKeyUp( util::key::type key ) {
		if (enabled()) {
			//	Find in keys dispatched
			util::key::Keys::iterator it = _dispatched.begin();
			while (it != _dispatched.end()) {
				if ((*it) == key) {
					_callback( key, true );
					_dispatched.erase(it);
					return true;	//	Only one time the same key
				}
				it++;
			}
		}

		return false;
	}

protected:
	bool hasReserved( util::key::type key ) const {
		return std::find( _keys.begin(), _keys.end(), key ) != _keys.end();
	}

private:
	ListenerID _id;
	Callback _callback;
	util::key::Keys _keys;
	util::key::Keys _dispatched;
	int _priority;
	bool _enabled;
};

}	//	namespace input


InputManager::InputManager( System *system )
{
	_inStandby = false;
	_sys = system;
}

InputManager::~InputManager()
{
	CLEAN_ALL( input::Listener *, _listeners );
}

/**
 * @param id Identificador del objeto @c listener que atiende el evento.
 * @param callback Función que será llamada cuándo se produzca una evento de teclado que involucre una de las teclas en @em keys.
 * @param keys Colección de teclas que serán atendidas por el objeto @c listener.
 * @param priority Prioridad que el objeto @c listener tiene para atender los eventos te teclado.
 */
void InputManager::reserveKeys( input::ListenerID id, const input::Callback &callback, const util::key::Keys &keys, int priority/*=MED_INPUT_PRIORITY*/ ) {
	LTRACE( "input", "Reserve keys: id=%p, listeners=%ld, keys=%d, priority=%d",
		id, _listeners.size(), keys.size(), priority );

	Listeners::const_iterator it=std::find_if(
		_listeners.begin(), _listeners.end(), boost::bind(&input::Listener::id,_1) == id );
	if (it == _listeners.end()) {
		_listeners.push_back( new input::Listener( id, callback, keys, priority ) );
	}
	else {
		input::Listener *l = (*it);

		//	Listener found, process keys
		if (keys.size() > 0) {
			l->enable(true);
			l->reserveKeys( keys );
		}
		else {
			l->enable( false );
		}
	}

	notifyReservationChanged();
}

void InputManager::releaseAll() {
	LTRACE( "input", "Release all keys" );
	BOOST_FOREACH( input::Listener *l, _listeners ) {
		l->enable( false );
	}
	notifyReservationChanged();
}

void InputManager::releaseKeys( input::ListenerID id ) {
	LTRACE( "input", "Release keys: id=%p", id );
	Listeners::const_iterator it=std::find_if(
		_listeners.begin(), _listeners.end(), boost::bind(&input::Listener::id,_1) == id );
	if (it != _listeners.end()) {
		(*it)->enable(false);
		notifyReservationChanged();
	}
}

input::KeySignal &InputManager::onKey() {
	return _onKey;
}

input::KeyTranslator &InputManager::onKeyTranslator() {
	return _translateKey;
}

input::KeyReservationChangedSignal &InputManager::onReservationChanged() {
	return _onKeyReservationChanged;
}

void InputManager::notifyReservationChanged() {
	util::key::Keys all;
	for (Listeners::const_iterator it=_listeners.begin(); it!=_listeners.end(); it++) {
		input::Listener *l = (*it);
		if (l->enabled()) {
			const util::key::Keys &lKeys = l->keys();
			all.insert( lKeys.begin(), lKeys.end() );
		}
	}
	_onKeyReservationChanged( all );
}

void InputManager::renice( input::ListenerID id, int priority ) {
	Listeners::const_iterator it=std::find_if(
		_listeners.begin(), _listeners.end(), boost::bind(&input::Listener::id,_1) == id );
	if (it != _listeners.end()) {
		(*it)->priority( priority );
	}
}

//	Standby/wakeup
void InputManager::standby() {
	//	In standby only send hi priority key events
	_inStandby = true;
}

void InputManager::wakeup() {
	_inStandby = false;
}

//	Buttons
input::ButtonSignal &InputManager::onButton() {
	return _onButton;
}

/**
 * Despacha un evento de teclado.
 * @param key Código de la tecla que presionada o soltada.
 * @param isUp True si el evento se generó por la pulsación de una tecla, false caso contrario.
 * @return True si se ha logrado despachar el evento, false caso contrario.
 */
bool InputManager::dispatchKey( util::key::type key, bool isUp ) {
	static int recursive=0;
	bool dispatched=false;

	LTRACE( "input", "Dispatch key: listeners=%d, key=%d, isUp=%d", _listeners.size(), key, isUp );

	//	Translate key (if necesary)
	boost::optional<util::key::type> newKey = _translateKey( key );
	if (newKey) {
		key = *newKey;
	}

	if (key == util::key::null) {
		return false;
	}

	recursive++;

	Listeners copy(_listeners);

	_sys->canvas()->beginDraw();

	//	If key press ...
	if (!isUp) {
		//	Find max priority
		int priority = 0;
		BOOST_FOREACH( input::Listener *listener, copy ) {
			if (listener->canDispatch( key, priority ) && listener->priority() > priority) {
				priority = listener->priority();
			}
		}

		if (!_inStandby || (_inStandby && priority == MAX_INPUT_PRIORITY)) {
			//	Dispatch key
			BOOST_FOREACH( input::Listener *listener, copy ) {
				dispatched |= listener->dispatchKeyDown( key, priority );
			}
		}
	}
	else {
		//	On key release ... send to all listener that handle the keyDown
		BOOST_FOREACH( input::Listener *listener, copy ) {
			dispatched |= listener->dispatchKeyUp( key );
		}
	}

	_onKey( key, isUp );

	recursive--;
	if (!recursive) {
		removeDisabled();
	}

	_sys->canvas()->endDraw();

	return dispatched;
}

void InputManager::removeDisabled() {
	Listeners::iterator it = _listeners.begin();
	while (it != _listeners.end()) {
		if (!(*it)->enabled()) {
			delete (*it);
			it = _listeners.erase(it);
		}
		else {
			it++;
		}
	}
}

void InputManager::dispatchButton( input::ButtonEvent *evt ) {
	if (!_inStandby) {
		_onButton( evt );
	}
}

}

