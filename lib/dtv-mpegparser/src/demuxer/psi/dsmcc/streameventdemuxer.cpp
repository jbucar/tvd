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
#include "streameventdemuxer.h"
#include "parser.h"
#include "../psi.h"
#include "../tableinfo.h"
#include <util/mcr.h>
#include <util/buffer.h>
#include <util/log.h>

namespace tuner {
namespace dsmcc {

namespace se {

struct TableID {
	ID eventID;
	bool operator==( const struct TableID &other ) const {
		return eventID == other.eventID;
	}
};

}

class StreamEventDemuxer::FindEventByID {
public:
	FindEventByID( WORD eventID ) : _eventID(eventID) {}
	virtual ~FindEventByID( void ) {}

	virtual bool operator()( const StreamEventFilter &filter ) const {
		return filter.first == _eventID;
	}
	WORD _eventID;
};

StreamEventDemuxer::StreamEventDemuxer( ID pid )
  : PSIDemuxer( pid )
{
}

StreamEventDemuxer::~StreamEventDemuxer()
{
	CLEAN_ALL(se::Table *,_tables);
}

//	Getters
ID StreamEventDemuxer::tableID() const {
	return DSMCC_TID_SD;
}

//  TSSectionDemuxer Virtual method
void StreamEventDemuxer::onSection( BYTE *section, size_t /*len*/ ) {
	//  Skip PSI header
	BYTE *dsmccSection = section+PSI_PAYLOAD_OFFSET;
	BYTE descTag = GET_SE_TAG(dsmccSection)
	switch (descTag) {
		case DSMCC_SE_NPT_REFERENCE:
		parseReferenceNPT( dsmccSection );
		break;
		case DSMCC_SE_STREAM_EVENT:
		case DSMCC_SE_GINGA_EDITCMD:
		parseSEStreamEvent( dsmccSection );
		break;
		default: {
			LWARN( "DSMCCDemuxer", "Stream event %x not handled", descTag );
		}
	};
}

TableInfo *StreamEventDemuxer::getTable( BYTE *section ) {
	se::TableID sub;

	//	Subtable is: programID + version
	sub.eventID = PSI_EXTENSION(section);
	return psi::getTable( _tables, sub );
}

bool StreamEventDemuxer::supportMultipleSections( void ) const {
	return false;
}

//  Parse Stream Events
void StreamEventDemuxer::registerStreamEvent( WORD eventID, const Callback &callback ) {
	_eventFilters.push_back( std::make_pair(eventID,callback) );
}

void StreamEventDemuxer::unregisterStreamEvent( WORD eventID ) {
	StreamEventFilters::iterator it=std::find_if( _eventFilters.begin(), _eventFilters.end(), FindEventByID( eventID ) );
	if (it != _eventFilters.end()) {
		_eventFilters.erase( it );
	}
}

void StreamEventDemuxer::parseReferenceNPT( BYTE *section ) {
	BYTE descLen          = GET_SE_LEN(section);
	bool postDiscontinuityIndicator = GET_BYTE(section+2) & 0x80 ? true : false;
	BYTE contentID        = GET_BYTE(section+2) & 0x7F;
	QWORD stcReference    = ((QWORD(GET_BYTE(section+3) & 0x1)) << 32) | (0x00000000FFFFFFFFULL & (GET_DWORD(section+4)));
	QWORD nptReference    = GET_QWORD(section+8) & 0x00000001FFFFFFFFULL;
	WORD scaleNumerator   = GET_WORD(section+16);
	WORD scaleDenominator = GET_WORD(section+18);

	LDEBUG( "DSMCCDemuxer", "Reference NPT: len=%d, post=%d, contentID=%d, stc=%llx, npt=%llx, nominator=%x, denominator=%x",
		descLen, postDiscontinuityIndicator, contentID, stcReference, nptReference, scaleNumerator, scaleDenominator );
}

void StreamEventDemuxer::parseSEStreamEvent( BYTE *section ) {
	BYTE descLen = GET_SE_LEN(section);
	WORD eventID = GET_WORD(section+2);

	StreamEventFilters::const_iterator it=std::find_if( _eventFilters.begin(), _eventFilters.end(), FindEventByID( eventID ) );
	if (it != _eventFilters.end()) {
		util::Buffer *buf = new util::Buffer( section, descLen+2 );
		(*it).second( buf );
	}
}

}
}

