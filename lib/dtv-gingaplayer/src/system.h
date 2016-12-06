/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <canvas/types.h>
#include <canvas/size.h>
#include <util/id/ident.h>
#include <string>
#include <vector>

namespace connector {
	class EditingCommandData;
}

namespace canvas {
	class System;
}

namespace player {

class Device;

/**
 * @c System actúa como una fachada para el objeto @c canvas::System.
 * Además tiene como responsabilidad contener y administrar a los objetos @c Devices y registrar objetos listeners para
 * eventos de teclado y entrada/salida.
 */
class System {
public:
	System();
	virtual ~System();

	//	Start/stop system
	bool canRun() const;
	void run();
	void exit();

	//	Device handling
	void addDevice( Device *dev );
	void destroy( Device *dev );
	void destroy();
	Device *getDevice( const std::string &device, int devNumber ) const;

	//	Enqueue task into player thread
	void enqueue( const boost::function<void (void)> &func );

	//	Input key
	bool dispatchKey( ::util::key::type key, bool isUp );
	bool addInputListener( canvas::input::ListenerID id, const canvas::input::Callback &callback, const ::util::key::Keys &keys );
	bool delInputListener( canvas::input::ListenerID id );

	//	Input button
	void dispatchButton( canvas::input::ButtonEvent *evt );
	typedef boost::function<void (canvas::input::ButtonEvent *ev)> ButtonCallback;
	boost::signals2::connection addButtonListener(const ButtonCallback &callback) ;

	//	Editing command
	typedef boost::function<void (connector::EditingCommandData *event)> OnEditingCommand;
	void onEditingCommand( const OnEditingCommand &callback );
	void dispatchEditingCommand( connector::EditingCommandData *event );

	//	Timers
	util::id::Ident registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback );
	void unregisterTimer( util::id::Ident &id );

	//	IO events;
	util::id::Ident addIO( util::DESCRIPTOR fd, const canvas::EventCallback &callback );
	void stopIO( util::id::Ident &id );

	//	Socket events
	util::id::Ident addSocket( util::SOCKET sock, const canvas::EventCallback &callback );
	void stopSocket( util::id::Ident &id );

protected:
	canvas::System *canvasSystem() const;
	void destroyDevice( Device *dev );
	void onReserveKeys( const ::util::key::Keys &keys );
	void onRunTask( canvas::Canvas *canvas, const boost::function<void (void)> &task );

private:
	std::vector<Device *> _devices;
	OnEditingCommand _onEditingCommand;
};

}

