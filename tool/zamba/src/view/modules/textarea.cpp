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
 * textarea.cpp
 *
 *  Created on: Jun 17, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"

#include "wgt.h"
#include "../widget/textarea.h"

#include <boost/make_shared.hpp>

namespace wgt {


namespace textarea {

std::map<WidgetId, TextAreaPtr> _widgets;

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
	const int lines         = luaL_checkint(L, narg++);
	const int margin        = luaL_checkint(L, narg++);

	TextAreaPtr ww;
	if ( args == 10) {
		ww = TextAreaPtr(new TextArea(x, y, w, h
				, colorMap
				, text, fontSize, lines, margin));
	} else if (args == 11) {
		const int alignment     = luaL_checkint(L, narg++);
		ww = TextAreaPtr(new TextArea(x, y, w, h
				, colorMap
				, text, fontSize, lines
				, margin, alignment));
	} else if (args == 12) {
		const int alignment     = luaL_checkint(L, narg++);
		const int borderWidth   = luaL_checkint(L, narg++);
		ww = TextAreaPtr(new TextArea(x, y, w, h
				, colorMap
				, text, fontSize, lines
				, margin, alignment
				, borderWidth));
	}
	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int setText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);

	TextAreaPtr widget = _widgets[id];
	widget->text(text);
	widget->needsRepaint(true);

	return 0;
}

int appendText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char* text = luaL_checkstring(L, narg++);

	TextAreaPtr widget = _widgets[id];
	widget->append(text);
	widget->needsRepaint(true);

	return 0;
}

int clearText(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	TextAreaPtr widget = _widgets[id];
	widget->clearText();
	widget->needsRepaint(true);

	return 0;
}

int fitToTextSize(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	//const bool b = luaL_checkint(L, narg++);

	TextAreaPtr widget = _widgets[id];
	widget->fitToText(true);
	widget->needsRepaint(true);

	return 0;
}

void init(lua_State *L){
	const struct luaL_Reg methods[] = {
			{"new",           create},
			{"setText",       setText},
			{"appendText",    appendText},
			{"clearText",     clearText},
			{"fitToTextSize", fitToTextSize},
			{NULL,  NULL}
	};

	luaL_register(L, "textarea", methods);
}

} // textarea

} // wgt
