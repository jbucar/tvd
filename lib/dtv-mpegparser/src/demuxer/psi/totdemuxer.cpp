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
#include "totdemuxer.h"
#include "psi.h"
#include "../descriptors.h"
#include "../descriptors/demuxers.h"
#include "../../time.h"
#include <util/log.h>

//	Documented in:
//		- ABNT 15603-2
//		- EN 300 468:2007

namespace tuner {

TOTDemuxer::TOTDemuxer( WORD pid, const std::string &country )
	: PSIDemuxer( pid ), _country(country.c_str()), _send(false)
{
}

TOTDemuxer::~TOTDemuxer()
{
}

//	Getters
bool TOTDemuxer::syntax() const {
	return false;
}

ID TOTDemuxer::tableID() const {
	return PSI_TID_TOT;
}

//  Signal
void TOTDemuxer::onParsed( const ParsedCallback &callback ) {
	_onParsed = callback;
}

DWORD TOTDemuxer::frequency() const {
	return 30*1000;
}

void TOTDemuxer::onSection( BYTE *section, size_t len ) {
	//	Check CRC
	if (checkCRC( section, len )) {
		desc::Descriptors descs;
		size_t offset=3;
	
		WORD mjd = RW(section,offset);
		DWORD lsb = RW(section,offset)
		lsb <<= 8;
		lsb  |= RB(section,offset);

		descs.append( section+offset, len-offset );

		bool needApply=true;
		{	//	Check country code from local time offset descriptor, if exist!
			desc::LocalTimeOffsetDescriptor desc;
			if (DESC_PARSE( descs, local_time_offset, desc )) {
				needApply = false;
				BOOST_FOREACH( const desc::LocalTimeOffset &timeOff, desc) {
					needApply |= (timeOff.countryCode == _country);
				}
			}
		}

		if (needApply) {
			MJDate date(mjd,lsb);
			pt::ptime now = date.get();

			//	Is valid?
			if (!now.is_special()) {
				time::clock.update( now );
			
				if (!_send) {
					Tot *tot = new Tot( date, descs );
					notify<Tot>( _onParsed, tot );
					_send = true;
				}
			}
		}
		else {
			LWARN( "TOTDemuxer", "Invalid country code" );			
		}
	}
	else {
		LWARN( "TOTDemuxer", "CRC error" );
	}
}

}
