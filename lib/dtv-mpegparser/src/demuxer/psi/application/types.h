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

#include "../../../types.h"
#include "../../descriptors.h"
#include "../../descriptors/generic.h"
#include <vector>

#define AIT_PROFILE_DESC_PARSE( tag, val ) findDesc( tuner::ait::desc::ident::tag, &tuner::ait::desc::parser::fnc_ ## tag, val )
#define AIT_DESC_PARSE( var, tag, val ) var.parse( tuner::ait::desc::ident::tag, &tuner::ait::desc::parser::fnc_ ## tag, val )

namespace tuner {
namespace ait {

namespace type {

//	Application Type
enum type {
	reserved =0x0000,
	dvb_java =0x0001,
	dvb_html =0x0002,
	acap_java=0x0006,
	arib_bml =0x0007,
	ginga    =0x0008,
	ginga_ncl=0x0009
};

}	//	namespace application type

namespace control_code {

enum type {
	reserved =0x00,
	autostart=0x01,
	present  =0x02,
	destroy  =0x03,
	kill     =0x04,
	prefetch =0x05,
	remote   =0x06,
	unbound  =0x07
};

}	//	namespace control_code

namespace visibility {

enum type {
	//	This application is visible neither to other applications
	//	via application enumeration API nor to users via navigator
	//	except for error information of log-out and the like
	none     =0x00,
	//	This application is not visible to users but visible from other
	//	applications via application enumeration API
	api      =0x01,
	//	Reserved for future
	reserved =0x02,
	//	This application is visible to both users and other applications
	both     =0x03
};

}	//	namespace visibility

namespace graphics_mode {

enum type {
	reserved       = 0,
	fullScreen     = 1,
	mode_960_540   = 2,
	mode_280_720   = 3,
	mode_1920_1080 = 4
};
	
}	//	namespace graphics_mode

namespace icon {

enum type {
	icon_32_32_1_1   = 0x0001,	//	32x32 for square pixel display
	icon_32_32_4_3   = 0x0002,	//	32x32 for broadcast pixels on 4:3 display
	icon_24_32_16_9  = 0x0004,	//	24x32 for broadcast pixels on 16:9 display
	icon_64_64_1_1   = 0x0008,	//	64x64 for square pixel display
	icon_64_64_4_3   = 0x0010,	//	64x64 for broadcast pixels on 4:3 display
	icon_48_64_16_9  = 0x0020,	//	48x64 for broadcast pixels on 16:9 display
	icon_128_128_1_1 = 0x0040,	//	128x128 for square pixel display
	icon_128_128_4_3 = 0x0080,	//	128x128 for broadcast pixels on 4:3 display
	icon_96_128_16_9 = 0x0100	//	96x128 for broadcast pixels on 16:9 display
};

}	//	namespace icon

//	Icons descriptor: ETSI TS 102 812 v1.2.1 (2003-06). Section 10.7.4.2
typedef std::vector<std::string> IconsDescriptor;

//	Graphics constraints descriptor: ETSI TS 102 809 v1.1.1 (2010-01). Section 5.2.9
struct GraphicsConstraintsStruct {
	bool canRunWithoutVisibleUI;
	bool handleConfigurationChanged;
	bool handleControlledVideo;
	std::vector<util::BYTE> supportedModes;
};
typedef struct GraphicsConstraintsStruct GraphicsConstraintsDescriptor;

//	Application Descriptor
struct ApplicationProfileStruct {
	util::WORD profile;
	util::BYTE major;
	util::BYTE minor;
	util::BYTE micro;	
};
typedef struct ApplicationProfileStruct ApplicationProfile;

struct ApplicationStruct {
	std::vector<ApplicationProfile> profiles;
	bool serviceBoundFlag;
	util::BYTE visibility;
	util::BYTE priority;
	std::vector<util::BYTE> transports;
};
typedef ApplicationStruct ApplicationDescriptor;

//	External Application Authorisation
struct ExternalApplicationStruct {
	util::DWORD orgID;
	util::WORD id;
	util::BYTE priority;
};
typedef ExternalApplicationStruct        ExternalApplication;
typedef std::vector<ExternalApplication> ExternalApplications;

//	Transport Protocol Descriptor
#define TP_OBJECT_CAROUSEL 0x0001
#define TP_DATA_CAROUSEL   0x0004
struct DSMCCTransportProtocolStruct {
	util::BYTE remote;
	util::WORD networkID;
	util::WORD tsID;
	util::WORD serviceID;
	util::BYTE componentTag;
};
typedef struct DSMCCTransportProtocolStruct DSMCCTransportProtocolDescriptor;

struct TransportProtocolStruct {
	util::WORD protocolID;
	util::BYTE label;
	Any info;
};
typedef struct TransportProtocolStruct TransportProtocolDescriptor;
typedef std::vector<TransportProtocolDescriptor> TransportDescriptor;

//	Application Name Descriptor
struct ApplicationNameStruct {
	Language language;
	std::string name;
};
typedef struct ApplicationNameStruct ApplicationName;
typedef std::vector<ApplicationName> ApplicationNameDescriptor;

//	Ginga Application Descriptor (Java/NCL)
typedef std::vector<std::string> GingaApplicationDescriptor;

//	Ginga Application Location Descriptor (Java/NCL)
struct GingaApplicationLocationStruct {
	std::string base;
	std::string classPath;
	std::string initialClass;
};
typedef struct GingaApplicationLocationStruct GingaApplicationLocationDescriptor;

//	Application
typedef struct {
	util::DWORD orgID;
	util::WORD id;
	util::BYTE ctrlCode;
	tuner::desc::Descriptors descriptors;
} Application;

namespace desc {

namespace ident {

enum type {
	application = 0x00,
	name = 0x01,
	transport_protocol = 0x02,
	ginga_j_app = 0x03,
	ginga_j_app_location = 0x04,
	external_app_auth = 0x05,
	ginga_ncl_app_desc = 0x06,
	ginga_ncl_app_location = 0x07,
	icons = 0x0B,
	prefetch = 0x0C,
	dii_location = 0x0D,
	ip_signalling = 0x11,
	graphics_constraints = 0x14
};

}	//	namespace ident

void show( const GraphicsConstraintsDescriptor &desc );
void show( const ApplicationDescriptor &app );
void show( const ApplicationNameDescriptor &desc );
void show( const Application &app );
void show( const ExternalApplications &apps );
void show( const TransportDescriptor &transports );
void show( const GingaApplicationDescriptor &desc );
void show( const GingaApplicationLocationDescriptor &desc );

namespace parser {

bool fnc_application( ApplicationDescriptor &desc, util::BYTE *descPayload );
bool fnc_name( ApplicationNameDescriptor &desc, util::BYTE *descPayload );
bool fnc_external_app_auth( ExternalApplications &desc, util::BYTE *descPaylaod );
bool fnc_transport_protocol( TransportDescriptor &desc, util::BYTE *descPaylaod );
bool fnc_icons( IconsDescriptor &desc, util::BYTE *descPaylaod );
bool fnc_graphics_constraints( GraphicsConstraintsDescriptor &desc, util::BYTE *descPaylaod );
bool fnc_ginga_ncl_app_location( GingaApplicationLocationDescriptor &desc, util::BYTE *descPaylaod );

}

}	//	namespace desc
}
}

