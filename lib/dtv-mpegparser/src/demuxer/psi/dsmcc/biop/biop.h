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

#include "../../../../types.h"
#include <util/buffer.h>
#include <vector>

#define BIOP_MAGIC            0x42494F50
#define BIOP_VER              0x0100
#define BIOP_MAX_HEADER       27 //  Size of BIOP object header included objectKind field

//  Profiles
#define TAG_PROFILE_LITE             0x49534f05
#define TAG_PROFILE_BIOP             0x49534f06
#define TAG_PROFILE_CONNBIND         0x49534f40
#define TAG_PROFILE_SERVICE_LOCATION 0x49534f46
#define TAG_PROFILE_OBJECT_LOCATION  0x49534f50

//	Type of Taps
#define BIOP_TAP_STR_EVENT_USE       0x000D	//	Stream Event Descriptor
#define BIOP_TAP_STR_STATUS_USE      0x000E	//	Stream Mode Descriptor
#define BIOP_TAP_DELIVERY_PARA_USE   0x0016	//	Module delivery parameters
#define BIOP_TAP_OBJECT_USE          0x0017	//	BIOP objects in Modules
#define BIOP_TAP_ES_USE              0x0018	//	Elementary stream
#define BIOP_TAP_PROGRAM_USE         0x0019	//	Program


//  Type of objects
#define BIOP_OBJECT_FILE 0x66696C00     //  File object
#define BIOP_OBJECT_DIR  0x64697200     //  Directory object
#define BIOP_OBJECT_GW   0x73726700     //  Service Gateway object
#define BIOP_OBJECT_STR  0x73747200     //  Stream object
#define BIOP_OBJECT_STE  0x73746500     //  Stream Event object

#define INVALID_KEY DWORD(-1)

namespace tuner {
namespace dsmcc {
namespace biop {

typedef DWORD ObjectKeyType;

struct TapStruct {
	WORD id;
	WORD use;
	WORD assocTag;
	Buffer selector;
};
typedef struct TapStruct Tap;
size_t parseTaps( BYTE *data, std::vector<Tap> &taps );
void show( const Tap &tap );
	
struct ModuleInfoStruct {
	DWORD moduleTimeOut;
	DWORD blockTimeOut;
	DWORD minBlockTime;
	std::vector<Tap> taps;
	Buffer userInfo;
};
typedef struct ModuleInfoStruct ModuleInfo;
void parseModuleInfo( const util::Buffer &moduleInfoBytes, ModuleInfo &info );
void show( const ModuleInfo &info );

}
}
}
