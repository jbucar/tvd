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
 * combobox.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/combobox.h"

#include <boost/make_shared.hpp>

namespace wgt {

namespace combobox {

std::map<WidgetId, ComboBoxPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );
	const char* title = luaL_checkstring(L, narg++);
	const int titleFontSize = luaL_checkint(L, narg++);
	std::vector<std::string> items;
	luaz::lua::readList(L, narg++, items);
	const int itemsFontSize = luaL_checkint(L, narg++);
	const int items_w = luaL_checkint(L, narg++);

	ComboBoxPtr ww = ComboBoxPtr(new ComboBox(x, y, w, h, colorMap, title, titleFontSize, items, itemsFontSize, items_w));
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int action(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* f     = luaL_checkstring(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->action(std::string(f));
	return 0;
}

int close(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->close();
	return 0;
}

int getSelected(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	int selected = combo->getIndex();
	lua_pushnumber(L, selected);
	return 1;
}

int setSelected(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const unsigned int ix = luaL_checkint(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->setIndex(ix);

	return 0;
}

int setItems(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<std::string> items;
	luaz::lua::readList(L, narg++, items);

	ComboBoxPtr combo = _widgets[id];
	combo->items(items);

	return 0;
}

int setMaxLines(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int max = luaL_checkint(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->maxLines(max);

	return 0;
}

int setTempItem(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* item  = luaL_checkstring(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->setTempItem(std::string(item));

	return 0;
}

int hideTempItem(lua_State* L) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ComboBoxPtr combo = _widgets[id];
	combo->hideTempItem();

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

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",          create},
			{"action",       action},
			{"close",        close},
			{"getSelected",  getSelected},
			{"setSelected",  setSelected},
			{"setItems",     setItems},
			{"setMaxLines",  setMaxLines},
			{"setTempItem",  setTempItem},
			{"hideTempItem", hideTempItem},
			{"scrollImages", l_scrollImages},
			{NULL,  NULL}
	};

	luaL_register(L, "combobox", methods);
}

} // combobox

} // wgt
