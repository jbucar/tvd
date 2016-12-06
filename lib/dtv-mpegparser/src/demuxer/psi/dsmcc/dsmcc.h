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

#include "../psi.h"
#include "../../../types.h"
#include <util/buffer.h>
#include <util/any.h>
#include <string>
#include <vector>
#include <map>

//	DSM-CC (ISO/IEC International Standard 13818-6) section 6
#define COMPATIBILITY_DESC_PAD       0x00	//	Pad descriptor
#define COMPATIBILITY_DESC_HARDWARE  0x01	//	System hardware descriptor
#define COMPATIBILITY_DESC_SOFTWARE  0x00	//	System software descriptor

//	ABNT NBR 15606-3:2007
#define MODULE_DESC_TYPE         0x01
#define MODULE_DESC_NAME         0x02
#define MODULE_DESC_INFO         0x03
#define MODULE_DESC_LINK         0x04
#define MODULE_DESC_CRC32        0x05
#define MODULE_DESC_LOCATION     0x06
#define MODULE_DESC_EST_DOWNLOAD 0x07
#define MODULE_DESC_COMPRESSED   0x09
#define MODULE_DESC_COMPRESSION  0xC2


namespace tuner {
namespace dsmcc {

//	Event Name
struct EventNameStruct {
	util::WORD id;
	std::string name;
};
typedef struct EventNameStruct EventName;
void show( const EventName &ev );

//	DSM-CC (ISO/IEC International Standard 13818-6) section 6
namespace compatibility {

typedef struct {
	util::BYTE  type;
	util::DWORD specifier;
	util::WORD  model;
	util::WORD  version;
} Descriptor;
typedef std::vector<Descriptor> Descriptors;

size_t parse( util::BYTE *data, size_t len, Descriptors &descriptors );

}	//	namespace compatibility

namespace module {

class InfoDescriptor {
public:
	std::string	language;
	std::string text;

	bool operator==( const InfoDescriptor &other ) const {
		return language == other.language && text == other.text;
	}
};

class LinkDescriptor {
public:
	util::BYTE position;
	util::WORD moduleID;

	bool operator==( const LinkDescriptor &other ) const {
		return position == other.position && moduleID == other.moduleID;
	}
};

class CompressionTypeDescriptor {
public:
	util::BYTE type;
	util::DWORD originalSize;

	bool operator==( const CompressionTypeDescriptor &other ) const {
		return type == other.type && originalSize == other.originalSize;
	}
};

typedef struct {
	ID     id;
	util::DWORD  size;
	util::BYTE   version;
	util::Buffer info;
} Type;

typedef std::vector<Type> Modules;
typedef std::map<util::BYTE,util::Any> Descriptors;

size_t parse( util::BYTE *data, size_t len, Modules &modules, bool skipVersion=false );
void parseDescriptors( const util::Buffer &info, Descriptors &descriptors );

}	//	namespace module

namespace group {	//	Group Info Indication (EN 301 192 v1.4.1)

struct GroupInfoStruct {
	util::DWORD id;
	util::DWORD size;
	compatibility::Descriptors compatibility;
	util::Buffer info;
};
typedef struct GroupInfoStruct GroupInfo;

struct GroupInfoIndicationStruct {
	std::vector<GroupInfo> groups;
	util::Buffer privateData;
};
typedef struct GroupInfoIndicationStruct GroupInfoIndication;

size_t parse( util::BYTE *data, size_t len, GroupInfoIndication &gii );
void show( const GroupInfoIndication &gii );

}

}
}

