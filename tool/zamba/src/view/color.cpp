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

#include "color.h"
#include "modules/wgt.h"
#include <luaz/lua.h>

namespace wgt{

Color Color::None      = Color(-1,-1,-1);

Color::Color() : _r(-1), _g(-1), _b(-1) {}

Color::Color(int r, int g, int b){
	_r = r;
	_g = g;
	_b = b;
}

Color::Color(const Color& c)
	: _r(c._r)
	, _g(c._g)
	, _b(c._b)
{}

Color::~Color(){
}

Color& Color::operator= (const Color& other)
{
	if (this != &other) {
		_r = other._r;
		_g = other._g;
		_b = other._b;
	}
	return *this;
}

int Color::r() const{
	return _r;
}

int Color::g() const{
	return _g;
}

int Color::b() const{
	return _b;
}

Color Color::toGrayscale() const
{
	int n = (int)(_r*0.3 + _g*0.59 + _b*0.11);
	return Color(n,n,n);
}

bool operator==(const Color& aColor, const Color& bColor)
{
	return aColor.r()==bColor.r()
	    && aColor.g()==bColor.g()
	    && aColor.b()==bColor.b();
}

bool operator!=(const Color& aColor, const Color& bColor)
{
	return !(aColor == bColor);
}

const colorMap_t parseColorTable(lua_State* L, int arg) {
	colorMap_t colorMap;

	lua_pushnil(L);
	while (lua_next(L, arg) != 0) {
		std::string key( luaL_checkstring(L, -2) );
		colorMap[key] = Wgt::view()->getColorById( luaL_checkint( L, -1 ) );
		lua_pop(L, 1);
	}

	return colorMap;
}

}

