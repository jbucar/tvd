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
#include "biop.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {
namespace biop {

void show( const Tap &tap ) {
	LDEBUG( "biop::Tap", "id=%04x, use=%04x, associationTag=%04x, sLen=%d",
		tap.id, tap.use, tap.assocTag, tap.selector.length() );
}

size_t parseTaps( BYTE *data, std::vector<Tap> &taps ) {
	size_t offset=0;

	//	Parse taps
	BYTE tCount = RB(data,offset);
	for (BYTE t=0; t<tCount; t++) {
		Tap tap;

		tap.id       = RW(data,offset);
		tap.use      = RW(data,offset);
		tap.assocTag = RW(data,offset);

		BYTE sLen = RB(data,offset);
		if (sLen) {
			tap.selector.copy( (data+offset), sLen );
		}
		offset += sLen;

		taps.push_back( tap );
		//show( tap );
	}
	return offset;
}

void parseModuleInfo( const util::Buffer &moduleInfoBytes, ModuleInfo &info ) {
	BYTE *data = moduleInfoBytes.bytes();
	size_t offset = 0;

	info.moduleTimeOut = RDW(data,offset);
	info.blockTimeOut  = RDW(data,offset);
	info.minBlockTime  = RDW(data,offset);

	//	Parse Taps
	offset += parseTaps( (data+offset), info.taps );

	//	User info
	BYTE usrInfoLen = RB(data,offset);
	info.userInfo.copy( (data+offset), usrInfoLen );
	//offset += usrInfoLen;

	show( info );
}

void show( const ModuleInfo &info ) {
	LDEBUG( "biop::ModuleInfo", "moduleTimeOut=%08x, blockTimeOut=%08x, minBlockTime=%08x, taps=%d, userInfo=%d",
		info.moduleTimeOut, info.blockTimeOut, info.minBlockTime, info.taps.size(), info.userInfo.length() );

	BOOST_FOREACH( const Tap &tap, info.taps ) {
		show( tap );
	}
}

}
}
}
