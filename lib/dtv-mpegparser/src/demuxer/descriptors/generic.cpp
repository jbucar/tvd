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

#include "generic.h"
#include "demuxers.h"
#include <util/log.h>

namespace tuner {
namespace desc {

util::BYTE getStreamIdentifier( const Descriptors &desc ) {
	util::BYTE id;
	if (DESC_PARSE( desc, stream_identifier, id )) {
		return id;
	}
	return 0xFF;
}

bool getVideoDecode( const Descriptors &desc, int &width, int &height ) {
	VideoDecodeDescriptor vDesc;
	if (DESC_PARSE( desc, video_decode, vDesc )) {
		bool result=true;
		switch (vDesc.videoEncodeFormat) {
			case 0:	//	1080p
			case 1:	//	1080i
				width = 1920;
				height = 1080;
				break;
			case 2:	//	720p
				width = 1280;
				height = 720;
				break;
			case 3:	//	480p
			case 4:	//	480i
				width = 720;
				height = 576;
				break;
			default:
				result=false;
		};
		return result;
	}
	return false;
}

}	//	namespace desc
}

