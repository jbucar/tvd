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
 * verticalset.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/verticalset.h"

#include <boost/make_shared.hpp>

namespace wgt {

namespace verticalset {

std::map<WidgetId, VerticalSetPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	VerticalSetPtr ww = boost::make_shared<VerticalSet>(x,y,w,h,colorMap);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int addWidget(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const WidgetId wgtid = luaL_checkint(L, narg++);

	VerticalSetPtr set = _widgets[setid];
	WidgetPtr widget   = Wgt::view()->getWidgetById(wgtid);

	set->addWidget(widget);

	return 0;
}

int addChild(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const WidgetId wgtid = luaL_checkint(L, narg++);
	VerticalSetPtr set = _widgets[setid];
	WidgetPtr widget   = Wgt::view()->getWidgetById(wgtid);

	if (lua_gettop(L) == 3) {
		set->addChild(widget);
	} else if (lua_gettop(L) == 4) {
		const bool canHandleKey = luaL_checkint(L, narg++) != 0;
		set->addChild(widget, canHandleKey);
	}

	return 0;
}

int setActiveItem(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int active  = luaL_checkint(L, narg++);

	VerticalSetPtr set = _widgets[setid];

	WidgetPtr child = set->activeChild();
	child->needsRepaint(true);

	set->selectChild(active);

	child = set->activeChild();
	child->needsRepaint(true);

	return 0;
}

int setCircular(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int circular   = luaL_checkint(L, narg++);

	VerticalSetPtr set = _widgets[setid];
	set->circular(circular != 0);

	return 0;
}

int enableWidget(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int wgt  = luaL_checkint(L, narg++);
	const int enable  = luaL_checkint(L, narg++);

	VerticalSetPtr set = _widgets[setid];
	set->enableChild(wgt, enable);
	set->rawNeedsRepaint(true);

	return 0;
}

int currentWidget(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);

	VerticalSetPtr set = _widgets[setid];
	WidgetPtr wgt = set->activeChild();
	
	lua_pushnumber(L, wgt->id());

	return 1;
}


void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",           create},
			{"addWidget",     addWidget},
			{"addChild",      addChild},
			{"setActiveItem", setActiveItem},
			{"circular",      setCircular},
			{"enableWidget",  enableWidget},
			{"currentWidget",  currentWidget},
			{NULL,  NULL}
	};

	luaL_register(L, "verticalset", methods);
}

} // verticalset

} // wgt
