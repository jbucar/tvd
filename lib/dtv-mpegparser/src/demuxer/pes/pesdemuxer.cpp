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
#include "pesdemuxer.h"
#include "../text.h"
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE    1024
#define SET_STATE_OFF(st,off)	  \
	_dataOff = off; _state = st; \
	LTRACE( "PESDemuxer", "Set demuxer state: st=%s, off=%d", #st, off );
#define SET_STATE(st) SET_STATE_OFF( st, 0 )
#define SET_STATE_SYN(st) \
	_sync = 0xFFFFFFFF; \
	SET_STATE_OFF( st, 0 )

//	Implemented following:
//		ISO/IEC 13818-1: 2000 (E); 2.4.3.6: PES packet
//		ARIB STD-B24 V5.2 Volume 3, Chapter 5; 5.2 - Independient PES transmission protocol (6-STD-B24v5_2-3p3-E1.pdf)

namespace tuner {

namespace impl {

#define DO_CREATE_DEMUXSTATE_NAMES(n) #n,
static const char *stateNames[] = {
	"Unknown",
	PES_DEMUXSTATE_LIST(DO_CREATE_DEMUXSTATE_NAMES)
	NULL
};
#undef DO_CREATE_DEMUXSTATE_NAMES

static const char *getName( int st ) {
	if (st >= pes::dmx::LAST_STATE) {
		st=pes::dmx::Unknown;
	}
	return stateNames[st];
}

static QWORD getValue( BYTE *ptr ) {
	QWORD value;

	value = (ptr[0] & 0x0e);
	value <<= 7;

	value |= ptr[1];
	value <<= 7;
	value |= BYTE(ptr[2] >> 1);
	value <<= 8;

	value |= ptr[3];
	value <<= 7;
	value |= BYTE(ptr[4] >> 1);

	return value;
}

}

/**
 * Constructor base.
 * @param pesType Indica de que tipo es el elementary stream (video, audio, subtítulos, etc).
 */
PESDemuxer::PESDemuxer( pes::type::type pesType )
{
	_pesType = pesType;
	_data = (BYTE *)malloc( MAX_BUFFER_SIZE );
	SET_STATE_SYN(pes::dmx::Sync);
	_dataOff = 0;
}

/**
 * Destructor base.
 */
PESDemuxer::~PESDemuxer()
{
	free(_data);
}

/**
 * @return El tipo del elementary stream.
 */
pes::type::type PESDemuxer::type() const {
	return _pesType;
}

bool PESDemuxer::fillData( int minSize, BYTE *data, int &dataOff, BYTE *ptr, size_t len, size_t &ptrOff ) {
	DTV_ASSERT( minSize >= dataOff );
	DTV_ASSERT( ptrOff <= len );

	//	How many bytes we need to complete header?
	size_t copy = minSize-dataOff;
	if (copy > (len-ptrOff)) {
		copy = (len-ptrOff);
	}
	if (copy) {	//	Copy bytes
		if (data) {	//	Skip data only?
			memcpy( data+dataOff, (ptr+ptrOff), copy );
		}
		dataOff += copy;
		ptrOff += copy;
	}

	//	Data is complete?
	if (dataOff < minSize) {
		//	Wait for more data
		return false;
	}

	_payloadSize += minSize;
	return true;
}

bool PESDemuxer::fillData( int minSize, bool needFill, BYTE *ptr, size_t len, size_t &ptrOff ) {
	return fillData( minSize, needFill ? _data : NULL, _dataOff, ptr, len, ptrOff );
}

bool PESDemuxer::fillPayload( BYTE *ptr, size_t len, size_t &ptrOff ) {
	DTV_ASSERT( _packetLen < MAX_BUFFER_SIZE );
	return fillData( _packetLen-_payloadSize, true, ptr, len, ptrOff );
}

/**
 * Realiza el demultiplexado de datos.
 * @param ptr Datos a demultiplexar.
 * @param len Indica la longitud de @b ptr.
 */
void PESDemuxer::parse( BYTE *ptr, size_t len ) {
	size_t off=0;

	while (off < len) {
		LTRACE( "PESDemuxer", "Parse: state=%s, len=%ld, off=%ld, bytes=%ld",
			impl::getName(_state), len, off, len-off );

		if (_state == pes::dmx::Sync) {
			while (off < len && ((_sync & 0x00FFFFFF) != 0x00000001)) {
				_sync <<= 8;
				_sync |= *(ptr+off);
				off++;
			}

			if (off < len) {
				LTRACE( "PESDemuxer", "Find sync: sync=%08x, off=%ld", _sync, off );
				//	Reset header and change state
				SET_STATE_OFF( pes::dmx::Header, 3 );
			}
		}
		else if (_state == pes::dmx::Header) {
			//	Copy header
			if (!fillData( PES_BASIC_HEADER, true, ptr, len, off )) {
				return;
			}

			//	Setup
			_payloadSize = 0;
			_packetLen = PES_PACKET_LEN(_data);
			_pts = PES_NO_PTS;
			_dts = PES_NO_PTS;

			//	Check demux streamID
			_sID = PES_STREAM_ID(_data);
			LTRACE( "PESDemuxer", "Setup streamID: streamID=%02x", _sID );

			if (!checkSID( _sID )) {
				//	Skip all packet
				LWARN( "PESDemuxer", "invalid stream ID: packetStreamID=%02x", _sID );
				SET_STATE( pes::dmx::Skip );
			}
			else if (!checkLen( PES_LEN(_data) )) {
				//	Check packet len
				LWARN( "PESDemuxer", "invalid pes len" );
				SET_STATE( pes::dmx::Skip );
			}
			else if (_sID != PES_SID_MAP &&
				_sID != PES_SID_PADDING &&
				_sID != PES_SID_PRIVATE_2 &&
				_sID != PES_SID_ECM &&
				_sID != PES_SID_EMM &&
				_sID != PES_SID_DIRECTORY &&
				_sID != PES_SID_DSMCC &&
				_sID != PES_SID_ITU_TYPE_E)
			{
				SET_STATE( pes::dmx::Fields );
			}
			else if (_sID == PES_SID_PADDING) {
				SET_STATE( pes::dmx::Skip );
			}
			else if (_sID == PES_SID_PRIVATE_2) {
				SET_STATE( pes::dmx::SyncHeader );
			}
			else {
				SET_STATE( pes::dmx::Payload );
			}
		}
		else if (_state == pes::dmx::Fields) {
			//	Fill PES fields
			if (!fillData( PES_FIELDS_HEADER, _fields, _dataOff, ptr, len, off )) {
				return;
			}

			LTRACE( "PESDemuxer", "Fields header: first=%02x, second=%02x, len=%02x", _fields[0], _fields[1], PES_FIELDS_LEN(_fields) );
			SET_STATE( pes::dmx::FieldsPayload );
		}
		else if (_state == pes::dmx::FieldsPayload) {
			//	Fill fields payload
			if (!fillData( PES_FIELDS_LEN(_fields), true, ptr, len, off )) {
				return;
			}

			//	GET PTS
			if (_fields[1] & PES_PTS_FLAG) {
				_pts=impl::getValue(_data);
			}

			//	Get DTS
			if (_fields[1] & PES_DTS_FLAG) {
				_dts=impl::getValue(_data+5);
			}

			LTRACE( "PESDemuxer", "PTS=%llx, DTS=%llx", _pts, _dts );

			//	PES have private stream header?
			if (_sID == PES_SID_PRIVATE_1) {
				SET_STATE( pes::dmx::SyncHeader );
			}
			else {
				SET_STATE( pes::dmx::Payload );
			}
		}
		else if (_state == pes::dmx::SyncHeader) {
			//	Implemented following ARIB STD-B24 V5.2 (6-STD-B24v5_2-3p3-E1.pdf)
			//	Fill private stream header
			if (!fillData( 3, true, ptr, len, off )) {
				return;
			}

			//	Parse
			size_t syncOff=0;
			BYTE dataID=RB(_data,syncOff);
			BYTE privateID=RB(_data,syncOff);
			_syncHeaderLen = RB(_data,syncOff);
			_syncHeaderLen &= 0x0F;

			LTRACE( "PESDemuxer", "Sync header: dataID=%02x, privStreamID=%02x, headerLen=%d",
				dataID, privateID, _syncHeaderLen );

			if (!checkSynHeader( dataID, privateID ))  {
				SET_STATE( pes::dmx::Skip );
			}
			else if (_syncHeaderLen) {
				SET_STATE( pes::dmx::SyncHeaderData );
			}
			else {
				SET_STATE( pes::dmx::Payload );
			}
		}
		else if (_state == pes::dmx::SyncHeaderData) {
			//	Fill private stream header
			if (!fillData( _syncHeaderLen, false, ptr, len, off )) {
				return;
			}

			SET_STATE( pes::dmx::Payload );
		}
		else if (_state == pes::dmx::Payload) {
			int state=parsePayload( ptr, len, off );
			if (state != _state) {
				if (state == pes::dmx::Sync) {
					SET_STATE_SYN( state );
				} else {
					SET_STATE( state );
				}
			}
		}
		else if (_state == pes::dmx::Skip) {
			if (!_packetLen || fillData( _packetLen, NULL, _payloadSize, ptr, len, off )) {
				//	Skip all packet
				LTRACE( "PESDemuxer", "Payload complete: packetLen=%d, bytes=%ld", _packetLen, len-off );
				SET_STATE_SYN( pes::dmx::Sync );
			}
		}
		else {
			DTV_ASSERT(false);
			return;
		}
	}
}

void PESDemuxer::parse( BYTE *tsPayload, size_t len, bool start ) {
	LTRACE( "PESDemuxer", "Pase data: len=%d, start=%d", len, start );
	if (start) {
		SET_STATE_SYN(pes::dmx::Sync);
	}
	parse( tsPayload, len );
}

int PESDemuxer::parsePayload( BYTE * /*ptr*/, size_t /*len*/, size_t & /*off*/ ) {
	return pes::dmx::Skip;
}

//	Getters
bool PESDemuxer::checkSID( BYTE sID ) {
	if (_pesType == pes::type::audio) {
		return (sID & 0xF0) == 0xC0;
	}
	else if (_pesType == pes::type::video) {
		return (sID & 0xF0) == 0xE0;
	}
	else {
		return true;
	}
}

bool PESDemuxer::checkLen( size_t /*len*/ ) const {
	return true;
}

bool PESDemuxer::checkSynHeader( BYTE /*dataID*/, BYTE /*privateID*/ ) const {
	return true;
}

QWORD PESDemuxer::pts() const {
	return _pts;
}

QWORD PESDemuxer::dts() const {
	return _dts;
}

BYTE *PESDemuxer::data() const {
	return _data;
}

int PESDemuxer::dataOff() const {
	return _dataOff;
}

}
