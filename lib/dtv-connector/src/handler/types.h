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
#pragma once

#include <util/types.h>

#define MESSAGE_HEADER  3
#define MESSAGE_TYPE    0
#define MESSAGE_SIZE    1

#define MSG_TYPE(b)         (util::BYTE)((b)[MESSAGE_TYPE])
#define MSG_SIZE(b)         (util::WORD)GET_WORD((b)+MESSAGE_SIZE)
#define MSG_PAYLOAD_SIZE(b) (MSG_SIZE(b)-MESSAGE_HEADER)
#define MSG_PAYLOAD(b)      (util::BYTE *)((b)+MESSAGE_HEADER)
#define MAKE_MSG(b,t,s)     (b)[MESSAGE_TYPE] = t; (b)[MESSAGE_SIZE] = util::BYTE((s) >> 8); (b)[MESSAGE_SIZE+1] = util::BYTE((s) & 0x00FF)

namespace connector {

namespace messages {
enum  type {
	keyRegister       = 0x01,
	keepAlive         = 0x02,
	editingCommand    = 0x03,
	videoResize       = 0x04,
	exit              = 0x05,
	keyEvent          = 0x06,
	startPresentation = 0x07,
	canvasCommand     = 0x08,
	playVideo         = 0x09,
	stopVideo         = 0x10,
	buttonEvent       = 0x11
};

}
}
