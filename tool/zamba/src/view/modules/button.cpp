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
 * button.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"
#include "../luacallback.h"
#include "wgt.h"
#include "../widget/button.h"
#include "../widget/buttonset.h"
#include "../widget/buttongrid.h"
#include "../widget/extendedbuttonset.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace wgt {

namespace button {

std::map<WidgetId, ButtonPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	ButtonPtr ww = boost::make_shared<Button>(x, y, w, h, colorMap);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int label(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);

	ButtonPtr button = _widgets[id];
	button->label(text);
	if (lua_gettop(L) == narg) {
		const int fsize = luaL_checkint(L, narg++);
		button->label()->fontSize(fsize);
	}

	return 0;
}

int icon(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* path  = luaL_checkstring(L, narg++);
	const char* aPath = luaL_checkstring(L, narg++);
	const int imgw    = luaL_checkint(L, narg++);
	const int imgh    = luaL_checkint(L, narg++);
	const int iconH = luaL_checkint(L, narg++);
	const int separation = luaL_checkint(L, narg++);

	ButtonPtr button = _widgets[id];

	if (lua_gettop(L) == 9) {
		button->icon(path, aPath, luaL_checkstring(L, narg++), imgw, imgh, iconH, separation);
	} else {
		button->icon(path, aPath, imgw, imgh, iconH, separation);
	}

	return 0;
}

int textColor(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const Color color       = Wgt::view()->getColorById( luaL_checkint( L, narg++ ) );
	const Color activeColor = Wgt::view()->getColorById( luaL_checkint( L, narg++ ) );

	ButtonPtr button = _widgets[id];
	button->setColor("textColor", color);
	button->setColor("selectedTextColor", activeColor);
	return 0;
}

int fontSize(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int size    = luaL_checkint(L, narg++);

	ButtonPtr button = _widgets[id];
	button->label()->fontSize(size);
	return 0;
}

static int enable(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int b = luaL_checkint(L, narg++);

	ButtonPtr button = _widgets[id];
	button->enable(b!=0);
	button->needsRepaint(true);

	return 0;
}

int action(lua_State* L) {
	const WidgetId id = luaL_checkint(L, 2);
	ButtonPtr button = _widgets[id];
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	button->action(boost::bind(&luaCallback, L, ref));
	return 0;
}

int selectedCallback( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 2);
	ButtonPtr button = _widgets[id];
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	button->selectedCallback(boost::bind(&luaCallback, L, ref, _1));
	return 0;
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",               create             },
			{"label",             label              },
			{"icon",              icon               },
			{"textColor",         textColor          },
			{"fontSize",          fontSize           },
			{"action",            action             },
			{"enable",            enable             },
			{"selectedCallback",  selectedCallback   },
			{NULL,                NULL               }
	};

	luaL_register(L, "button", methods);
}

} // button

namespace buttonset {

std::map<WidgetId, ButtonSetPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	ButtonSetPtr ww = boost::make_shared<ButtonSet>(x, y, w, h, colorMap);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int createExtended(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	ExtendedButtonSetPtr ww = boost::make_shared<ExtendedButtonSet>(x, y, w, h, colorMap);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int addButton(lua_State* L)
{
	// Por ahora no usa nada específico de los botones, se podría reemplazar por addChild de Window
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const WidgetId btnid = luaL_checkint(L, narg++);

	ButtonSetPtr set = _widgets[setid];
	ButtonPtr btn = button::_widgets[btnid] ;//Wgt::view()->getWidgetById(btnid);

	set->addButton(btn);

	return 0;
}

int setActiveButton(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int active  = luaL_checkint(L, narg++);

	ButtonSetPtr set = _widgets[setid];
	if (active >= 0) {
		set->selectChild(active);
	} else {
		set->deactivateChild();
	}
	set->needsRepaint(true);

	return 0;
}

static int getCurrentIndex( lua_State* L ) {
	const WidgetId wid = luaL_checkint(L, 2);
	ButtonSetPtr set = _widgets[wid];

	lua_pushnumber(L, set->currentPos());

	return 1;
}

int setCircular(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int circular   = luaL_checkint(L, narg++);

	ButtonSetPtr set = _widgets[setid];
	set->circular(circular != 0);

	return 0;
}

static int enableButton(lua_State* L) {
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int btn  = luaL_checkint(L, narg++);
	const int enable  = luaL_checkint(L, narg++);

	ButtonSetPtr set = _widgets[setid];
	set->enableChild(btn, enable == 1);
	set->needsRepaint(true);

	return 0;
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",                  create           },
			{"newExtended",          createExtended   },
			{"addButton",            addButton        },
			{"enableButton",         enableButton     },
			{"setActiveButton",      setActiveButton  },
			{"getCurrentIndex",      getCurrentIndex  },
			{"circular",             setCircular      },
			{NULL,                   NULL             }
	};

	luaL_register(L, "buttonset", methods);
}

} // buttonset

namespace buttongrid {

std::map<WidgetId, ButtonGridPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const int rowsVisible = luaL_checkint(L, narg++);
	const int rowH = luaL_checkint(L, narg++);
	const int rowSeparation = luaL_checkint(L, narg++);
	const int rows = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	ButtonGridPtr ww = boost::make_shared<ButtonGrid>(x, y, w, h, rowsVisible, rowH, rowSeparation, rows, colorMap);
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int appendRow(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	unsigned int offset = 0;

	if (lua_gettop(L) == 3) {
		offset = luaL_checkint(L, narg++);
	}

	ButtonGridPtr set = _widgets[setid];
	set->appendRow(offset);
	
	return 0;
}

int selectRow(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const unsigned int row = (unsigned int)luaL_checkint(L, narg++);
	ButtonGridPtr set = _widgets[setid];
	set->selectRow( row );
	
	return 0;
}

int addButton(lua_State* L)
{
	// Por ahora no usa nada específico de los botones, se podría reemplazar por addChild de Window
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const WidgetId btnid = luaL_checkint(L, narg++);
	const int row = luaL_checkint(L, narg++);

	ButtonGridPtr set = _widgets[setid];
	ButtonPtr btn = button::_widgets[btnid];

	set->addButton(btn, row);

	return 0;
}

int setActiveButton(lua_State* L)
{
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	const int active  = luaL_checkint(L, narg++);
	ButtonGridPtr set = _widgets[setid];

	if (lua_gettop(L) == 4) {
		set->selectRow(luaL_checkint(L, narg++));
	}

	set->selectButton(active);
	set->needsRepaint(true);

	return 0;
}

int l_scrollImages( lua_State* L ) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char *up = luaL_checkstring(L, narg++);
	const char *down = luaL_checkstring(L, narg++);

	_widgets[id]->scrollbarArrows(up, down);

	return 0;
}

int getActiveButton( lua_State* L ) {
	int narg = 2;
	const WidgetId setid = luaL_checkint(L, narg++);
	ButtonGridPtr set = _widgets[setid];
	int row = set->activeRow();
	int col = set->activeColumn();
	lua_pushnumber(L, col);
	lua_pushnumber(L, row);
	
	return 2;
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",             create},
			{"appendRow",       appendRow},
			{"selectRow",       selectRow},
			{"addButton",       addButton},
			{"setActiveButton", setActiveButton},
			{"getActiveButton", getActiveButton},
			{"scrollImages",    l_scrollImages},
			{NULL,  NULL}
	};

	luaL_register(L, "buttongrid", methods);
}

} // buttongrid

} // wgt
