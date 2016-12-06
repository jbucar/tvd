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
 * luawgt.h
 *
 *  Created on: May 13, 2011
 *      Author: gonzalo
 */

#ifndef LUAWGT_H_
#define LUAWGT_H_

#include <luaz/lua.h>

namespace wgt
{
	namespace label {
		void init(lua_State *luaState);
	}
	namespace list {
		void init(lua_State *luaState);
	}
	namespace changechannel {
		void init(lua_State *luaState);
	}
	namespace videotoggle {
		void init(lua_State *luaState);
	}
	namespace menupage {
		void init(lua_State *luaState);
	}
	namespace mainmenu {
		void init(lua_State *luaState);
	}
	namespace window {
		void init(lua_State *luaState);
	}
	namespace button {
		void init(lua_State *luaState);
	}
	namespace buttonset {
		void init(lua_State *luaState);
	}
	namespace buttongrid {
		void init(lua_State *luaState);
	}
	namespace bar {
		void init(lua_State *luaState);
	}
	namespace combobox {
		void init(lua_State *luaState);
	}
	namespace image {
		void init(lua_State *luaState);
	}
	namespace imageviewer {
		void init(lua_State *luaState);
	}
	namespace videoviewer {
		void init(lua_State *luaState);
	}
	namespace progressbar {
		void init(lua_State *luaState);
	}
	namespace slidebar {
		void init(lua_State *luaState);
	}
	namespace textarea {
		void init(lua_State *luaState);
	}
	namespace verticalset {
		void init(lua_State *luaState);
	}
	namespace tabbedwindow {
		void init(lua_State *luaState);
	}
	namespace schedule {
		void init(lua_State *luaState);
	}

	namespace yesnodialog {
		void init(lua_State *luaState);
	}
	namespace alertdialog {
		void init(lua_State *luaState);
	}
	namespace pindialog {
		void init(lua_State *luaState);
	}
	namespace basicdialog {
		void init(lua_State *luaState);
	}
	namespace scroll {
		void init(lua_State *luaState);
	}
}

#endif /* LUAWGT_H_ */
