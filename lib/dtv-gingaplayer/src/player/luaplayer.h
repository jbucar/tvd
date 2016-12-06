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

#include "graphicplayer.h"
#include <util/keydefs.h>
#include <vector>

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace player {

namespace mcanvas {
	class Module;
}

namespace settings {
	class Module;
}

namespace persistent {
	class Module;
}

class LuaPlayer: public GraphicPlayer {
public:
	LuaPlayer( Device *dev );
	virtual ~LuaPlayer();

	virtual bool isApplication() const;

	//	Callback initialization
	virtual void setEventCallback( const event::Callback &callback );
	virtual void setInputEventCallback( const event::InputCallback &callback );

	void update();

protected:
	//	Types
	typedef std::pair<std::string,std::string> AttributionEventData;
	typedef std::pair<util::key::type,bool> SelectionEventData;
	
	//	Virtual methods
	virtual bool init();
	virtual void fin();
	virtual bool startPlay( void );
	virtual void stopPlay( void );
	virtual void pausePlay( bool pause );
	virtual void abortPlay( void );
	virtual void refresh();
	virtual void registerProperties();

	//	Events
	void onPresentationEvent( event::evtAction::type action, const std::string &label="" );
	void sendAttributionEvent( const AttributionEventData &data );
	void onAttributionEvent( const std::string &name="", const std::string &value="" );
	void sendKeyEvent( const SelectionEventData &data );
	void onKeyEvent( util::key::type key, bool isUp );
	void onBtnEvent( canvas::input::ButtonEvent *evt );

	//	Aux lua
	bool loadModules();
	void loadModule( lua_CFunction fnc, const char *name);
	void loadModuleWithout( lua_CFunction fnc, const char *name, std::vector<std::string> &toExclude );
	void unloadModules();
	void stopPlayer( event::evtAction::type evtAction );
	void closeLua();
	void addPath( const std::string &path );

private:
	lua_State *_lua;
	event::Module *_event;
	mcanvas::Module *_canvas;
	settings::Module *_settings;
	persistent::Module *_persistent;
	std::string _packagePath;
	boost::signals2::connection _onButton;
};

}
