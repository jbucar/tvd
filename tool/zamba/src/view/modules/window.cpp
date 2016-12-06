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

/*
 * window.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/borderedwindow.h"
#include "../widget/frame.h"

#include <boost/make_shared.hpp>

namespace wgt {

namespace window {

std::map<WidgetId, WindowPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	int args = lua_gettop(L);
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	unsigned short zIndex = (unsigned short)luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	WindowPtr window;
	if (args == 7) {
		window = boost::make_shared<Window>(x, y, w, h, colorMap);
	} else if (args == 8) {
		const int border_w     = luaL_checkint(L, narg++);
		window = boost::make_shared<BorderedWindow>(x, y, w, h, colorMap, border_w);
	}
	window->zIndex(zIndex);
	WidgetId id = Wgt::view()->addWidget(window);
	_widgets[id] = window;

	lua_pushnumber(L, id);
	return 1;
}

int createFrame(lua_State* L)
{
	int narg = 2;
	const int outterx = luaL_checkint(L, narg++);
	const int outtery = luaL_checkint(L, narg++);
	const int outterw = luaL_checkint(L, narg++);
	const int outterh = luaL_checkint(L, narg++);
	const int innerx  = luaL_checkint(L, narg++);
	const int innery  = luaL_checkint(L, narg++);
	const int innerw  = luaL_checkint(L, narg++);
	const int innerh  = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	WindowPtr window = boost::make_shared<Frame>(outterx, outtery, outterw, outterh, innerx, innery, innerw, innerh, colorMap);
	WidgetId id = Wgt::view()->addWidget(window);
	_widgets[id] = window;

	lua_pushnumber(L, id);
	return 1;
}

int addChild(lua_State* L)
{
	int narg = 2;
	const WidgetId fid = luaL_checkint(L, narg++);
	const WidgetId sid = luaL_checkint(L, narg++);

	WindowPtr father = _widgets[fid];
	WidgetPtr son  = Wgt::view()->getWidgetById(sid);

	father->addChild(son);

	return 0;
}

int open(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	WindowPtr window = _widgets[id];
	window->open();

	return 0;
}

int close(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	WindowPtr window = _widgets[id];
	window->close();

	return 0;
}

static int activeChild( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 2);
	WindowPtr window = _widgets[id];

	if (lua_gettop(L) == 3) {
		window->selectChild((size_t) luaL_checkint(L, 3));
		return 0;
	} else {
		lua_pushnumber(L, window->getActiveIndex());
		return 1;
	}
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",      create},
			{"newFrame", createFrame},
			{"addChild", addChild},
			{"activeChild", activeChild},
			{"open",     open},
			{"close",    close},
			{NULL,  NULL}
	};

	luaL_register(L, "window", methods);
}

} // window

} // wgt
