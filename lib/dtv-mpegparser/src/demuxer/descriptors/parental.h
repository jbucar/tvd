/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../descriptors.h"

#define ENUM_PARENTAL_CONTENT(func) \
	func(none,     0x00) \
	func(drugs,    0x10) \
	func(violence, 0x20) \
	func(sex,      0x40)


#define ENUM_PARENTAL_AGE(func) \
	func(none,    0x01) \
	func(year10,  0x02) \
	func(year12,  0x03) \
	func(year14,  0x04) \
	func(year16,  0x05) \
	func(year18,  0x06)

namespace tuner {
namespace desc {
namespace parental {

namespace content {

enum type {
#define DO_PARENTAL(n,v) n = v,
	ENUM_PARENTAL_CONTENT(DO_PARENTAL)
#undef DO_PARENTAL
	LAST_CONTENT
};

}	//	namespace content


namespace age {

enum type {
	reserved=0x00,
#define DO_AGE(n,v) n = v,
	ENUM_PARENTAL_AGE(DO_AGE)
#undef DO_AGE
	LAST_AGE
};

}	//	namespace age


//	Returns parental rating
void parse( const Descriptors &descs, const std::string &lang, age::type &age, util::BYTE &content );

}	//	namespace parental
}	//	namespace desc
}	//	namespace tuner

