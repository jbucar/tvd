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
#include "../luacallback.h"

#include "wgt.h"
#include "../widget/button.h"
#include "../widget/scrollarrow.h"
#include "../widget/buttonset.h"
#include "../widget/buttongrid.h"
#include "../widget/extendedbuttonset.h"

#include <util/assert.h>
#include <util/log.h>
#include <boost/make_shared.hpp>

namespace wgt {
namespace scroll {

std::map<WidgetId, ScrollPtr> _widgets;

int l_new( lua_State* L ) {
	const int x = luaL_checkint(L, 1);
	const int y = luaL_checkint(L, 2);
	const int w = luaL_checkint(L, 3);
	const int h = luaL_checkint(L, 4);
	const int sep = luaL_checkint(L, 5);
	const colorMap_t colorMap = parseColorTable(L, 6);
	const int setid = luaL_checkint(L, 7);

	ButtonSetPtr set = boost::static_pointer_cast<ButtonSet>(Wgt::view()->getWidgetById(setid));

	ScrollPtr ww = ScrollPtr(new Scroll(x, y, w, h, sep, colorMap, set));

	WidgetId id = wgt::Wgt::view()->addWidget(ww);
	_widgets[id] = ww;
	lua_pushnumber(L, id);
	return 1;
}

int l_selectChild( lua_State* L ) {
	const int id = luaL_checkint(L, 1);
	const int cid = luaL_checkint(L, 2);

	ScrollPtr scroll = _widgets[id];
	scroll->selectChild(cid);

	return 0;
}

int l_reset( lua_State* L ) {
	const int id = luaL_checkint(L, 1);
	const int limit = luaL_checkint(L, 2);

	ScrollPtr scroll = _widgets[id];
	scroll->init(limit);

	return 0;
}

const struct luaL_Reg methods[] = {
	{"new",             wgt::scroll::l_new            },
	{"reset",           wgt::scroll::l_reset          },
	{"selectChild",     wgt::scroll::l_selectChild    },
	{NULL,              NULL                          }
};

void init( lua_State *L ) {
	luaL_register(L, "scroll", methods);
}

}
}
