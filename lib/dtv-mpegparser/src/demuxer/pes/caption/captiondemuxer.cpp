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
#include "captiondemuxer.h"
#include "caption.h"
#include "../types.h"
#include <util/buffer.h>
#include <util/functions.h>
#include <util/log.h>

namespace tuner {
namespace arib {

//	Types
enum CaptionDemuxState {
	dmxDataGroup=pes::dmx::LAST_STATE,
	dmxManagement,
	dmxStatement,
	dmxDataUnit
};


CaptionDemuxer::CaptionDemuxer( bool isCC )
	: PESDemuxer( pes::type::subtitle ), _isCC(isCC)
{
	_groupID = 0;
}

CaptionDemuxer::~CaptionDemuxer()
{
}

//  Caption Management notifications
void CaptionDemuxer::onManagement( const ManagementCallback &callback ) {
	_onManagement = callback;
}

//  Caption Statement notifications
void CaptionDemuxer::onStatement( const StatementCallback &callback ) {
	_onStatement = callback;
}

void CaptionDemuxer::filterGroup( BYTE groupID ) {
	_groupID = groupID;
}

//	PESDemuxer virtual methods
bool CaptionDemuxer::checkSID( BYTE sID ) {
	return _isCC ? (sID == PES_SID_PRIVATE_1) : (sID == PES_SID_PRIVATE_2);
}

bool CaptionDemuxer::checkLen( size_t len ) const {
	//	ARIB TR-B14 6.2.2
	return len <= 640;
}

bool CaptionDemuxer::checkSynHeader( BYTE dataID, BYTE privateID ) const {
	//	Check PES header (ARIB TR-B14 6.2.2)
	if (privateID != 0xFF) {
		LWARN( "CaptionDemuxer", "Invalid Private_stream_id; must be 0xFF: privateID=%02x", privateID );
		return false;
	}

	if (_isCC && dataID != 0x80) {
		LWARN( "CaptionDemuxer", "Invalid Data_identifier; must be 0x80: isCC=%d, dataID=%02x", _isCC, dataID );
		return false;
	}

	if (!_isCC && dataID != 0x81) {
		LWARN( "CaptionDemuxer", "Invalid Data_identifier; must be 0x81: dataID=%02x", _isCC, dataID );
		return false;
	}

	return true;
}

int CaptionDemuxer::parsePayload( BYTE *ptr, size_t len, size_t &off ) {
	//	Caption data < 640, fill payload into demuxer data buffer
	if (!fillPayload( ptr, len, off )) {
		return pes::dmx::Payload;
	}

	//	Reset parsed data
	_parsed.groupID=0;
	_parsed.pts=0;
	_parsed.unit.resize(0);
	_parsed.pts=pts();

	parseDataGroup( data(), dataOff() );

	return pes::dmx::Sync;
}

void CaptionDemuxer::parseDataGroup( BYTE *packet, size_t len ) {
	//	Data Group (ARIB STD-B24 Volume 1, Table 9-1)
	size_t offset=0;
	BYTE group=RB(packet,offset);
	BYTE linkNumber=RB(packet,offset);
	BYTE lastLinkNumber=RB(packet,offset);
	WORD groupSize=RW(packet,offset);
	if (offset+groupSize > len) {
		LWARN( "CaptionDemuxer", "Group size invalid: offset=%d, group=%02x, linkNumber=%02x, lastLinkNumber=%02x, groupSize=%d, len=%d",
			offset, group, linkNumber, lastLinkNumber, groupSize, len );
		return;
	}

	//	Check data groups count by ES (ARIB TR-B14 6.2.2)
	if (linkNumber || lastLinkNumber) {
		LWARN( "CaptionDemuxer", "data group count invalid" );
		return;
	}

	//	Check CRC
	WORD crc=GET_WORD(packet+groupSize+5);
	WORD calcCrc=crc16_calc( 0, packet, groupSize+5 );
	if (crc != calcCrc) {
		LWARN( "CaptionDemuxer", "invalid CRC: crc=%04x, calc=%04x",
			crc, calcCrc );
		return;
	}

	_parsed.groupID=GROUP_ID(group);
	BYTE version=GROUP_VERSION(group);
	LTRACE( "CaptionDemuxer", "Group: group=%02x (%02x,%02x), link=%x, lastLink=%x, groupSize=%d, len=%d",
		group, _parsed.groupID, version, linkNumber, lastLinkNumber, groupSize, len );

	if (GROUP_IS_CAPTION_MANAGEMENT(_parsed.groupID)) {
		if (!_onManagement.empty()) {
			parseManagement( packet+offset, (len-offset) );
		}
	}
	else if (!_onStatement.empty() && _groupID == _parsed.groupID) {
		parseStatement( packet+offset, (len-offset) );
	}
}

void CaptionDemuxer::parseManagement( BYTE *packet, size_t len ) {
	size_t offset=0;

	{	//	Parse Caption Management data: Table 9-3
		BYTE tmd=RB(packet,offset);
		if ((tmd >> 6) == 0x2) {
			//	Skip OTM
			offset += 5;
		}
	}

	//	Check languages (TR-B14 6.2.1.4)
	BYTE nLang=RB(packet,offset);
	if (!nLang || nLang > 2) {
		LWARN( "CaptionDemuxer", "cannot be more than two language in a ES" );
		return;
	}

	std::vector<CaptionLanguage> langs;
	for (BYTE i=0; i<nLang; i++) {
		CaptionLanguage lang;
		BYTE tmp=RB(packet,offset);
		lang.tag=BYTE(tmp >> 5);
		BYTE dmf=tmp & 0x0F;
		if ((dmf & 0x0F) == 0x0F) {
			offset += 1;	//	Skip DC
		}
		parseLanguage( lang.name, packet+offset );
		offset += 3;

		lang.flags=RB(packet,offset);	//	Format+TCS+rollup_mode
		langs.push_back( lang );
	}

	//	Parse data unit
	if (parseDataUnit( packet+offset, (len-offset) )) {
		_onManagement( new Management(_parsed.groupID,_parsed.pts,_parsed.unit,langs) );
	}
}

void CaptionDemuxer::parseStatement( BYTE *packet, size_t len ) {
	size_t offset=0;

	//	Parse Caption Statement: Table 9-10
	BYTE b=RB(packet,offset);
	BYTE tmd = BYTE(b >> 6);
	if (tmd == 0x1 || tmd == 0x2) {
		//	Skip tmd
		offset += 5;
	}

	//	Parse data unit, if ok, call to dependent
	if (parseDataUnit( packet+offset, (len-offset) )) {
		_onStatement( new Statement(_groupID,_parsed.pts,_parsed.unit) );
	}
}

bool CaptionDemuxer::parseDataUnit( BYTE *packet, size_t len ) {
	if (len < 3) {
		LWARN( "CaptionDemuxer", "cannot parse data unit, invalid len" );
		return false;
	}

	//	Parse Data Unit: Table 9-11
	size_t offset=0;
	DWORD dataLoopEnd;
	DATA_UNIT_SIZE(dataLoopEnd,packet,offset);
	if (dataLoopEnd > len) {
		LWARN( "CaptionDemuxer", "cannot parse data unit, invalid data unit len" );
		return false;
	}

	while (offset < dataLoopEnd) {
		//	Check data unit separator code should be 0x1F
		BYTE sep=RB(packet,offset);
		if (sep != 0x1F) {
			LWARN( "CaptionDemuxer", "invalid separator" );
			return false;
		}

		//	Check parameter
		BYTE parameter=RB(packet,offset);
		if (parameter != DATA_UNIT_STATEMENT) {
			LWARN( "CaptionDemuxer", "invalid parameter" );
			return false;
		}

		DWORD dataUnitSize;
		DATA_UNIT_SIZE(dataUnitSize,packet,offset);
		_parsed.unit.append( (packet+offset), dataUnitSize );
		offset += dataUnitSize;
		// LDEBUG( "CaptionDemuxer", "DataUnit: sep=%02x, parameter=%02x, dataUnitSize=%d, data=%s",
		// 	sep, parameter, dataUnitSize, _parsed.unit.asHexa().c_str() );
	}

	return true;
}

}
}

