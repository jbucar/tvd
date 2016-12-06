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

#include "luaplayer.h"
#include "settings.h"
#include "lua/lua.h"
#include "lua/event/event.h"
#include "lua/canvas/canvas.h"
#include "lua/settings/settings.h"
#include "lua/persistent/persistent.h"
#include "../property/forwardproperty.h"
#include "../device.h"
#include "../system.h"
#include <canvas/system.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>

namespace player {

namespace fs = boost::filesystem;

LuaPlayer::LuaPlayer( Device *dev )
  : GraphicPlayer( dev )
{
	_lua = NULL;
	_event = NULL;
	_canvas = NULL;
	_settings = NULL;
	_persistent = NULL;
}

LuaPlayer::~LuaPlayer()
{
	DTV_ASSERT(!_event);
	DTV_ASSERT(!_canvas);
	DTV_ASSERT(!_settings);
	DTV_ASSERT(!_persistent);
}

bool LuaPlayer::isApplication() const {
	return true;
}

bool LuaPlayer::init() {
	DTV_ASSERT( !_lua );

	//	Initialize graphic player
	if (!GraphicPlayer::init()) {
		return false;
	}

	//	Create lua state
	_lua = luaL_newstate();
	if (!_lua) {
		LWARN("LuaPlayer", "Cannot initialize lua engine");
		return false;
	}

	if (!loadModules()) {
		LWARN("LuaPlayer", "Cannot load lua modules");
		return false;
	}

	return true;
}

void LuaPlayer::fin() {
	unloadModules();
	closeLua();
	GraphicPlayer::fin();
}

void LuaPlayer::loadModule( lua_CFunction fnc, const char *name) {
	lua_pushcfunction(_lua, fnc);
	lua_pushstring(_lua, name);
	lua_call(_lua, 1, 0);
}

void LuaPlayer::loadModuleWithout( lua_CFunction fnc, const char *name, std::vector<std::string> &toExclude ) {
	loadModule(fnc, name);
	lua_getglobal(_lua, name);

	for (std::vector<std::string>::iterator it = toExclude.begin() ; it != toExclude.end(); ++it) {
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, (*it).c_str());
	}

	lua_setglobal(_lua, name);
}

bool LuaPlayer::loadModules() {
	//	Open libs
	loadModule(luaopen_base, LUA_COLIBNAME);
	loadModule(luaopen_table, LUA_TABLIBNAME);
	loadModule(luaopen_string, LUA_STRLIBNAME);
	loadModule(luaopen_math, LUA_MATHLIBNAME);
	loadModule(luaopen_io, LUA_IOLIBNAME);

	std::vector<std::string> toExclude;
	toExclude.push_back(std::string("clock"));
	toExclude.push_back(std::string("execute"));
	toExclude.push_back(std::string("exit"));
	toExclude.push_back(std::string("getenv"));
	toExclude.push_back(std::string("remove"));
	toExclude.push_back(std::string("rename"));
	toExclude.push_back(std::string("tmpname"));
	toExclude.push_back(std::string("setlocale"));

	loadModuleWithout(luaopen_os, LUA_OSLIBNAME, toExclude);

	toExclude.clear();
	toExclude.push_back("loadlib");
	loadModuleWithout(luaopen_package, LUA_LOADLIBNAME, toExclude);

	//	Initialize event/canvas modules
	_event = new event::Module( device()->systemPlayer(), this, _lua );
	_canvas = new mcanvas::Module( this, _lua );
	_settings = new settings::Module( _lua );
	_persistent = new persistent::Module( _lua );

	return true;
}

void LuaPlayer::unloadModules() {
	//	Unload event module
	DEL(_event);
	DEL(_canvas);
	DEL(_settings);
	DEL(_persistent);
}

void LuaPlayer::closeLua() {
	if (_lua) {
		lua_close( _lua );
		_lua = NULL;
	}
}

void LuaPlayer::addPath( const std::string &modulePath ) {
	lua_getglobal( _lua, "package" );
	lua_getfield( _lua, -1, "path" ); // get field "path" from table at top of stack (-1)
	if (_packagePath.empty()) {
		_packagePath = lua_tostring( _lua, -1 ); // grab path string from top of stack
	}

	std::string cur_path=_packagePath;

	{	// Add search: modulePath/?
		cur_path.append( ";" );
		fs::path path = modulePath;
		path /= "?";
		cur_path.append( path.string().c_str() );
	}

	{	// Add search: modulePath/?.lua
		cur_path.append( ";" ); // do your path magic here
		fs::path path = modulePath;
		path /= "?.lua";
		cur_path.append( path.string().c_str() );
	}

	lua_pop( _lua, 1 ); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring( _lua, cur_path.c_str() ); // push the new one
	lua_setfield( _lua, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
	lua_pop( _lua, 1 ); // get rid of package table from top of stack
}

bool LuaPlayer::startPlay( void ) {
	if (GraphicPlayer::startPlay()) {
		DTV_ASSERT(_lua);

		//	Calculate the path from lua file
		LINFO("LuaPlayer", "Start play, source=%s", body().c_str());

		//	Add path to load library
		addPath( rootPath() );

		//	Start canvas
		_canvas->start( surface() );

		//	Register button listener
		_onButton = device()->systemPlayer()->addButtonListener( boost::bind(&LuaPlayer::onBtnEvent,this,_1) );

		return true;
	}
	return false;
}

void LuaPlayer::stopPlayer( event::evtAction::type evtAction ) {
	DTV_ASSERT(_lua);
	LINFO("LuaPlayer", "Stop play");

	onPresentationEvent( evtAction );

	//	Stop canvas
	_canvas->stop();

	// Disconnect button listener
	_onButton.disconnect();

	GraphicPlayer::stopPlay();
}

void LuaPlayer::stopPlay( void ) {
	stopPlayer( event::evtAction::stop );
}

void LuaPlayer::pausePlay( bool pause ) {
	if (pause) {
		onPresentationEvent( event::evtAction::pause );
	} else {
		onPresentationEvent( event::evtAction::resume );
	}
}

void LuaPlayer::abortPlay( void ) {
	stopPlayer( event::evtAction::abort );
}

void LuaPlayer::refresh() {
	//	Load file
	if (luaL_dofile(_lua, body().c_str() )) {
		std::string desc(lua_tostring(_lua, lua_gettop(_lua)));
		if (desc.find(":") != std::string::npos) {
			std::string path = desc.substr(0,desc.find(":"));
			desc.erase(0,desc.find(":")+1);
			std::string line = desc.substr(0,desc.find(":"));
			desc.erase(0,desc.find(":")+1);
			LERROR("LuaPlayer", "Lua script '%s' at line %s, error description: %s", path.c_str(), line.c_str(), desc.c_str());
		} else {
			LERROR("LuaPlayer", "%s", desc.c_str());
		}
	}
	onPresentationEvent( event::evtAction::start );
}

void LuaPlayer::registerProperties() {
	//	Setup graphic player
	GraphicPlayer::registerProperties();

	{	//	Property attributionEvent
		ForwardProperty<AttributionEventData> *prop = new ForwardProperty<AttributionEventData>(
			boost::bind(&LuaPlayer::sendAttributionEvent,this,_1), true );
		addProperty( property::type::attributionEvent, prop );
	}

	{	//	Property presentationEvent
		ForwardProperty<std::string> *prop = new ForwardProperty<std::string>(
			boost::bind(&LuaPlayer::onPresentationEvent,this,event::evtAction::start,_1), true );
		addProperty( property::type::presentationEvent, prop );
	}

	{	//	Property selectionEvent
		ForwardProperty<SelectionEventData> *prop = new ForwardProperty<SelectionEventData>(
			boost::bind(&LuaPlayer::sendKeyEvent,this,_1) );
		addProperty( property::type::selectionEvent, prop );
	}
}

void LuaPlayer::setEventCallback( const event::Callback &callback ) {
	_event->setCallback( callback );
}

void LuaPlayer::setInputEventCallback( const event::InputCallback &callback ) {
	_event->setInputEventCallback( callback );
}

void LuaPlayer::onPresentationEvent( player::event::evtAction::type action, const std::string &label/*=""*/ ) {
	_event->dispatchPresentation( action, label );
}

void LuaPlayer::sendAttributionEvent( const AttributionEventData &data ) {
	onAttributionEvent( data.first, data.second );
}

void LuaPlayer::onAttributionEvent( const std::string &name/*=""*/, const std::string &value/*=""*/ ) {
	_event->dispatchAttribution( name, event::evtAction::start, value );
}

void LuaPlayer::sendKeyEvent( const SelectionEventData &data ) {
	onKeyEvent( data.first, data.second );
}

void LuaPlayer::onKeyEvent( util::key::type key, bool isUp ) {
	_event->dispatchKey( key, isUp );
}
void LuaPlayer::onBtnEvent( canvas::input::ButtonEvent *evt ) {
	_event->dispatchBtn( evt->isPress, evt->x, evt->y );
}

void LuaPlayer::update() {
	redraw();
}

}
