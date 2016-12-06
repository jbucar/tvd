/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "language.h"
#include <string.h>

namespace tuner {

namespace impl {

typedef struct {
	const char *code;
	const char *name;
} LangNames;

#define DO_ENUM_LANG_NAMES(c,t) { #c, #t },
static LangNames names[] = {
	LANG_NAME_LIST(DO_ENUM_LANG_NAMES)
	{ NULL, NULL }
};
#undef DO_ENUM_LANG_NAMES

}

Language::Language()
{
	memset(_code,0,4);
}

Language::Language( const Language &other )
{
	memcpy( _code, other._code, 4 );
}

Language::Language( const char *code )
{
	memset(_code,0,4);	
	if (code) {
		memcpy( _code, code, 3 );
	}
}

Language::~Language()
{
}

Language &Language::operator=( const Language &other ) {
	memcpy( _code, other._code, 4 );
	return *this;
}

bool Language::operator==( const Language &other ) const {
	return !memcmp( _code, other._code, 4 );
}

bool Language::empty() const {
	return _code[0] == '\0';
}

const char *Language::name() const {
	int i=0;
	while (impl::names[i].code) {
		if (!strncmp(_code,impl::names[i].code,3)) {
			return impl::names[i].name;
		}
		i++;
	}
	return impl::names[0].name;
}

}	//	namespace tuner

