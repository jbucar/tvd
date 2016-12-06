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
#include "demuxer.h"
#include "filter.h"
#include "../ts.h"
#include "../psi/psi.h"
#include "../psi/psidemuxer.h"
#include <util/assert.h>
#include <util/buffer.h>
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace demuxer {
namespace ts {

/**
 * Constructor base.
 * @param count Indica la cantidad máxima de buffers a guardar en el pool.
 */
Demuxer::Demuxer()
{
	//	Create ts pool
	_local = new util::Buffer(TS_PACKET_SIZE);
}

/**
 * Destructor base.
 */
Demuxer::~Demuxer()
{
	delete _local;
}

void Demuxer::reset() {
	_local->resize( 0 );
}

/**
 * Inicia el filtro pasado por parámetro.
 * @param filter Instancia de @c Filter a ejecutar.
 * @return true si pudo comenzar la ejecución del filtro correctamente, false en caso contrario.
 */
bool Demuxer::startFilter( Filter *filter ) {
	bool result=false;
	DTV_ASSERT( filter );

	//	LDEBUG( "Demuxer", "start filter: filter=%p, pid=%04x", filter, filter->pid() );
	_mutex.lock();
	MapOfFilters::const_iterator it = _filters.find( filter->pid() );
	if (it == _filters.end()) {
		_filters[filter->pid()] = filter;
		result=true;
	}
	_mutex.unlock();
	return result;
}

/**
 * Elimina el filtro del listado a utilizar.
 * @param filter Instancia de @c Filter a frenar.
 */
void Demuxer::stopFilter( Filter *filter ) {
	//	LDEBUG( "Demuxer", "stop filter %p", filter );
	_mutex.lock();
	MapOfFilters::iterator it=_filters.find( filter->pid() );
	if (it != _filters.end()) {
		_filters.erase( it );
	}
	_mutex.unlock();
}

//	Parser
Filter *Demuxer::checkContinuity( ID pid, bool tsStart, util::BYTE actualCC ) {
	Filter *filter=NULL;

	MapOfFilters::iterator it=_filters.find( pid );
	if (it != _filters.end()) {
		filter = (*it).second;

		//	Check continuity counter
		if (!filter->checkContinuity( tsStart, actualCC )) {
			filter = NULL;
		}
	}

	return filter;
}

void Demuxer::parse( util::Buffer *buf ) {
	size_t offset = 0;
	size_t bufLen = buf->length();

	//LDEBUG( "Demuxer", "push data: buf=%p, size=%d", buf, buf->length() );

	//	Process local buffer ...
	while (_local->length() && offset < bufLen) {
		//	Copy necesary data to complete local buffer
		size_t need = TS_PACKET_SIZE - _local->length();
		offset		 = (need > bufLen) ? bufLen : need;
		_local->append( buf->data(), offset );
		if (_local->length() >= TS_PACKET_SIZE) {
			//	Parse local buffer
			size_t parsed = parse( _local, 0 );
			if (parsed == static_cast<size_t>(_local->length())) {
				_local->resize( 0 );
			}
			else if (parsed) {
				DTV_ASSERT(true);
			}
		}
	}

	//	Parse rest of buffer
	if (offset < bufLen) {
		size_t parsed = parse( buf, offset );
		size_t rest   = bufLen-parsed;
		if (rest) {
			_local->append( buf->bytes()+parsed, rest );
		}
		//LDEBUG( "Demuxer", "Parsed buffer: offset=%ld, parsed=%ld, rest=%ld", offset, parsed, rest );
	}
}

#define TSA_HAVE_PCR(ptr)  (TSA_FIELD(ptr) & 0x10)
#define TSA_HAVE_OPCR(ptr) (TSA_FIELD(ptr) & 0x20)
#define TSA_FIELDS_OFF     6
#define TSA_CLOCK_SIZE     6

// static inline QWORD getClock( BYTE *ptr ) {
// 	DWORD pcr1;
// 	WORD pcr2;
// 	QWORD pcr, pcr_ext;

// 	pcr1 = GET_DWORD(ptr);
// 	pcr2 = GET_WORD(ptr+4);

// 	pcr = ((QWORD) pcr1) << 1;
// 	pcr |= (pcr2 & 0x8000) >> 15;
// 	pcr_ext = (pcr2 & 0x01ff);
// 	return pcr * 300 + pcr_ext % 300;
// }

//	input parser: Parse a TS packet Documentation at iso13818-1.pdf
size_t Demuxer::parse( util::Buffer *buf, size_t parsed ) {
	BYTE *ptr	= buf->bytes();
	size_t size = buf->length();

	//LDEBUG( "Demuxer", "Begin parse: offset=%ld, bufLen=%ld, local=%d", parsed, size, (_local == buf) );

	while (parsed < size) {
		{	//	Find TS SYNC byte
			size_t begin=parsed;
			while (ptr[parsed] != TS_SYNC && parsed < size) {
				parsed++;
			}
			if (parsed - begin) {
				Buffer show( buf->bytes()+begin, parsed+10, false );
				LWARN( "Demuxer", "Sync lost offset=%ld, size=%ld, count=%ld, isLocal=%d, data=%s",
					begin, size, parsed-begin, (_local == buf), show.asHexa().c_str() );
			}
		}

		//	is the begin of TS packet!
		if (parsed < size) {
			int len	 = size - parsed;
			BYTE *ts = ptr + parsed;

			//	is ths TS complete?
			if (len >= TS_PACKET_SIZE) {
				ID pid = TS_PID(ts);

				//	Check for Transport Error Indicator (TES), payload exist, and null packets!!
				if (!TS_HAS_ERROR(ts) && pid != TS_PID_NULL) {
					bool startFlag = TS_START(ts) ? true : false;

					//	Find filter (and check continuity bit)
					_mutex.lock();
					Filter *filter = checkContinuity( pid, startFlag, TS_CONTINUITY(ts) );
					if (filter) {
						if (filter->processRawTS()) {
							filter->push( ts, TS_PACKET_SIZE );
						}
						else {
							int payloadOffset = TS_HEAD_SIZE;

							//	Adaptation field exists?
							if (TS_HAS_ADAPTATION(ts)) {
								//QWORD pcr=0;
								//QWORD opcr=0;
								//size_t off = TSA_FIELDS_OFF;

								if (TSA_HAVE_PCR(ts)) {
									//pcr = getClock( ts+off );
									//off += TSA_CLOCK_SIZE;
								}

								if (TSA_HAVE_OPCR(ts)) {
									//opcr = getClock( ts+off );
									//off += TSA_CLOCK_SIZE;
								}

								// LDEBUG( "Demuxer", "Adatation field: pid=%04x, len=%x, byte=%02X, pcr=%08llx, opcr=%08llx",
								// 	pid, TSA_LEN(ts), TSA_FIELD(ts), pcr, opcr );

								//	Only calculate payload offset if adaptation field exist
								payloadOffset += 1+TSA_LEN(ts);
							}

							//	Check payload offset
							if (TS_HAS_PAYLOAD(ts) && payloadOffset < TS_PACKET_SIZE) {
								BYTE *tsPayload	 = ts+payloadOffset;
								int tsPayloadLen = TS_PACKET_SIZE-payloadOffset;

								//	Packet is a section?
								bool needPush = true;
								if (startFlag && !TS_PAYLOAD_IS_PES(tsPayload)) {
									//	Get pointer field, skip them in payload and len
									BYTE pointerField = tsPayload[0];
									tsPayload++;
									tsPayloadLen--;

									//	Check pointer field
									if (!pointerField || pointerField < tsPayloadLen) {
										if (pointerField) {
											//	Append last block of a section
											filter->push( tsPayload, pointerField, false );

											//	Skip data marked via pointer field
											tsPayload	 += pointerField;
											tsPayloadLen -= pointerField;
										}
									}
									else {
										needPush = false;
										LWARN( "Demuxer", "Pointer field invalid pointer=%d, tsPayloadLen=%d", pointerField, tsPayloadLen );
									}
								}

								if (needPush) {
									//	Start a new section
									filter->push( tsPayload, tsPayloadLen, startFlag );
								}
							}
						}
					}
					_mutex.unlock();
				}

				parsed += TS_PACKET_SIZE;
			}
			else {
				//	break loop
				break;
			}
		}
	}

	//LDEBUG( "Demuxer", "End parse: parsed=%ld", parsed );
	return parsed;
}

}
}
}

