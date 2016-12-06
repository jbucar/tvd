/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../types.h"
#include <mpegparser/demuxer/descriptors/demuxers.h>
#include <util/log.h>

namespace pvr {
namespace dvb {

static category::Type abnt_content( util::BYTE l1, util::BYTE l2 ) {
	category::Type tmp = category::unknown;

	switch (l1) {
		case 0x0:
			tmp = (l2 == 0x2) ? category::documentary : category::news;
			break;
		case 0x1:
			tmp = category::sports;
			break;
		case 0x2:
			tmp = category::educative;
			break;
		case 0x3:
			tmp = category::soap_opera;
			break;
		case 0x4:
		case 0x5:
			tmp = category::series;
			break;
		case 0x6: {
			switch (l2) {
				case 2:
					tmp = category::musical;
					break;
				default:
					tmp = category::art;
					break;
			};
			break;
		}
		case 0x7:
			tmp = category::reality;
			break;
		case 0x8: {
			switch (l2) {
				case 0:
					tmp = category::cooking;
					break;
				case 1:
					tmp = category::fashion;
					break;
				case 4:
					tmp = category::travel;
					break;
				default:
					tmp = category::information;
					break;
			};
			break;
		}
		case 0x9:
			tmp = category::comical;
			break;
		case 0xA:
			tmp = category::children;
			break;
		case 0xB:
			tmp = category::erotic;
			break;
		case 0xC:
			tmp = category::movie;
			break;
		case 0xE:
			tmp = category::discussion;
			break;
		default: {
			tmp = category::unknown;
			break;
		}
	};

	return tmp;
}

static category::Type etsi_content( util::BYTE l1, util::BYTE l2 ) {
	category::Type tmp = category::unknown;

	switch (l1) {
		case 0x1:
			switch (l2) {
				case 0x4:
					tmp = category::comical;
					break;
				case 0x5:
					tmp = category::soap_opera;
					break;
				case 0x8:
					tmp = category::erotic;
					break;
				default:
					tmp = category::movie;
					break;
			};
			break;
		case 0x2:
			switch (l2) {
				case 0x3:
					tmp = category::documentary;
					break;
				default:
					tmp = category::news;
					break;
			};
			break;
		case 0x3:
			tmp = category::information;
			break;
		case 0x4:
			tmp = category::sports;
			break;
		case 0x5:
			tmp = category::children;
			break;
		case 0x6:
			tmp = category::musical;
			break;
		case 0x7:
			tmp = category::art;
			break;
		case 0x8: {
			switch (l2) {
				case 0:
					tmp = category::discussion;
					break;
				case 1:
					tmp = category::documentary;
					break;
			};
			break;
		}
		case 0x9:
			tmp = (l2 == 0) ? category::educative : category::information;
			break;
		case 0xA: {
			switch (l2) {
				case 0x1:
					tmp = category::travel;
					break;
				case 0x5:
					tmp = category::cooking;
					break;
				default:
					tmp = category::information;
					break;
			};
			break;
		}
		default: {
			tmp = category::unknown;
			break;
		}
	};

	return tmp;
}

category::Type parseContent( const std::string &spec, const tuner::desc::Descriptors &descs ) {
	category::Type tmp = category::unknown;
	util::BYTE content;

	{	//	Get classification
		util::WORD val;
		tuner::desc::ContentDescriptor contentDescriptor;
		if (DESC_PARSE( descs, content, contentDescriptor ) && contentDescriptor.size() > 0) {
			val = contentDescriptor[0];
			content = util::BYTE((val & 0xFF00) >> 8);

			util::BYTE l1 = (content & 0xF0) >> 4;
			util::BYTE l2 = (content & 0x0F);

			if (spec == "ABNT") {
				tmp = abnt_content( l1, l2 );
			}
			else if (spec == "ETSI") {
				tmp = etsi_content( l1, l2 );
			}

			LTRACE( "content", "Parse content: spec=%s, l1=%02x, l2=%02x, type=%d", spec.c_str(), l1, l2, tmp );
		}
	}

	return tmp;
}

}
}

