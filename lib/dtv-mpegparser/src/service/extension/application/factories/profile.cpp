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

#include "profile.h"
#include "../application.h"
#include "../../../../demuxer/psi/application/types.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace app {

Profile::Profile( Extension *ext )
{
	_extension = ext;
}

Profile::~Profile( void )
{
}

void Profile::serviceStarted( Service * /*srv*/ ) {
}

void Profile::serviceStopped( Service * /*srv*/ ) {
}

Extension *Profile::extension() const {
	return _extension;
}

Application *Profile::create( const ApplicationID &appID, ID &serviceID, const ait::Application &app, const desc::Descriptors &aitDescs ) {
	Application *iApp=NULL;
	std::string name;
	Language language;
	bool serviceBound = false;
	BYTE visibility = 0x00;
	BYTE priority = 0x00;
	BYTE tag = 0x00;
	std::vector<BYTE> transports;

	_aitDescs = &aitDescs;
	_appDescs = &app.descriptors;

	//	Process AIT application descriptor
	if (processApplicationDescriptor( serviceBound, visibility, priority, transports )) {
		//	Process AIT application name descriptor
		if (processApplicationNameDescriptor( name, language )) {
			//	Process AIT transport descriptor
			if (processTransportProtocol( tag, transports )) {
				//	Try create application
				iApp = create( appID );
				if (iApp) {
					//	Setup AIT Application
					iApp->autoStart( app.ctrlCode == ait::control_code::autostart );
					iApp->autoDownload( app.ctrlCode == ait::control_code::prefetch );
					iApp->componentTag( tag );

					if (serviceBound) {
						iApp->service( serviceID );
					}
					else {
						serviceID = iApp->service();
					}

					app::visibility::type vis = visibility::none;
					if (visibility == ait::visibility::both) {
						vis = visibility::both;
					}
					else if (visibility == ait::visibility::api) {
						vis = visibility::api;
					}

					iApp->visibility( vis );
					iApp->priority( priority );
					iApp->name( name, language );
					iApp->readOnly( false );

					//	Process other descriptors
					process( iApp );
				}
			}
		}
	}

	_aitDescs = NULL;
	_appDescs = NULL;
	return iApp;
}

//	Process AIT descriptors
void Profile::process( Application *app ) {
	//	Process other descriptors
	processGraphicsConstraints( app );
	processIcons( app );
}

bool Profile::processApplicationDescriptor( bool &serviceBound, BYTE &visibility, BYTE &priority, std::vector<BYTE> &transports ) {
	//	Check application descriptor
	ait::ApplicationDescriptor appDesc;
	if (!AIT_PROFILE_DESC_PARSE( application, appDesc )) {
		LWARN("Profile", "Application Descriptor not present");
		return false;
	}

	//	Check profile: ETSI TS 102 812 v1.2.1
	//		The MHP terminal shall only launch applications if the following
	//		expression is true for one of the singalled profiles
	bool appSupported=false;
	BOOST_FOREACH( const ait::ApplicationProfile &appProfile, appDesc.profiles ) {
		//	If profile is supported
		if (isProfileSupported( appProfile )) {
			appSupported=true;
			break;
		}
	}
	if (!appSupported) {
		LWARN("Profile", "Application profile not supported");
		return false;
	}

	serviceBound = appDesc.serviceBoundFlag;
	visibility   = appDesc.visibility;
	priority     = appDesc.priority;
	transports   = appDesc.transports;

	return true;
}

bool Profile::processApplicationNameDescriptor( std::string &name, Language &language ) {
	//	Check application name descriptor
	ait::ApplicationNameDescriptor appNameDesc;
	if (!AIT_PROFILE_DESC_PARSE( name, appNameDesc )) {
		LWARN("Profile", "ignoring application becouse Application Name Descriptor not present");
		return false;
	}

	//	Check if application name is valid
	if (!appNameDesc.size()) {
		LWARN("Profile", "ignoring application becouse Application Name is invalid");
		return false;
	}

	//	Get first name
	const ait::ApplicationName &appName = appNameDesc[0];
	name     = appName.name;
	language = appName.language;
	return true;
}

struct TransportFinder {
	TransportFinder( BYTE label ) : _label(label) {}
	bool operator()( const ait::TransportProtocolDescriptor &desc ) const {
		return desc.label == _label;
	}
	BYTE _label;
};

bool Profile::processTransportProtocol( BYTE &tag, std::vector<BYTE> &transports ) {
	//	Check tranport protocol descriptor present
	ait::TransportDescriptor transportsDesc;
	if (!AIT_PROFILE_DESC_PARSE( transport_protocol, transportsDesc )) {
		LWARN("Profile", "ignoring application becouse Transport Protocol Descriptor not present");
		return false;
	}

	//	Try find a transport by label
	ait::TransportDescriptor::const_iterator itTrans=transportsDesc.end();
	BOOST_FOREACH( BYTE label, transports ) {
		itTrans=std::find_if( transportsDesc.begin(), transportsDesc.end(), TransportFinder(label) );
		if (itTrans != transportsDesc.end()) {
			break;
		}
	}
	if (itTrans == transportsDesc.end()) {
		LWARN("Profile", "ignoring application becouse transport label not found!");
		return false;
	}
	const ait::TransportProtocolDescriptor &transportDesc = (*itTrans);

	//	Support is only for object carousel transport, ignore others
	//	TODO: Add more transport protocols support
	if (transportDesc.protocolID != TP_OBJECT_CAROUSEL) {
		LWARN("Profile", "ignoring application becouse transport not supported");
		return false;
	}

	//	Check object carousel tranport
	const ait::DSMCCTransportProtocolDescriptor &dsmccDesc = transportDesc.info.get<ait::DSMCCTransportProtocolDescriptor>();
	if (dsmccDesc.remote) {
		LWARN("Profile", "ignoring application becouse object carousel transport is remote");
		return false;
	}
	tag = dsmccDesc.componentTag;

	return true;
}

void Profile::processIcons( Application *app ) {
	//	Check descriptor present
	ait::IconsDescriptor desc;
	if (AIT_PROFILE_DESC_PARSE( icons, desc )) {
		LINFO("Profile", "Process icons");
		app->addIcons( desc );
	}
}

void Profile::processGraphicsConstraints( Application *app ) {
	//	Check descriptor present
	ait::GraphicsConstraintsDescriptor desc;
	if (AIT_PROFILE_DESC_PARSE( graphics_constraints, desc )) {
		LINFO("Profile", "Process graphics constraints");
		BOOST_FOREACH( util::BYTE mode, desc.supportedModes ) {
			video::mode::type videoMode;

			//	Set mode from ETSI TS 102 809 v1.1.1 (2010-01)
			if (mode == ait::graphics_mode::mode_960_540 ||
				mode > ait::graphics_mode::mode_1920_1080)
			{
				videoMode = video::mode::sd;
			}
			else {
				videoMode = (video::mode::type)mode;
			}

			app->addVideoMode( videoMode );
		}
	}
}

}
}
