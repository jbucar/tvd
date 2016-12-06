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
#include "rawdemuxer.h"
#include "psi.h"
#include "../ts.h"
#include <util/functions.h>
#include <util/buffer.h>

namespace tuner {

RawDemuxer::RawDemuxer( ID pid )
	: PSIDemuxer( pid )
{
	_lastCRC = 0;
	_timeout = util::DWORD(-1);
	_std = true;

	_filterTID = false;
	_tID = 0;
	_tMask = 0;
}

RawDemuxer::RawDemuxer( ID pid, util::DWORD timeout, bool std )
	: PSIDemuxer( pid )
{
	_timeout = timeout;
	_lastCRC = 0;
	_std = std;
	_filterTID = false;
	_tID = 0;
	_tMask = 0;	
}

RawDemuxer::~RawDemuxer()
{
}

util::DWORD RawDemuxer::timeout() const {
	return _timeout != util::DWORD(-1) ? _timeout : PSIDemuxer::timeout();
}

bool RawDemuxer::checkTableID( ID tID ) const {
	return _filterTID ? (_tID == tID) : true;
}

bool RawDemuxer::filterTableID( util::BYTE &tID, util::BYTE &mask ) const {
	if (_filterTID) {
		tID = _tID;
		mask = _tMask;
		return true;
	}
	return false;
}

void RawDemuxer::setFilter( util::BYTE tID, util::BYTE tMask ) {
	_filterTID = true;
	_tID = tID;
	_tMask = tMask;
}

//  Signals
void RawDemuxer::onParsed( const ParsedCallback &callback ) {
    _onParsed = callback;
}

void RawDemuxer::onSection( BYTE *section, size_t len ) {
	DWORD crc;
	if (PSI_SYNTAX(section)) {
		crc = GET_CRC( section+len );
	}
	else {
		crc= util::crc_calc( util::DWORD(-1), section, len );
	}

	if (crc != _lastCRC) {
		util::Buffer *raw = new util::Buffer( (const char *)section, len+4 );
		notify<util::Buffer>( _onParsed, raw );
		_lastCRC = crc;
	}
}

bool RawDemuxer::useStandardVersion() const {
	return _std;
}

bool RawDemuxer::checkSyntax() const {
	return false;
}

}

