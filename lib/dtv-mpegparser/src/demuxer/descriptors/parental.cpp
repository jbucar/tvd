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

#include "parental.h"
#include "demuxers.h"
#include <boost/foreach.hpp>

namespace tuner {
namespace desc {
namespace parental {

//	Returns parental rating
void parse( const Descriptors &descs, const std::string &lang, age::type &age, util::BYTE &content ) {
	util::BYTE rating = util::BYTE((content::none << 4) & age::none);

	{	//	Parse parental raging descriptor
		ParentalRatingDescriptor ratingDesc;
		if (DESC_PARSE( descs, parental_rating, ratingDesc )) {
			BOOST_FOREACH( const struct ParentalRating &desc, ratingDesc ) {
				if (lang == desc.language.code()) {
					rating = (desc.rating & 0xFF);
				}
			}
		}
	}

	//	Get parentalAge
	age = age::type(rating & 0x0F);
	if (!age) {	//	Is reserved ... set none
		age = age::none;
	}

	//	Get parentalContent
	content = (rating & 0xF0);
}

bool isProtected( util::BYTE curContent, util::BYTE protectedContent, age::type curAge, age::type protectedAge ) {
	return (protectedAge != 1 && curAge >= protectedAge) || ((protectedContent & curContent) != 0);
}

}	//	namespace parental
}	//	namespace desc
}	//	namespace tuner

