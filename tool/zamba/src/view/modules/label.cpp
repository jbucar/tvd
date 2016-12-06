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
 * label.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/label.h"
#include "../widget/maskedlabel.h"

#include <boost/make_shared.hpp>

namespace wgt {
namespace label {

std::map<WidgetId, LabelPtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	int args = lua_gettop(L);
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	const char* text        = luaL_checkstring(L, narg++);
	const int fontSize      = luaL_checkint(L, narg++);
	const int alignment     = luaL_checkint(L, narg++);

	LabelPtr ww;
	if (args == 9) {
			ww = LabelPtr(new Label(x, y, w, h, colorMap, text, fontSize, alignment));
	}else { // lua_gettop(L) == 11
		const int margin        = luaL_checkint(L, narg++);
		ww = LabelPtr(new Label(x, y, w, h, colorMap, text, fontSize, alignment, margin));
	}

	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int createMasked(lua_State* L)
{
	int narg = 2;
	int args = lua_gettop(L);
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	const char* text        = luaL_checkstring(L, narg++);
	const int fontSize      = luaL_checkint(L, narg++);
	const int alignment     = luaL_checkint(L, narg++);

	LabelPtr ww;
	if (args == 9) {
		ww = MaskedLabelPtr(new MaskedLabel(x, y, w, h, colorMap, text, fontSize, alignment));
	}else { // lua_gettop(L) == 11
		const int margin        = luaL_checkint(L, narg++);
		ww = MaskedLabelPtr(new MaskedLabel(x, y, w, h, colorMap, text, fontSize, alignment, margin));
	}

	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int setColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const ColorId  c  = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->setColor("textColor", Wgt::view()->getColorById(c) );
	widget->needsRepaint(true);

	return 0;
}


int setBgColor(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const ColorId  c  = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->setColor("bgLabelColor", Wgt::view()->getColorById(c) );
	widget->needsRepaint(true);

	return 0;
}

int setText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->text(text);
	widget->needsRepaint(true);

	return 0;
}

int getText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	lua_pushstring(L, widget->text().c_str());

	return 1;
}

int reset(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->reset();

	return 0;
}

int setMask(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* mask  = luaL_checkstring(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->mask(mask);

	return 0;
}

int setEmptyMask(lua_State* L)
{
	int narg = 2;
	const WidgetId id     = luaL_checkint(L, narg++);
	const char* emptyMask = luaL_checkstring(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->emptyMask(emptyMask);

	return 0;
}

int setTextLength(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int length  = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	widget->textLength(length);

	return 0;
}

int getRawText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	LabelPtr widget = _widgets[id];
	lua_pushstring(L, widget->rawText().c_str());

	return 1;
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",           create},
			{"newMasked",     createMasked},
			{"setColor",      setColor},
			{"setBgColor",    setBgColor},
			{"setText",       setText},
			{"getText",       getText},
			{"setMask",       setMask},
			{"setEmptyMask",  setEmptyMask},
			{"setTextLength", setTextLength},
			{"getRawText",    getRawText},
			{"reset",         reset},
			{NULL,  NULL}
	};

	luaL_register(L, "label", methods);
}

} // label

} // wgt
