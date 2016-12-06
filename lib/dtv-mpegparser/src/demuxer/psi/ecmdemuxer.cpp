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
#include "ecmdemuxer.h"
#include "psi.h"
#include "../ts.h"
#include "../descriptors/ca.h"
#include <util/functions.h>
#include <util/buffer.h>
#include <util/log.h>

namespace tuner {

ECMDemuxer::ECMDemuxer( ID pid, ID caID )
	: PSIDemuxer( pid ), _caID(caID)
{
	_timeout = util::DWORD(-1);
	_std = true;
}

ECMDemuxer::~ECMDemuxer()
{
}

util::DWORD ECMDemuxer::timeout() const {
	return _timeout != util::DWORD(-1) ? _timeout : PSIDemuxer::timeout();
}

//  Signals
void ECMDemuxer::onParsed( const ParsedCallback &callback ) {
    _onParsed = callback;
}

void ECMDemuxer::onSection( BYTE *section, size_t len ) {
	DWORD crc;
	if (PSI_SYNTAX(section)) {
		crc = GET_CRC( section+len );
	}
	else {
		crc= util::crc_calc( util::DWORD(-1), section, len );
	}

	size_t num = 0;
	size_t count = 0;
	if (_caID >= 0x0600 && _caID <= 0x06FF && len >= 8) {	//	IRDETO
		num   = section[4];
		count = section[5];
	}

	if (count+1 != _lasts.size()) {
		LDEBUG( "cam", "Resize sections: count=%d, size=%d -> result=%d", count, _lasts.size(), count+1 );
		_lasts.clear();
		_lasts.resize(count+1);
	}

	if (num >= count+1) {
		LERROR( "cam", "Invalid num of section: num=%d, count=%d", num, count );
	}

	if (_lasts[num] != crc) {
		util::Buffer *raw = new util::Buffer( (const char *)section, len+4 );
		LDEBUG( "cam", "section changed: crc=%08x, num=%d, count=%d, len=%d",
			crc, num, count, len );
		notify<util::Buffer>( _onParsed, raw );
		_lasts[num] = crc;
	}
}

bool ECMDemuxer::useStandardVersion() const {
	return _std;
}

bool ECMDemuxer::checkSyntax() const {
	return false;
}

bool ECMDemuxer::checkTableID( ID tID ) const {
	return tID == 0x80 || tID == 0x81;
}

}

