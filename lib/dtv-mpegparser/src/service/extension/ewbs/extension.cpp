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

#include "extension.h"
#include "../../service.h"
#include "../../../demuxer/descriptors/demuxers.h"
#include <util/log.h>

namespace tuner {
namespace ewbs {

class FindEmergencyService {
 public:
	FindEmergencyService( ID srvID ) : _srvID(srvID) {}

	bool operator()( const EmergencyInfoService &info ) const {
		return info.srvID == _srvID;
	}

	ID _srvID;
};

Extension::Extension( int areaCode/*=-1*/ )
	: _areaCode(areaCode), _inEmergency(0)
{
}

Extension::~Extension()
{
}

//	Start/Stop
void Extension::onServiceReady( Service *srv, bool ready ) {
	LDEBUG( "ewbs", "Service ready: state=%d, srvID=%04x", ready, srv->id() );
	if (ready) {
		if (_areaCode < 0) {
			//	Try find area code
			desc::TerrestrialDeliverySystemDescriptor tdsd;
			if (DESC_PARSE( srv->descriptors(service::desc::nit), terrestrial_delivery_system, tdsd )) {
				_areaCode = tdsd.area;
				LINFO( "ewbs", "Get automatic area code from 0xFA descriptor: area=%04x", _areaCode );
			}
		}

		//	Try find emergency descriptor
		desc::EmergencyInformationDescriptor eid;
		if (DESC_PARSE( srv->descriptors(service::desc::pmt), emergency_information, eid )) {
			BOOST_FOREACH( const desc::ServiceEmergencyInformation &srvEID, eid ) {
				//	Find if emergency apply on my area code
				bool apply = false;
				BOOST_FOREACH( util::WORD areaCode, srvEID.areaCodes ) {
					LDEBUG( "ewbs", "Check if apply emergency: areaCode=(stb=%04x,desc=%04x)", _areaCode, areaCode );
					if (areaCode == _areaCode) {
						apply=true;
						break;
					}
				}

				if (apply) {
					//	Change emergency state?
					bool chgState = (!_inEmergency && srvEID.startEndFlag);

					//	Update references
					_inEmergency += srvEID.startEndFlag ? 1 : -1;

					EmergencyInfo::iterator it=std::find_if( _info.begin(), _info.end(), FindEmergencyService(srvEID.serviceID) );
					if (it == _info.end()) {
						//	Get emergency info
						EmergencyInfoService eSrv;
						eSrv.srvID = srvEID.serviceID;
						eSrv.level = srvEID.signalLevel;

						LDEBUG("ewbs","Add emergency service: level=%d, serviceID=%02x", eSrv.level, eSrv.srvID );

						_info.push_back( eSrv );
					}

					if (chgState) {
						notify( _inEmergency );
					}
				}
				else {
					LDEBUG( "ewbs", "Emergency not applied: area=%04x", _areaCode );
				}
			}
		}
	}
	else if (_inEmergency > 0) {
		EmergencyInfo::iterator it=std::find_if( _info.begin(), _info.end(), FindEmergencyService(srv->id()) );
		if (it != _info.end()) {
			_info.erase(it);
		}

		_inEmergency--;
		if (!_inEmergency) {
			notify( false );
		}
	}
}

const EmergencyInfo &Extension::getAll() const {
	return _info;
}

bool Extension::inEmergency() const {
	return _inEmergency > 0;
}

bool Extension::inEmergency( ID srvID ) const {
	if (inEmergency()) {
		//	Find service in emergency list
		tuner::ewbs::EmergencyInfo::const_iterator it=std::find_if(
			_info.begin(),
			_info.end(),
			FindEmergencyService(srvID)
		);
		return (it != _info.end());
	}
	return false;
}

void Extension::onEmergency( const EmergencyCallback &callback ) {
	_onEmergency = callback;
}

void Extension::notify( bool state ) {
	LINFO( "ewbs", "Emergency active: state=%d", state );
	if (!_onEmergency.empty()) {
		_onEmergency( state );
	}
}

}
}

