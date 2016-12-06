/******************************************************************************

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

#include "mainwindow.h"
#include "../lua.h"
#include "canvas/canvas.h"
#include "module.h"
#include "generated/config.h"
#include <zapper/zapper.h>
#include <pvr/time.h>
#include <canvas/rect.h>
#include <canvas/surface.h>
#include <canvas/canvas.h>
#include <canvas/inputmanager.h>
#include <canvas/window.h>
#include <canvas/system.h>
#include <util/io/dispatcher.h>
#include <util/mcr.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#define BACKGROUND_LAYER 1

namespace luaz {

namespace impl {
	static MainWindow *win = NULL;
}

namespace mainwindow {

static int l_run( lua_State * /*L*/ ) {
	impl::win->runLoop();
	return 0;
}

static int l_stop( lua_State *L ) {
	int retCode=luaL_checkint(L, 1);
	impl::win->stopLoop( retCode );
	return 0;
}

static int l_lockKeys( lua_State *L ) {
	impl::win->lockKeys( luaL_checkint(L, 1) == 1 );
	return 0;
}

static int l_resetConfig( lua_State * /*L*/ ) {
	impl::win->reset();
	return 0;
}

static int l_getTime( lua_State *L ) {
	const char *tmp = luaL_checkstring(L, 1);
	DTV_ASSERT(tmp);
	return impl::win->getTime( tmp );
}

static int l_getDate( lua_State *L ) {
	const char *tmp = luaL_checkstring(L, 1);
	const int daysOffset = luaL_checkinteger(L, 2);
	DTV_ASSERT(tmp);
	return impl::win->getDate( tmp , daysOffset );
}

static int l_cancelTimer( lua_State *L ) {
	impl::win->cancelTimer( luaL_checkint(L, 1) );
	return 0;
}

static int l_registerTimer( lua_State *L ) {
	int ms  = luaL_checkint(L, 1);
	int tid = impl::win->registerTimer( ms );
	lua_pushnumber( L, tid );
	return 1;
}

static int l_showBackground( lua_State *L ) {
	int needShow = luaL_checkint(L, 1);
	impl::win->showBackground( needShow == 1 );
	return 0;
}

static int l_launchBrowser( lua_State * /*L*/ ) {
	impl::win->launchBrowser();
	return 0;
}

static int l_fullscreen( lua_State *L ) {
	impl::win->fullscreen( lua_toboolean(L, 1) != 0 );
	return 0;
}

static int l_isFullscreen( lua_State *L ) {
	int tmp = impl::win->isFullscreen() ? 1 : 0;
	lua_pushboolean( L, tmp );
	return 1;
}

static int l_iconify( lua_State *L ) {
	impl::win->iconify( lua_toboolean(L, 1) != 0 );
	return 0;
}

static int l_inStandby( lua_State *L ) {
	int tmp = impl::win->inStandby() ? 1 : 0;
	lua_pushboolean( L, tmp );
	return 1;
}

static int l_ramDisk( lua_State *L ) {
	const std::string &ramDisk = impl::win->zapper()->ramDisk();
	lua_pushstring( L, ramDisk.c_str() );
	return 1;
}

static int l_imagesPath( lua_State *L ) {
	::fs::path path = luaz::lua::imagesPath();

	if (lua_gettop(L) == 1) {
		path /= luaL_checkstring(L, 1);
	}

	lua_pushstring(L, path.string().c_str());
	return 1;
}

static const struct luaL_Reg mainwindow_methods[] = {
	{ "run",              l_run              },
	{ "stop",             l_stop             },
	{ "resetConfig",      l_resetConfig      },
	{ "lockKeys",         l_lockKeys         },
	{ "getTime",          l_getTime          },
	{ "getDatePlus",      l_getDate          },
	{ "registerTimer",    l_registerTimer    },
	{ "cancelTimer",      l_cancelTimer      },
	{ "launchBrowser",    l_launchBrowser    },
	{ "showBackground",   l_showBackground   },
	{ "inStandby",        l_inStandby        },
	{ "fullscreen",       l_fullscreen       },
	{ "isFullscreen",     l_isFullscreen     },
	{ "iconify",          l_iconify          },
	{ "ramDisk",          l_ramDisk          },
	{ "imagesPath",       l_imagesPath       },
	{ NULL,               NULL               }
};

}

struct CompareTimers {
	CompareTimers( int tID ) : _tID(tID) {}

	bool operator()( const util::id::Ident &id) {
		return id->getID() == _tID;
	}

	int _tID;
};

MainWindow::MainWindow( zapper::Zapper *zapper, lua_State *lua )
{
	_lua = lua;
	_zapper = zapper;
	_bgSurface = NULL;
	_inStandby = false;

	//	Setup MainWindow
	impl::win = this;
}

MainWindow::~MainWindow()
{
	impl::win = NULL;
}

//	Run main lua loop
void MainWindow::runLoop() {
	_zapper->run();
}

void MainWindow::stopLoop( int ret ) {
	_zapper->exit( ret );
}

bool MainWindow::initialize() {
	//	Initialize modules
	if (!canvas::init( _zapper )) {
		LWARN("MainWindow", "Cannot init canvas module");
		return false;
	}
	module::initialize( _zapper );
	_onStandbyConnection = _zapper->onStandby().connect( boost::bind(&MainWindow::onStandby,this,_1) );
	_onTimeChangedConnection = pvr::time::clock()->onTimeValidChanged().connect( boost::bind(&MainWindow::onTimeChanged,this,_1) );
	return true;
}

void MainWindow::finalize() {
	_onStandbyConnection.disconnect();
	_onTimeChangedConnection.disconnect();
	module::finalize();
	canvas::fin();
}

bool MainWindow::start() {
	//	Register lua modules
	luaL_register( _lua, "mainWindow", mainwindow::mainwindow_methods );
	canvas::start( _lua );
	module::start( _lua );

	//	Register all keys into for zapper
	registerKeys();

	//	Setup background surface
	initBgSurface();

	return true;
}

void MainWindow::stop() {
	//	destroy background surface
	sys()->canvas()->destroy( _bgSurface );

	//	Un-register lua modules
	module::stop();
}

//	Standby
bool MainWindow::inStandby() const {
	return _inStandby;
}

void MainWindow::onStandby( bool inStandby ) {
	_inStandby = inStandby;
	lua::call( _lua, "zapperOnStandby", inStandby ? 1 : 0 );
}

void MainWindow::onTimeChanged( bool isValid ) {
	lua::call( _lua, "zapperOnTimeChanged", isValid ? 1 : 0 );
}

void MainWindow::launchBrowser() {
	exit(163);
}

//	Reset config
void MainWindow::reset() {
	_zapper->resetConfig();
}

void MainWindow::onKey( util::key::type key, bool isUp ) {
	if (key != util::key::null) {
		lua::call( _lua, "OnKeyEvent", key, isUp );
	}
}

void MainWindow::lockKeys( bool needKeys ) {
	sys()->input()->renice( this, needKeys ? HI_INPUT_PRIORITY : LOW_INPUT_PRIORITY );
}

void MainWindow::registerKeys() {
	util::key::Keys keys;
#define SAVE_KEY( c, n, u ) keys.insert( static_cast<util::key::type>(util::key::c) );
	UTIL_KEY_LIST(SAVE_KEY);
#undef SAVE_KEY
	sys()->input()->reserveKeys( this, boost::bind(&MainWindow::onKey,this,_1,_2), keys, LOW_INPUT_PRIORITY );
	sys()->input()->onButton().connect( boost::bind(&MainWindow::onButtonEvent,this,_1) );

	//	Export keys to zapper
#define DO_STR(s) #s
#define EXPORT_KEY( c, n, u )	  \
	lua_pushnumber( _lua, util::key::c ); \
	lua_setglobal ( _lua, DO_STR(KEY_##u) );
	UTIL_KEY_LIST(EXPORT_KEY);
#undef EXPORT_KEY
#undef DO_STR
}

void MainWindow::onButtonEvent( ::canvas::input::ButtonEvent *evt ) {
	lua::call( _lua, "OnButtonEvent", evt->button, evt->isPress ? 1 : 0, evt->x, evt->y );
}

void MainWindow::initBgSurface() {
	::canvas::Canvas *canv= sys()->canvas();

	{	//	Add custom fonts
		boost::filesystem::path tmp = lua::fontsPath();
		canv->addFontDirectory( tmp.string() );
	}

	//	Configure layer
	const ::canvas::Rect bgRect( ::canvas::Point(0,0), sys()->canvas()->size() );
	_bgSurface = canv->createSurface( bgRect );
	_bgSurface->setZIndex( BACKGROUND_LAYER );
	_bgSurface->setVisible(false);
	_bgSurface->autoFlush(true);

	{	//	Draw bg image
		boost::filesystem::path tmp = lua::imagesPath();
		tmp /= "inicio.png";
		_bgSurface->drawImage( tmp.string() );
	}
}

void MainWindow::showBackground( bool show ) {
	_bgSurface->setVisible( show );
	sys()->canvas()->flush();
}

int MainWindow::registerTimer( int ms ) {
	util::id::Ident id = sys()->io()->addTimer(ms, boost::bind(&MainWindow::processTimer, this, _1));
	_timers.push_back(id);
	return id->getID();
}

void MainWindow::cancelTimer( int tID ) {
	ListOfTimers::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		CompareTimers(tID)
	);
	if(it != _timers.end()) {
		sys()->io()->stopTimer( (*it) );
		_timers.erase( it );
	}
}

void MainWindow::processTimer( util::id::Ident & id ) {
	//	Find timer
	ListOfTimers::iterator it=std::find_if(
		_timers.begin(), _timers.end(), CompareTimers(id->getID()) );
	if(it != _timers.end()) {
		//	Timer found, destroy timer
		_timers.erase( it );

		lua::call( _lua, "OnTimerEvent", id->getID() );
	}
}

//	Time
int MainWindow::getTime( const std::string &format ) {
	pvr::bpt::ptime now;
	pvr::time::clock()->get( now );
	char buffer[256];
	struct tm curtime = pvr::bpt::to_tm(now);
	strftime( buffer, 256, format.c_str(), &curtime );
	lua_pushstring( _lua, buffer );
	return 1;
}

// Date
int MainWindow::getDate( const std::string &format, int daysOffset ) {
	pvr::bpt::ptime now;
	pvr::time::clock()->get( now );
	char buffer[256];
	now += boost::gregorian::date_duration(daysOffset);
	struct tm dd = pvr::bpt::to_tm(now);
	strftime( buffer, 256, format.c_str(), &dd );
	lua_pushstring( _lua, buffer );
	return 1;
}

void MainWindow::fullscreen( bool enable ) {
	sys()->window()->fullScreen( enable );
}

bool MainWindow::isFullscreen() const {
	return sys()->window()->isFullScreen();
}

void MainWindow::iconify( bool enable ) {
	sys()->window()->iconify( enable );
}

::canvas::System *MainWindow::sys() const {
	return _zapper->sys();
}

zapper::Zapper *MainWindow::zapper() const {
	return _zapper;
}

}
