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

#include "device.h"
#include "mediaplayer.h"
#include "../system.h"
#include <connector/handler/editingcommandhandler.h>
#include <connector/handler/exitcommandhandler.h>
#include <connector/handler/keepalivehandler.h>
#include <connector/handler/keyregisterhandler.h>
#include <connector/handler/keyeventhandler.h>
#include <connector/handler/buttoneventhandler.h>
#include <connector/handler/startcommandhandler.h>
#include <connector/connector.h>
#include <canvas/audio.h>
#include <canvas/player.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <canvas/factory.h>
#include <util/task/dispatcher.h>
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"

namespace player {
namespace remote {

class Factory : public canvas::Factory {
public:
	Factory() {}
	virtual ~Factory() {}

	virtual canvas::Audio *createAudio() const {
		return canvas::Audio::create("dummy");
	}

	virtual canvas::Player *createPlayer() const {
		return canvas::Player::create("dummy");
	}
};

Device::Device( System *sys, connector::Connector *conn )
	: player::Device( sys ), _connector(conn)
{
	_medias = 0;

	//	Setup dispatcher
	_connector->setDispatcher( boost::bind(&player::System::enqueue,sys,_1) );

	//	Setup disconnect
	conn->onDisconnect( boost::bind(&Device::exit,this) );

	//	Setup key reservation
	_keyRegisterHandler = new connector::KeyRegisterHandler();

	{	//	Setup editing command handler
		connector::EditingCommandHandler *handler = new connector::EditingCommandHandler();
		handler->onEditingCommandHandler(boost::bind(&Device::onConnectorEditingCommandEvent, this, _1));
		_connector->addHandler( handler );
	}

	{	//	Setup exit command handler
		connector::ExitCommandHandler *exit = new connector::ExitCommandHandler();
		exit->onExitEvent(boost::bind(&Device::exit, this));
		_connector->addHandler(exit);
	}

	{	//	Setup keys command handler
		connector::KeyEventHandler *handler = new connector::KeyEventHandler();
		handler->onKeyEvent( boost::bind( &Device::dispatchKey, this, _1, _2 ) );
		_connector->addHandler( handler );
	}

	{	//	Setup button command handler
		connector::ButtonEventHandler *handler = new connector::ButtonEventHandler();
		handler->onButtonEvent( boost::bind( &Device::dispatchButton, this, _1, _2, _3, _4 ) );
		_connector->addHandler( handler );
	}
}

Device::~Device()
{
	DEL(_connector);
}

canvas::Factory* Device::createFactory() const {
    return new Factory();
}

//	Start/stop
void Device::start() {
	player::Device::start();

	_medias = 0;
	system()->dispatcher()->registerTarget( this, "player::remote::Device" );

	connector::StartCommandHandler handler;
	handler.send( _connector, true );
}

void Device::stop() {
	player::Device::stop();

	system()->dispatcher()->unregisterTarget( this );

	{	//	Disable disconnect
		boost::function<void (void)> dummyFnc;
		_connector->onDisconnect( dummyFnc );
	}

	connector::StartCommandHandler handler;
	handler.send( _connector, false );
}

canvas::MediaPlayer *Device::createMediaPlayer() {
	return new MediaPlayer( _medias++, _connector, system() );
}

void Device::destroy( canvas::MediaPlayerPtr &mp ) {
	mp->stop();
	delete mp;
	mp = NULL;
}

void Device::onReserveKeys( const ::util::key::Keys &keys ) {
	_keyRegisterHandler->send( _connector, keys );
}

void Device::onConnectorEditingCommandEvent( const connector::EditingCommandDataPtr &data ) {
	dispatchEditingCommand( data.get() );
}

}
}

