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
 * list.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/list.h"

#include <boost/make_shared.hpp>

namespace wgt {

namespace list {
std::map<WidgetId, ListPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const int row_sep = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	ListPtr list = boost::make_shared<List>(x, y, w, h, 22, row_sep, colorMap, 14);
	WidgetId id = Wgt::view()->addWidget(list);
	_widgets[id] = list;
	lua_pushnumber(L, id);

	return 1;
}

int setVisibleRows(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int n       = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
	list->setVisibleRows(n);

	return 0;
}

int l_setCellAlignments( lua_State* L ) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<int> alignments;
	luaz::lua::readList(L, narg++, alignments);

	ListPtr list = _widgets[id];
	list->cellAlignments(alignments);

	return 0;
}

int setCellWidths(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<int> widths;
	luaz::lua::readList(L, narg++, widths);

	ListPtr list = _widgets[id];
 	list->cellWidths(widths);

 	return 0;
}

int setCellHeaders(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<std::string> headers;
	luaz::lua::readList(L, narg++, headers);

	ListPtr list = _widgets[id];
 	list->cellHeaders(headers);

 	return 0;
}

int setValues(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<std::string> column_keys;
	std::vector<std::string> indices;
	std::vector<std::vector<std::string> > values;

	luaz::lua::readList(L, narg++, column_keys);
	const char* index_key = luaL_checkstring(L, narg++);

	lua_pushnil(L);
	while (lua_next(L, narg) != 0) {
		std::vector<std::string> row;

		// regular columns
		for (std::vector<std::string>::const_iterator it = column_keys.begin(); it != column_keys.end(); ++it) {
			lua_getfield(L, -1, (*it).c_str());
			if (!lua_isnil(L, -1)) {
				char *btext = (char *)luaL_checkstring(L, -1);
				row.push_back(btext);
			} else {
				row.push_back("");
			}
			lua_pop(L, 1);
		}

		// value
		lua_pushstring(L, index_key);
		lua_gettable(L, -2);
		const char* value = luaL_checkstring(L, -1);
		lua_pop(L, 1);
		indices.push_back(std::string(value));

		values.push_back(row);
		lua_pop(L, 1);
	}

	ListPtr list = _widgets[id];
 	list->contents(values, indices);

	return 0;
}

int updateRow(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int row_ix  = luaL_checkint(L, narg++);
	std::vector<std::string> row;
	std::vector<std::string> column_keys;
	luaz::lua::readList(L, narg++, column_keys);
	const char* index_key = luaL_checkstring(L, narg++);

	// regular columns
	for (std::vector<std::string>::const_iterator it = column_keys.begin(); it != column_keys.end(); ++it) {
		lua_getfield(L, -1, (*it).c_str());
		if (!lua_isnil(L, -1)) {
			char *btext = (char *)luaL_checkstring(L, -1);
			row.push_back(btext);
		} else {
			row.push_back("");
		}
		lua_pop(L, 1);
	}
	// value
	lua_pushstring(L, index_key);
	lua_gettable(L, -2);
	const char* value = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	ListPtr list = _widgets[id];
	list->updateRow(row_ix, row, std::string(value));
	list->needsRepaint(true);
	
	return 0;
}

static int autoscroll(lua_State* L) {
	const WidgetId id = luaL_checkint(L, 2);
	const bool scroll  = luaz::lua::checkBool(L, 3);

	ListPtr list = _widgets[id];
	list->autoscroll(scroll);

	return 0;
}

int updateScrollbar(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int flag    = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
	list->setUpdateScrollbar(flag != 0);

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

int addRow(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<std::string> row;
	std::vector<std::string> column_keys;
	luaz::lua::readList(L, narg++, column_keys);
	const char* index_key = luaL_checkstring(L, narg++);

	// regular columns
	for (std::vector<std::string>::const_iterator it = column_keys.begin(); it != column_keys.end(); ++it) {
		lua_getfield(L, -1, (*it).c_str());
		if (!lua_isnil(L, -1)) {
			char *btext = (char *)luaL_checkstring(L, -1);
			row.push_back(btext);
		} else {
			row.push_back("");
		}
		lua_pop(L, 1);
	}
	// value
	lua_pushstring(L, index_key);
	lua_gettable(L, -2);
	const char* value = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	ListPtr list = _widgets[id];
	list->addRow(row, value);
	list->needsRepaint(true);
	return 0;
}

int setSelected(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int value   = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
 	list->selected(value);
	list->needsRepaint(true);

	return 0;
}

int getSelected(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
 	lua_pushnumber(L, list->selected());

	return 1;
}

int getCurrentValue(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
 	lua_pushstring(L, list->value().c_str());

	return 1;
}

int addImage(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* key   = luaL_checkstring(L, narg++);
	const char* value = luaL_checkstring(L, narg++);

	ListPtr list = _widgets[id];
 	list->addImage(key, value);

	return 0;
}

int mapColumn(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int column  = luaL_checkint(L, narg++);
	const char* from  = luaL_checkstring(L, narg++);
	const char* toN   = luaL_checkstring(L, narg++);
	const char* toS   = luaL_checkstring(L, narg++);

	ListPtr list = _widgets[id];
	list->addColumnMapping(column, std::string(from), std::string(toN), std::string(toS));

	return 0;
}

int getRowCount(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	ListPtr list = _widgets[id];
 	lua_pushnumber(L, list->contents().size());

	return 1;
}

int cellColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const Color color = Wgt::view()->getColorById(luaL_checkint(L, narg++));
	const Color selectedColor = Wgt::view()->getColorById(luaL_checkint(L, narg++));

	ListPtr list = _widgets[id];
	list->setColor( "cellColor", color );
	list->setColor( "selectedCellColor", selectedColor );

	return 0;
}

int textColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const Color color = Wgt::view()->getColorById(luaL_checkint(L, narg++));
	const Color selectedColor = Wgt::view()->getColorById(luaL_checkint(L, narg++));

	ListPtr list = _widgets[id];
	list->setColor( "textColor", color );
	list->setColor( "selectedTextColor", selectedColor );

	return 0;
}

int headerColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const Color color = Wgt::view()->getColorById(luaL_checkint(L, narg++));
	const Color textColor = Wgt::view()->getColorById(luaL_checkint(L, narg++));

	ListPtr list = _widgets[id];
	list->setColor( "headerColor", color );
	list->setColor( "textHeaderColor", textColor );

	return 0;
}

void init( lua_State *L ) {
	const struct luaL_Reg methods[] = {
		{"new",               create},
		{"setVisibleRows",    setVisibleRows},
		{"setValues",         setValues},
		{"updateRow",         updateRow},
		{"updateScrollbar",   updateScrollbar},
		{"autoscroll",        autoscroll},
		{"addRow",            addRow},
		{"setSelected",       setSelected},
		{"setCellWidths",     setCellWidths},
		{"setCellHeaders",    setCellHeaders},
		{"setCellAlignments", l_setCellAlignments},
		{"selected",          getSelected},
		{"addImage",          addImage},
		{"mapColumn",         mapColumn},
		{"getCurrentValue",   getCurrentValue},
		{"getRowCount",       getRowCount},
		{"cellColor",         cellColor},
		{"textColor",         textColor},
		{"headerColor",       headerColor},
		{"scrollImages",      l_scrollImages},
		{NULL,  NULL}
	};
	luaL_register(L, "list", methods);
}

} // list

}
