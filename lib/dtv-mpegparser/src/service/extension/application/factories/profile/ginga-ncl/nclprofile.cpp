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
#include "nclprofile.h"
#include "nclapplication.h"
#include "generated/config.h"
#include "../../../../../service.h"
#include "../../../../../../demuxer/psi/psi.h"
#include "../../../../../../demuxer/descriptors/demuxers.h"
#include <util/log.h>
#include <util/cfg/configregistrator.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>

namespace tuner {
namespace app {

namespace fs = boost::filesystem;

NCLProfile::NCLProfile( Extension *ext )
	: Profile( ext )
{
}

NCLProfile::~NCLProfile( void )
{
}

const std::string NCLProfile::ginga() {
	const char *ptr = getenv( "GINGA_PROCESS" );
	if (ptr) {
		return std::string(ptr);
	}
	return util::cfg::getValue<std::string>("tuner.service.ginga-ncl.process");
}

void NCLProfile::serviceStarted( Service *srv ) {
	//	Find descriptor 0xFD (data component descriptor) in DSMCCC_B stream
	BOOST_FOREACH( const ElementaryInfo &elem, srv->elements() ) {
		if (elem.streamType == PSI_ST_TYPE_B) {
			//	Find data_component_descriptor
			desc::DataComponentDescriptor desc;
			if (DESC_PARSE( elem.descriptors, data_component, desc )) {
				processAdditionalGingaInfo( &desc.info );
			}
		}
	}
}

void NCLProfile::serviceStopped( Service * /*srv*/ ) {
	//	TODO: Remove modes detected in that service!!!
}

bool NCLProfile::isSupported( ID type ) const {
	return (type == ait::type::ginga || type == ait::type::ginga_ncl);
}

Application *NCLProfile::create( const ApplicationID &appID, const std::string &file ) const {
	Application *app=NULL;
	fs::path f(file);
	if (f.extension() == ".ncl") {
		app = new NCLApplication( extension(), appID, file );
	}
	return app;
}

Application *NCLProfile::create( const ApplicationID &id ) const {
	std::string script;

	//	Process NCL descriptors
	if (!processNCLDescriptors( script )) {
		return NULL;
	}

	//	Create application
	return new NCLApplication( extension(), id, script );
}

bool NCLProfile::isProfileSupported( const ait::ApplicationProfile &appProfile ) const {
	return appProfile.profile == 1 &&
		appProfile.major <= 1 &&
		appProfile.minor <= 0 &&
		appProfile.micro <= 0;
}

//	Process NCL descriptors
void NCLProfile::process( Application *app ) {
	//	Process default aux descriptors
	Profile::process( app );

	LDEBUG("NCLProfile", "Apply additional ginga info: modes=%d", _defaultModes.size());

	//	Add video modes from additional ginga info
	BOOST_FOREACH(const GraphicsResolution resolution, _defaultModes) {
		//	If is application
		if (app->appID() == resolution.first) {
			//	Modes from: Guia de Operacao Nov06v05
			video::mode::type mode;
			switch (resolution.second) {
				case 0x01:
					mode = video::mode::fullHD;
					break;
				case 0x02:
					mode = video::mode::hd;
					break;
				default:
					mode = video::mode::sd;
			};
			app->addVideoMode( mode );
		}
	}
}

bool NCLProfile::processNCLDescriptors( std::string &script ) const {
	//	Check Ginga NCL application location descriptor
	ait::GingaApplicationLocationDescriptor gingaLocDesc;
	if (!AIT_PROFILE_DESC_PARSE( ginga_ncl_app_location, gingaLocDesc )) {
		LWARN("NCLProfile", "(AitFactory) Ginga NCL Application Location Descriptor not present");
		return false;
	}

	fs::path p( gingaLocDesc.base );
	p.make_preferred();
	p /= gingaLocDesc.initialClass;
	script = p.string();

	return true;
}

bool NCLProfile::processAdditionalGingaInfo( util::Buffer *info ) {
	LDEBUG("NCLProfile", "Process additional ginga info: size=%d", info->length());

	//	TODO: Move parsing to demux
	if (info->length() >= 1) {
		util::BYTE *buf = info->bytes();
		size_t off = 0;

		//	All applications apply
		ApplicationID appID( 0, 0xFFFF );
		util::BYTE data = RB(buf,off);

		//	Application identifier flag == 1
		if (data & 0x20) {
			DWORD orgID = RDW(buf,off);
			WORD id = RW(buf,off);
			appID = ApplicationID( orgID, id );
		}

		//	Transmission format (00 | 01 | 10)

		//	Document resolution
		util::BYTE defaultResolution = util::BYTE((data & 0x1E) >> 1);
		_defaultModes.push_back( std::make_pair(appID,defaultResolution) );
	}
	return true;
}


}
}
