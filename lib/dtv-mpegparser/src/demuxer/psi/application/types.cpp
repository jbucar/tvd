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
#include "types.h"
#include "../../text.h"
#include <util/string.h>
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace ait {
namespace desc {

namespace parser {

bool fnc_application( ApplicationDescriptor &app, BYTE *buf ) {
	size_t offset = DESC_HEADER;

	BYTE profilesLen = RB(buf,offset);
	size_t end = offset + profilesLen;
	while (offset < end) {
		ait::ApplicationProfile profile;

		profile.profile = RW(buf,offset);
		profile.major   = RB(buf,offset);
		profile.minor   = RB(buf,offset);
		profile.micro   = RB(buf,offset);

		app.profiles.push_back( profile );
	}

	BYTE data = RB(buf,offset);
	app.serviceBoundFlag = (data & 0x80) ? true : false;
	app.visibility       = BYTE((data & 0x60) >> 5);

	app.priority         = RB(buf,offset);

	while (offset < DESC_SIZE(buf)) {
		BYTE transportProtocolLabel = RB(buf,offset);
		app.transports.push_back( transportProtocolLabel );
	}

	ait::desc::show( app );

	return true;
}

bool fnc_transport_protocol( TransportDescriptor &transports, BYTE *buf ) {
	size_t offset = DESC_HEADER;
	ait::TransportProtocolDescriptor desc;

	desc.protocolID = RW(buf,offset);
	desc.label      = RB(buf,offset);
	switch (desc.protocolID) {
		case TP_OBJECT_CAROUSEL:
		case TP_DATA_CAROUSEL:
		{
			ait::DSMCCTransportProtocolDescriptor dsmcc;

			dsmcc.remote  = RB(buf,offset);
			dsmcc.remote &= 0x80;
			if (dsmcc.remote) {
				dsmcc.networkID = RW(buf,offset);
				dsmcc.tsID      = RW(buf,offset);
				dsmcc.serviceID = RW(buf,offset);
			}
			else {
				dsmcc.networkID = 0;
				dsmcc.tsID      = 0;
				dsmcc.serviceID = 0;
			}
			dsmcc.componentTag = RB(buf,offset);
			desc.info = dsmcc;
			break;
		}
		default: {
			LWARN( "AITDemuxer", "Transport protocol descriptor of protocolID=%04x not parsed", desc.protocolID );
		}
	}

	transports.push_back( desc );
	show( transports );
	return true;
}

bool fnc_name( ApplicationNameDescriptor &desc, BYTE *buf ) {
	size_t offset = DESC_HEADER;
	while (offset < DESC_SIZE(buf)) {
		ait::ApplicationName appName;
		offset += parseLanguage( appName.language, buf+offset );
		offset += parseText( appName.name, buf+offset );
		desc.push_back( appName );
	}
	show( desc );
	return true;
}

// size_t parseGingaApp( BYTE *buf, size_t len, BYTE tag, desc::Descriptors &descriptors ) {
// 	size_t offset = DESC_HEADER;
// 	ait::GingaApplicationDescriptor desc;
// 	std::string tmp;

// 	while (offset < len) {
// 		offset += parseText( tmp, buf+offset );
// 		desc.push_back( tmp );
// 	}

// 	show( desc );
// 	descriptors[tag] = desc;

// 	return len;
// }

bool fnc_ginga_ncl_app_location( GingaApplicationLocationDescriptor &desc, BYTE *buf ) {
	size_t offset = DESC_HEADER;
	offset += parseText( desc.base, buf+offset );
	offset += parseText( desc.classPath, buf+offset );
	int rest = DESC_SIZE(buf)-offset;
	parseText( desc.initialClass, buf+offset, rest );

	show( desc );
	return true;
}

bool fnc_external_app_auth( ExternalApplications &desc, BYTE *buf ) {
	size_t offset = DESC_HEADER;

	while (offset < DESC_SIZE(buf)) {
		ait::ExternalApplication app;

		app.orgID    = RDW(buf,offset);
		app.id       = RW(buf,offset);
		app.priority = RB(buf,offset);

		desc.push_back( app );
	}
	show( desc );
	return true;
}

bool fnc_icons( IconsDescriptor &desc, BYTE *buf ) {
	size_t off=DESC_HEADER;

	std::string locator;
	off += parseText( locator, buf+off );
	WORD flags = RW(buf,off);

	//	Make icon name
	WORD mask=0;
	for (BYTE i=0; i<16; i++) {
		mask = WORD(1 << i);
		if (mask & flags) {
			std::string name = util::format( "%s/dvb.icon.%04x", locator.c_str(), mask );
			desc.push_back( name );
		}
	}
	return true;
}

bool fnc_graphics_constraints( GraphicsConstraintsDescriptor &desc, BYTE *buf ) {
	size_t off=DESC_HEADER;

	BYTE data = RB(buf,off);

	desc.canRunWithoutVisibleUI = (data & 0x04) ? true : false;
	desc.handleConfigurationChanged = (data & 0x02) ? true : false;
	desc.handleControlledVideo = (data & 0x01) ? true : false;
	while (off < DESC_SIZE(buf)) {
		BYTE mode = RB(buf,off);
		desc.supportedModes.push_back( mode );
	}
	LDEBUG( "AITDemuxer", "Graphics constraints descriptor: modes=%d", desc.supportedModes.size() );
	return true;
}

}	//	namespace demuxer

void show( const ApplicationDescriptor &app ) {
	LDEBUG( "ait::Descriptor", "srvBoundFlag=%d, visibility=%x, priority=%d",
		app.serviceBoundFlag, app.visibility, app.priority );

	BOOST_FOREACH( const ApplicationProfile &profile, app.profiles ) {
		LDEBUG( "ait::Descriptor", "\t profile=%04x, version=(%02x,%02x,%02x)",
			profile.profile, profile.major, profile.minor, profile.micro );
	}

	BOOST_FOREACH( BYTE label, app.transports ) {
		LDEBUG( "ait::Descriptor", "\t Transport label: %02x", label );
	}
}

void show( const ExternalApplications &apps ) {
	BOOST_FOREACH( const ExternalApplication &app, apps ) {
		LDEBUG( "ait::ExternalApplication", "orgID=%08x, appID=%04x, priority=%02x", app.orgID, app.id, app.priority );
	}
}

void show( const TransportDescriptor &transports ) {
	LDEBUG( "ait::Transports", "size=%d", transports.size() );
	BOOST_FOREACH( const TransportProtocolDescriptor &transport, transports ) {
		LDEBUG( "ait::TPDescriptor", "protocolID=%04x, label=%x",
			transport.protocolID, transport.label );

		switch (transport.protocolID) {
			case TP_OBJECT_CAROUSEL:
			case TP_DATA_CAROUSEL:
			{
				const DSMCCTransportProtocolDescriptor dsmcc = transport.info.get<DSMCCTransportProtocolDescriptor>();
				if (dsmcc.remote) {
					LDEBUG( "ait::TPDesc", "\t DSMCC: remote=%02x, networkID=%04x, tsID=%04x, serviceID=%04x, tag=%02x",
						dsmcc.remote, dsmcc.networkID, dsmcc.tsID, dsmcc.serviceID, dsmcc.componentTag );
				}
				else {
					LDEBUG( "ait::TPDesc", "\t DSMCC: remote=%02x, tag=%02x",
						dsmcc.remote, dsmcc.componentTag );
				}
				break;
			}
			default:
				break;
		};
	}
}

void show( const ApplicationNameDescriptor &desc ) {
	BOOST_FOREACH( const ApplicationName &app, desc ) {
		LDEBUG( "ait::NameDescriptor", "Name=%s, language=%s",
			app.name.c_str(), app.language.code() );
	}
}

void show( const GingaApplicationDescriptor &desc ) {
	BOOST_FOREACH( const std::string &param, desc ) {
		LDEBUG( "ait::GingaApp", "param=%s", param.c_str() );
	}
}

void show( const GingaApplicationLocationDescriptor &desc ) {
	LDEBUG( "ait::GingaAppLocation", "base=%s, classPath=%s, initial=%s",
		desc.base.c_str(), desc.classPath.c_str(), desc.initialClass.c_str() );
}

void show( const Application &app ) {
	LDEBUG( "ait::Application", "Application: organization=%x, appID=%x, ctrlCode=%x, descriptors=%s",
		app.orgID, app.id, app.ctrlCode, app.descriptors.show().c_str() );
}

}	//	namespace desc
}
}
