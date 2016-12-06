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
#include "nclapplication.h"
#include "nclprofile.h"
#include "../../../controller/controller.h"
#include "../../../controller/runtask.h"
#include "../../../controller/playeradapter.h"
#include "../../../extension.h"
#include "../../../../../servicemanager.h"
#include "../../../../../../resourcemanager.h"
#include "../../../../../../demuxer/psi/dsmcc/event.h"
#include "generated/config.h"
#include <connector/connector.h>
#include <connector/handler/keepalivehandler.h>
#include <connector/handler/keyregisterhandler.h>
#include <connector/handler/keyeventhandler.h>
#include <connector/handler/playvideohandler.h>
#include <connector/handler/stopvideohandler.h>
#include <connector/handler/videoresizehandler.h>
#include <connector/handler/exitcommandhandler.h>
#include <connector/handler/editingcommandhandler.h>
#include <connector/handler/startcommandhandler.h>
#include <util/task/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <set>

namespace tuner {
namespace app {

namespace bfs = boost::filesystem;

NCLApplication::NCLApplication( Extension *ext, const ApplicationID &id, const std::string &script )
	: Application( ext, id ), _script( script )
{
	name( script );
	_connector = NULL;
}

NCLApplication::~NCLApplication()
{
	DTV_ASSERT(!_connector);
}

void NCLApplication::show() const {
	Application::show();
	LINFO("app::NCLApplication", "\tscript=%s", _script.c_str());
}

const std::string &NCLApplication::script() const {
	return _script;
}

//	Aux start/stop
void NCLApplication::onStarting( RunTask *task ) {
	std::string pipe;

	LDEBUG( "app::NCLApplication", "Setup process" );

	//	Connector can be active, if so, stop process and continue
	DTV_ASSERT(!_connector);

	{	//	Make pipe
		bfs::path tmp = bfs::temp_directory_path();
		tmp /= "connector_%%%%%%";
		pipe = bfs::unique_path(tmp).string();
		LDEBUG("app::NCLApplication", "Setup process: pipe=%s", pipe.c_str());
	}

	//	Setup task
	task->cmd( NCLProfile::ginga() );
	task->params().clear();
	task->env().clear();

	//	Setup BaseID
	ID nitID = extension()->srvMgr()->currentNit();
	if (nitID != NIT_ID_RESERVED) {
		task->addParam( "base_id", boost::lexical_cast<std::string>( nitID ) );
	}

	{	//	Settings
		int w,h;
		if (ctrl()->setupLayer( this, w, h )) {
			std::string setting = "gui.use=custom:gui.window.use=remote:";

			LINFO("app::NCLApplication", "Using layer resolution: (%d,%d)", w, h);
			setting += "gui.window.size.width=" + boost::lexical_cast<std::string>(w);
			setting += ":gui.window.size.height=" + boost::lexical_cast<std::string>(h);
			task->addParam( "set", setting );
		}
	}

	//	Setup pipe
	task->addParam( "connector", pipe );

	{	//	Setup NCL name
		std::string ncl = path() + "/" + _script;
		task->addParam( "ncl", ncl );
	}

	//	-------------------------------------------------------------------------------------
	//	Setup connector!!
	//	-------------------------------------------------------------------------------------

	//	Register into dispatcher to get called
	extension()->srvMgr()->dispatcher()->registerTarget( this, "tuner::NCLApplication" );

	//	Create connector
	_connector = new connector::Connector( pipe, true );
	_connector->setDispatcher( boost::bind(&util::task::Dispatcher::post,extension()->srvMgr()->dispatcher(),this,_1) );

	//	Handle connect
	_connector->onConnect( boost::bind(&Application::started,this) );

	//	Handle error cases
	boost::function<void (void)> fnc = boost::bind(&Application::stop,this,false);
	_connector->onConnectTimeout( fnc );
	_connector->onDisconnect( fnc );
	_connector->onTimeout( fnc );

	{	//	Reserve/Release keys messages
		connector::KeyRegisterHandler *keyHandler = new connector::KeyRegisterHandler();
		keyHandler->onKeyEvent( boost::bind(&NCLApplication::onRegisterKeyEvent,this,_1) );
		_connector->addHandler( keyHandler );
	}

	{	//	Play video
		connector::PlayVideoHandler *handler = new connector::PlayVideoHandler();
		handler->onPlayVideoEvent( boost::bind(&NCLApplication::onPlayVideoEvent,this,_1,_2) );
		_connector->addHandler( handler );
	}

	{	//	Stop video
		connector::StopVideoHandler *handler = new connector::StopVideoHandler();
		handler->onStopVideoEvent( boost::bind(&NCLApplication::onStopVideoEvent,this,_1) );
		_connector->addHandler( handler );
	}

	{	//	Video resize
		connector::VideoResizeHandler<int> *videoResizeHandler = new connector::VideoResizeHandler<int>();
		videoResizeHandler->onVideoResizeEvent( boost::bind(&NCLApplication::onResizeVideoEvent,this,_1,_2) );
		_connector->addHandler( videoResizeHandler );
	}

	{	//	Setup start presentation
		connector::StartCommandHandler *handler = new connector::StartCommandHandler();
		handler->onStartPresentationEvent( boost::bind(&NCLApplication::onStartPresentation,this,_1) );
		_connector->addHandler( handler );
	}
}

void NCLApplication::onStopping() {
	DTV_ASSERT(_connector);

	//	UnRegister into dispatcher to get called
	extension()->srvMgr()->dispatcher()->unregisterTarget( this );

	//	Stop ginga
	connector::ExitCommandHandler exit;
	exit.send( _connector );
}

void NCLApplication::onStopped() {
	//	Cleanup connector
	DTV_ASSERT(_connector);
	DEL(_connector);

	destroyPlayers();

	Application::onStopped();
}

int NCLApplication::exitTimeout() const {
	return util::cfg::getValue<int>("tuner.service.ginga-ncl.timeout");
}

//	NCL Editing command
void NCLApplication::onEditingCommand( const Buffer &buf ) {
	LDEBUG("app::NCLApplication", "Editing command received: size=%d", buf.length());
	if (_connector) {
		connector::EditingCommandHandler cmd;
		cmd.send( _connector, buf );
	}
}

struct NCLEventFinder {
	NCLEventFinder( std::vector<std::string> &events ) : _events( events ) {}
	void operator()( dsmcc::Event *evt ) {
		util::WORD eventID;
		evt->show();
		if (evt->includes( "nclEditingCommand", eventID )) {
			_events.push_back( evt->url() );
		}
	}

	std::vector<std::string> &_events;

private:
	// Avoid VisualStudio warning C4512
	NCLEventFinder &operator=(NCLEventFinder &/*ef*/) { return *this; }
};

void NCLApplication::registerEditingCommands() {
	LDEBUG("app::NCLApplication", "Register editing commands");

	std::vector<std::string> names;
	NCLEventFinder iter(names);
	loopEvents( iter );
	BOOST_FOREACH( const std::string &name, names ) {
		LDEBUG("app::NCLApplication", "Editing command found: event=%s", name.c_str());
		registerEvent( name, "nclEditingCommand", boost::bind(&NCLApplication::onEditingCommand,this,_1) );
	}
}

//	Connector aux
void NCLApplication::onStartPresentation( bool start ) {
	if (start) {
		ctrl()->enableLayer(true);

		LINFO("app::NCLApplication", "Ginga presentation started");
		//	Register to all nclEditingCommand events
		registerEditingCommands();
	}
	else {
		ctrl()->enableLayer(false);
	}
}

//	Video
void NCLApplication::onPlayVideoEvent( size_t videoID, const std::string &url ) {
	if (_players.find( videoID ) != _players.end()) {
		LWARN( "app::NCLApplication", "Player ID is already created: id=%d, url=%s", videoID, url.c_str() );
		return;
	}

	PlayerAdapter *adapter;

	{	//	Parse url
		std::string tmp(url);
		boost::algorithm::to_lower(tmp);
		size_t pos=tmp.find( "://" );
		if (pos == std::string::npos) {
			adapter = ctrl()->createMediaPlayerAdapter();
		}
		else {
			std::string schema = tmp.substr( 0, pos );
			if (schema == "sbtvd-ts") {
				std::string body= tmp.substr( pos+3 );
				if (!body.compare(0,5,"video")) { //	video | video(i)
					if (body.length() > 5) {
						std::string videoBody= body.substr( 6, body.length()-6-1 );
						try {
							int id = boost::lexical_cast<int>(videoBody);
							adapter = ctrl()->createElementaryPlayerAdapter( id );
						} catch(...) {
							LWARN( "app::NCLApplication", "Invalid URL: id=%d, url=%s", videoID, url.c_str() );
							return;
						}
					}
					else {
						adapter = ctrl()->createMainPlayerAdapter();
					}
				}
				else if (!body.compare(0,1,"0")) {
					adapter = ctrl()->createMainPlayerAdapter();
				}
				else {
					//	audio | audio(i)
					//	text  | text(i)
					adapter = NULL;
				}
			}
			else {
				adapter = ctrl()->createMediaPlayerAdapter();
			}
		}
	}

	if (!adapter) {
		LWARN( "app::NCLApplication", "Player adapter not supported: id=%d, url=%s", videoID, url.c_str() );
		return;
	}

	LDEBUG( "app::NCLApplication", "Play video: videoID=%d, url=%s, adapter=%s", videoID, url.c_str(), adapter->name().c_str() );
	if (!adapter->play( url )) {
		LWARN( "app::NCLApplication", "Cannot play adapter: id=%d, url=%s", videoID, url.c_str() );
		delete adapter;
		return;
	}

	//	Add player
	_players[videoID] = adapter;
}

void NCLApplication::onStopVideoEvent( size_t videoID ) {
	Players::const_iterator it=_players.find( videoID );
	if (it == _players.end()) {
		LWARN( "app::NCLApplication", "stopVideo; Invalid player: id=%d", videoID );
		return;
	}

	LDEBUG( "app::NCLApplication", "Stop video: videoID=%d", videoID );
	PlayerAdapter *adapter = it->second;
	DTV_ASSERT(adapter);
	adapter->stop();
}

void NCLApplication::onResizeVideoEvent( size_t videoID, const connector::NCLVideoDescriptorPtr &av ) {
	Players::const_iterator it=_players.find( videoID );
	if (it == _players.end()) {
		LWARN( "app::NCLApplication", "resizeVideo; Invalid player: id=%d", videoID );
		return;
	}

	LDEBUG( "app::NCLApplication", "Resize video: videoID=%d, rect=(%d,%d,%d,%d)",
		videoID, av->getX(), av->getY(), av->getWidth(), av->getHeight() );
	PlayerAdapter *adapter = it->second;
	DTV_ASSERT(adapter);
	adapter->moveResize( av->getX(), av->getY(), av->getWidth(), av->getHeight() );
}

void NCLApplication::destroyPlayers() {
	LDEBUG( "app::NCLApplication", "Destroy players: players=%d", _players.size() );
	BOOST_FOREACH( const Players::value_type &item, _players ) {
		PlayerAdapter *adapter = item.second;
		adapter->stop();
		delete adapter;
	}
	_players.clear();
}

//	Keys
void NCLApplication::dispatchKeyImpl( util::key::type key, bool isUp ) {
	LDEBUG("app::NCLApplication", "Dispatch key: key=%d, isUp=%d", key, isUp);
	if (_connector) {
		connector::KeyEventHandler handler;
		handler.send( _connector, key, isUp );
	}
}

void NCLApplication::onRegisterKeyEvent( const connector::KeyRegisterDataPtr &data ) {
	std::vector<util::key::type> keys;
	BOOST_FOREACH( util::key::type key, *data.get() ) {
		keys.push_back( key );
	}
	reserveKeys( keys );
}

}
}
