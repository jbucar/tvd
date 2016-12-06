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

#include "luawgt.h"
#include "wgt.h"
#include "../widget/progressbar.h"
#include <boost/make_shared.hpp>

namespace wgt {
namespace progressbar {

std::map<WidgetId, ProgressBarPtr> _widgets;

static int l_new( lua_State* L ) {

	// Mandatory arguments
	const int x = luaL_checkint(L, 1);
	const int y = luaL_checkint(L, 2);
	const int w = luaL_checkint(L, 3);
	const int h = luaL_checkint(L, 4);
	const int bar_margin = luaL_checkint(L, 5);
	unsigned short zIndex = (unsigned short) luaL_checkint(L, 6);
	const colorMap_t colorMap = parseColorTable(L, 7);

	// Optional arguments
	const char* label = NULL;
	if (lua_gettop(L) == 8) {
		label = luaL_checkstring(L, 8);
	}

	ProgressBarPtr bar = boost::make_shared<ProgressBar>(x, y, w, h, bar_margin, colorMap);
	bar->zIndex(zIndex);

	if (label) {
		bar->setLabel(label);
	}

	WidgetId id = Wgt::view()->addWidget(bar);
	_widgets[id] = bar;
	bar.reset();

	lua_pushnumber(L, id);
	return 1;
}

static int l_setValue( lua_State* L ) {
	// Mandatory arguments
	const WidgetId id = luaL_checkint(L, 1);
	const int value = luaL_checkint(L, 2);

	// Optional arguments
	const char* text = NULL;
	if (lua_gettop(L) == 3) {
		text = luaL_checkstring(L, 3);
	}

	ProgressBarPtr bar = _widgets[id];
	bar->show(value);

	if (text) {
		bar->setText(text);
	}

	return 0;
}

static int l_setMinValue( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 1);
	const int value = luaL_checkint(L, 2);

	ProgressBarPtr bar = _widgets[id];
	bar->setMinValue(value);

	return 0;
}

static int l_setMaxValue( lua_State* L ) {
	const WidgetId id = luaL_checkint(L, 1);
	const int value = luaL_checkint(L, 2);

	ProgressBarPtr bar = _widgets[id];
	bar->setMaxValue(value);

	return 0;
}

static int l_setFontSize( lua_State* L ) {
	int narg = 1;
	const WidgetId id = luaL_checkint(L, narg++);
	const int value = luaL_checkint(L, narg++);

	ProgressBarPtr bar = _widgets[id];
	bar->setFontSize(value);
	return 0;
}

const struct luaL_Reg methods[] = {
	{"new",             l_new          },
	{"setValue",        l_setValue     },
	{"setMaxValue",     l_setMaxValue  },
	{"setMinValue",     l_setMinValue  },
	{"setFontSize",     l_setFontSize  },
	{NULL,              NULL           }
};

void init( lua_State *L ) {
	luaL_register(L, "progressbar", methods);
}

}
}
