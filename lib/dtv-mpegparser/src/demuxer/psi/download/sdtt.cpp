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
#include "sdtt.h"
#include <boost/foreach.hpp>
#include <util/log.h>

namespace tuner {

Sdtt::Sdtt( Version version, ID modelID, ID tsID, ID netID, ID serviceID, std::vector<ElementaryInfo> elements )
{
	_version   = version;
	_modelID   = modelID;
	_tsID      = tsID;
	_nitID     = netID;
	_serviceID = serviceID;
	_elements  = elements;
}

Sdtt::~Sdtt( void )
{
}

//  Getters
Version Sdtt::version() const {
	return _version;
}

ID Sdtt::modelID() const {
	return _modelID;
}

ID Sdtt::tsID() const {
	return _tsID;
}

ID Sdtt::nitID() const {
	return _nitID;
}

ID Sdtt::serviceID() const {
	return _serviceID;
}

const std::vector<Sdtt::ElementaryInfo> &Sdtt::elements() const {
	return _elements;
}

void Sdtt::show() const {
	LDEBUG( "Sdtt", "SDTT: version=%d, tsID=%x, nitID=%x, modelID=%04x, serviceID=%x", _version, _tsID, _nitID, _modelID, _serviceID );
	
	BOOST_FOREACH( Sdtt::ElementaryInfo info, _elements ) {
		LDEBUG( "Sdtt", "\tgroup=%x, targetVer=%x, newver=%x, downloadLevel=%x, versionIndicator=%x, timeShift=%x",
			info.group, info.targetVersion, info.newVersion, info.downloadLevel, info.versionIndicator, info.scheduleTimeShiftInfo );

		//	Show scheduleInfo
		LDEBUG( "Sdtt", "\tSchedule Info:" );
		BOOST_FOREACH( Sdtt::ScheduleElementaryInfo &sched, info.schedules ) {
			LDEBUG( "Sdtt", "\t\tstartTime=%llx, duration=%x", sched.startTime, sched.duration );
		}

		//	Show DownloadContentDescriptor
		LDEBUG( "Sdtt", "\tContents descriptors:" );
		BOOST_FOREACH( const DownloadContentDescriptor &desc, info.contents ) {
			LDEBUG( "Sdtt", "\t\treboot=%d, addOn=%d, componentSize=%d, downloadID=%x, timeoutDII=%d, leakRate=%08x, componentTag=%02x, lang=%s, text=%s",
				desc.reboot, desc.addOn, desc.componentSize, desc.downloadID,
				desc.timeoutDII, desc.leakRate, desc.componentTag, desc.lang.code(), desc.text.c_str() );

			if (desc.hasCompatibility) {
				//	Show Compatibility descriptor
				BOOST_FOREACH( const dsmcc::compatibility::Descriptor &comp, desc.compatibilities ) {
					LDEBUG( "Sdtt", "\t\t\tCompatibility: type=%02x, model=%04x, version=%04x, specifier=%08x",
						comp.type, comp.model, comp.version, comp.specifier );
					LDEBUG( "Sdtt", "\t\t\tIdentification field: type=%02x, data=%08x, marker=%02x, model=%02x, group=%02x, version=%04x",
						SPECIFIER_TYPE(comp.specifier), SPECIFIER_DATA(comp.specifier),
						MAKER_ID(comp.model), MODEL_ID(comp.model),
						GROUP_ID(comp.version), VERSION_ID(comp.version) );
				}
			}

			if (desc.hasModuleInfo) {
				//	Show Modules
				BOOST_FOREACH( const dsmcc::module::Type &module, desc.modules ) {
					LDEBUG( "Sdtt", "\t\t\tModules: id=%x, size=%d, version=%x, info=%d",
						module.id, module.size, module.version, module.info.length() );
				}
			}
		}
	} 
}

}
