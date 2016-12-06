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

#pragma once

#include <canvas/types.h>
#include <util/id/ident.h>
#include <boost/function.hpp>
#include <string>
#include <list>
#include <queue>

typedef struct lua_State lua_State;

namespace util {
	class Task;
}

namespace canvas {
	class System;
	class Timer;
	class Surface;
}

namespace zapper {
	class Zapper;
}

namespace luaz {

class MainWindow {
public:
	MainWindow( zapper::Zapper *zapper, lua_State *st );
	virtual ~MainWindow();

	//	Initialize/finalize
	bool initialize();
	void finalize();
	bool start();
	void stop();

	//	Run main lua loop
	void runLoop();
	void stopLoop( int ret );

	//	Keys
	void lockKeys( bool needKeys );

	//	Background
	void showBackground( bool show );

	//	Reset config
	void reset();

	//	Time
	int getTime( const std::string &format );
	int getDate( const std::string &format, int daysOffset );

	//	Timers
	int registerTimer( int ms );
	void cancelTimer( int tID );

	//	Web browser
	void launchBrowser();

	//	Window functions
	void fullscreen( bool enable );
	bool isFullscreen() const;
	void iconify( bool enable );

	//	Standby
	bool inStandby() const;

	// Accessors
	zapper::Zapper *zapper() const;

protected:
	//	Types
	typedef std::list<util::id::Ident>  ListOfTimers;

	//	Aux
	void registerKeys();
	void onKey( util::key::type key, bool isUp );
	void onButtonEvent( canvas::input::ButtonEvent *evt );
	void registerModules();
	void processTimer( util::id::Ident &id );
	void initBgSurface();
	void onStandby( bool inStandby );
	void onTimeChanged( bool isValid );
	canvas::System *sys() const;

private:
	zapper::Zapper *_zapper;
	lua_State *_lua;
	ListOfTimers _timers;
	canvas::Surface *_bgSurface;
	boost::signals2::connection _onStandbyConnection;
	boost::signals2::connection _onTimeChangedConnection;
	bool _inStandby;
};

}
