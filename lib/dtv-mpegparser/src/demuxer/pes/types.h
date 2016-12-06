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

#include "../types.h"

//	PES basic fields
#define PES_BASIC_HEADER  6
#define PES_STREAM_ID(ptr)   (ptr)[3]
#define PES_PACKET_LEN(ptr)  GET_WORD((ptr)+4)
#define PES_LEN(ptr)         (PES_PACKET_LEN(ptr)+PES_BASIC_HEADER)
#define PES_MAX_PAYLOAD      200*1024
#define PES_FIELDS_HEADER    3
#define PES_FIELDS_LEN(ptr)  util::BYTE(((ptr)[2]))

//	PES flags
#define PES_PTS_FLAG       0x80
#define PES_DTS_FLAG       0x40
#define PES_NO_PTS         0

//	StreamID assignments, ISO/IEC 13818-1 : 2000
#define PES_SID_INVALID    0x00
#define PES_SID_MAP        0xBC
#define PES_SID_PRIVATE_1  0xBD
#define PES_SID_PADDING    0xBE
#define PES_SID_PRIVATE_2  0xBF
#define PES_SID_ECM        0xF0
#define PES_SID_EMM        0xF1
#define PES_SID_DIRECTORY  0xFF
#define PES_SID_DSMCC      0xF2
#define PES_SID_ITU_TYPE_E 0xF8

namespace tuner {
namespace pes {
namespace dmx {

#define PES_DEMUXSTATE_LIST(fnc) \
	fnc(Sync) \
	fnc(Header) \
	fnc(Fields) \
	fnc(FieldsPayload) \
	fnc(SyncHeader) \
	fnc(SyncHeaderData) \
	fnc(Payload) \
	fnc(Skip)

#define DO_ENUM_DEMUXSTATE(n) n,
enum type {
	Unknown,
	PES_DEMUXSTATE_LIST(DO_ENUM_DEMUXSTATE)
	LAST_STATE
};
#undef DO_ENUM_DEMUXSTATE

}
}
}

