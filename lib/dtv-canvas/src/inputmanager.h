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

#pragma once

#include "types.h"

namespace canvas {

namespace input {
	class Listener;
}

class System;

class InputManager {
public:
	explicit InputManager( System *system );
	virtual ~InputManager();

	//	Keys
	void reserveKeys( input::ListenerID id, const input::Callback &callback, const util::key::Keys &keys, int priority=MED_INPUT_PRIORITY );
	void releaseKeys( input::ListenerID id );
	void releaseAll();
	void renice( input::ListenerID id, int priority );
	input::KeyReservationChangedSignal &onReservationChanged();
	input::KeySignal &onKey();
	input::KeyTranslator &onKeyTranslator();

	//	Buttons
	input::ButtonSignal &onButton();

	//	Standby/wakeup
	void standby();
	void wakeup();

	//	Implementation (User do not use it!!)
	bool dispatchKey( util::key::type key, bool isUp );
	void dispatchButton( input::ButtonEvent *evt );

protected:
	typedef std::list<input::Listener *> Listeners;

	void notifyReservationChanged();
	void removeDisabled();

private:
	Listeners _listeners;
	bool _inStandby;
	System *_sys;
	input::KeySignal _onKey;
	input::KeyTranslator _translateKey;
	input::KeyReservationChangedSignal _onKeyReservationChanged;
	input::ButtonSignal _onButton;
};

}

