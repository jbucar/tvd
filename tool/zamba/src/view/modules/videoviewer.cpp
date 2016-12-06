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
#include "../widget/videoviewer.h"

#include <boost/make_shared.hpp>

namespace wgt {

namespace videoviewer {

namespace impl {
	VideoViewerPtr videoViewer;
}

int create( lua_State* L ) {
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const int zindex = luaL_checkint(L, narg++);

	const colorMap_t colorMap;
	impl::videoViewer = boost::make_shared<VideoViewer>( x, y, w, h, colorMap );
	impl::videoViewer->zIndex((short unsigned int)zindex);
	
	WidgetId id = Wgt::view()->addWidget(impl::videoViewer);
	lua_pushnumber(L, id);

	return 1;
}

void init( lua_State *L ){
	const struct luaL_Reg methods[] = {
			{"new",           create},
			{NULL,  NULL}
	};
	luaL_register(L, "videoviewer", methods);
}

} // videoviewer
} // wgt
