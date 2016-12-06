/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "ui.h"
#include "lua.h"
#include "modules/mainwindow.h"
#include <zapper/zapper.h>
#include <util/log.h>
#include <util/fs.h>
#include <util/cfg/cfg.h>
#include <boost/filesystem.hpp>

namespace luaz {

namespace impl {
	static int traceback(lua_State *L) {
		lua_getfield(L, LUA_GLOBALSINDEX, "debug");
		lua_getfield(L, -1, "traceback");

		lua_pushvalue(L, 1);
		lua_pushinteger(L, 2);
		lua_call(L, 2, 1);
		fprintf(stderr, "[luaz::UI] Error: %s \nend strack traceback\n", lua_tostring(L, -1));
		return 1;
	}
}

/**
 * Construcor base.
 * @param name Nombre a utilizar para crear el zapper.
 */
UI::UI( const std::string &name )
	: _name(name)
{
	_zapper = NULL;
	_lState = NULL;
}

/**
 * Destructor base.
 */
UI::~UI()
{
}

/**
 * Ejecuta el script de lua indicado en @b script
 * @param script Ruta absoluta al script de lua a ejecutar.
 * @return El codigo de error con el que finalizó la ejecución (0 si no hubo error).
 */
int UI::run( const std::string &script ) {
	int code=-1;

	code = initialize();
	if (!code) {
		//	Start zapper
		_zapper->start();
		_mainWindow->start();

		//	Load variables
		loadEnvironmentVars();

		//	Run lua script
		runScript( util::fs::make(lua::scriptsPath(), script) );

		//	Stop zapper
		_mainWindow->stop();
		_zapper->stop();

		code = finalize();
	}

	return code;
}

int UI::initialize() {
	LDEBUG( "UI", "Initialize" );

	//	Setup icon
	util::cfg::setValue(
		"gui.window.icon",
		util::fs::make( lua::imagesPath(), "icon.png" )
	);

	// Setup common tool envirtonment vars
	setEnvironmentVar("zapperVersion", _zapper->getVersion() );
	setEnvironmentVar("platformVersion", _zapper->getPlatformVersion() );
	setEnvironmentVar("toolVersion", util::cfg::getValue<std::string>("tool.version") );
	setEnvironmentVar("toolBuildVersion", util::cfg::getValue<std::string>("tool.buildVersion") );

	//	Setup lua
	_lState = lua::init();
	if (!_lState) {
		return -1;
	}

	//	Create zapper
	_zapper = zapper::Zapper::create( _name );
	if (!_zapper) {
		LERROR( "UI", "Cannot create zapper object" );
		return -1;
	}

	//	Initialize zapper
	if (!_zapper->initialize()) {
		int ret = _zapper->exitCode();
		DEL(_zapper);
		return ret;
	}

	//	Create and initialize main window
	_mainWindow = new MainWindow(_zapper,_lState);
	if (!_mainWindow->initialize()) {
		_zapper->finalize();
		DEL(_zapper);
		return -1;
	}

	return 0;
}

int UI::finalize() {
	LDEBUG( "UI", "Finalize" );

	//	Finalize main window
	_mainWindow->finalize();

	//	Finalize zapper!
	int ret = _zapper->exitCode();
	_zapper->finalize();

	//	Close lua
	lua_close( _lState );
	_lState = NULL;

	DEL(_mainWindow);
	DEL(_zapper);

	return ret;
}

void UI::loadEnvironmentVars() {
	// Specific tool vars
	BOOST_FOREACH( Setter set, _environmentVars ) {
		set( _lState );
	}
}

void UI::runScript( const std::string &script ) {
	//	Start script of lua
	LDEBUG( "UI", "Run script: %s", script.c_str() );

	lua_pushcfunction(_lState, impl::traceback);
	if (!luaL_loadfile(_lState, script.c_str())) {
		lua_pcall(_lState, 0, 0, lua_gettop(_lState) - 1);
	} else {
		LERROR( "UI", "%s", luaL_checkstring(_lState, -1));
	}

	LDEBUG( "UI", "Close script!" );
}

}

