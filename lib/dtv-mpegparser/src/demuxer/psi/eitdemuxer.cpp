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
#include "eitdemuxer.h"
#include "psi.h"
#include "tableinfo.h"
#include "../../time.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

//	Implementation based in:
//		ETSI EN 300 468 v1.11.1 (2010-4)
//		ETSI TR 101 211 v1.6.1 (2004-05)
//		ABNT NBR 15603-2:2007
//		ARIB TR-B14 v3.8 Fascicle 2, Volume 4, Chapter 13: EIT Transmission (8-TR-B14v3_8-2p3-E1.pdf)

//#define DEBUG_EIT

namespace tuner {

namespace eit {

struct TableID {
	ID tableID;
	ID lastTableID;
	ID serviceID;
	ID tsID;
	ID nitID;

	bool operator ==( const struct TableID &other ) const {
		return tableID == other.tableID &&
		serviceID == other.serviceID &&
		tsID == other.tsID &&
		nitID == other.nitID;
	}
};

class Table : public TableImpl<struct TableID> {
public:
	Table( const struct TableID &id ) : TableImpl<struct TableID>(id) {
		LTRACE( "eit", "Table ctor: tableID=%04x, serviceID=%04x, tsID=%04x, nitID=%04x",
			id.tableID, id.serviceID, id.tsID, id.nitID );
	}
	virtual ~Table() {
		CLEAN_ALL( eit::Event *, _events );
	}

	void addEvent( bool isSched, WORD sec, eit::Event *ev ) {
		DTV_ASSERT(ev);
		if (!isSched) {	//	Present!
			if (_events.size() < (size_t)sec+1) {
				_events.resize(sec+1);
			}
			_events[sec] = ev;
		}
		else {
			_events.push_back( ev );
		}
	}

	eit::Events &events() {
		return _events;
	}

	virtual void update( Version ver ) {
		TableImpl<struct TableID>::update( ver );
		CLEAN_ALL( eit::Event *, _events );
	}

	virtual bool checkSection( BYTE *section ) {
		//	ARIB TR-B14, section 13.16: Transmission of the H-EIT(schedule) that became past information as time
		//	passed should be stopped on a segment basis.
		if (sections() > 0 && isFirstDay()) {
			//	Segments affected: 0 to 8
#ifdef DEBUG_EIT
			LTRACE( "eit", "Check Section: tableID=%04x, serviceID=%04x, firstValidSegment=%d",
				id().tableID, id().serviceID, getFirstValidSegment() );
#endif
			markSection( 0, getFirstValidSegment()*8 );
		}
		return TableImpl<struct TableID>::checkSection( section );
	}

	bool isSchedule() const {
		return id().tableID >= PSI_TID_EIT_SBF_ACTUAL && id().tableID <= PSI_TID_EIT_SEL_OTHER;
	}

	bool isFirstDay() const {
		return id().tableID == PSI_TID_EIT_SBF_ACTUAL ||
		id().tableID == PSI_TID_EIT_SEF_ACTUAL ||
		id().tableID == PSI_TID_EIT_SBF_OTHER ||
		id().tableID == PSI_TID_EIT_SEF_OTHER;
	}

	void validateEvents() {
		{	//	Check all events are valid!
			eit::Events::iterator it=_events.begin();
			while (it != _events.end()) {
				eit::Event *evt = (*it);
				if (!evt) {
					it = _events.erase(it);
				}
				else {
					it++;
				}
			}
		}

		if (isFirstDay()) {
			BYTE firstValidSegment=getFirstValidSegment();
			eit::Events::iterator it=_events.begin();
			while (it != _events.end()) {
				if ((*it)->segment < firstValidSegment) {
					delete (*it);
					it = _events.erase(it);
				}
				else {
					it++;
				}
			}
		}
	}

protected:
	BYTE getFirstValidSegment() const {
		pt::ptime tot;
		time::clock.get( tot );
		pt::time_duration td=tot.time_of_day();
		return (util::BYTE) (td.hours() / util::BYTE(3));
	}

private:
	eit::Events _events;
};

}

EITDemuxer::EITDemuxer( ID pid, bool onlyBasic/*=true*/, bool onlyActual/*=false*/ )
: PSIDemuxer( pid )
{
	_validTables.push_back( PSI_TID_EIT_PF_ACTUAL );	//	Actual TS, present/following
	for (ID t=PSI_TID_EIT_SBF_ACTUAL; t<=PSI_TID_EIT_SBL_ACTUAL; t++) {	//	Actual TS, basic event schedule information
		_validTables.push_back( t );
	}
	if (!onlyBasic) {
		for (ID t=PSI_TID_EIT_SEF_ACTUAL; t<=PSI_TID_EIT_SEL_ACTUAL; t++) {	//	Actual TS, extended event schedule information
			_validTables.push_back( t );
		}
	}

	if (!onlyActual) {
		_validTables.push_back( PSI_TID_EIT_PF_OTHER );	//	Other TS, present/following
		for (ID t=PSI_TID_EIT_SBF_OTHER; t<=PSI_TID_EIT_SBL_OTHER; t++) {	//	Other TS, event schedule information
			_validTables.push_back( t );
		}
		if (!onlyBasic) {
			for (ID t=PSI_TID_EIT_SEF_OTHER; t<=PSI_TID_EIT_SEL_OTHER; t++) {	//	Other TS, event schedule information
				_validTables.push_back( t );
			}
		}
	}
}

EITDemuxer::~EITDemuxer()
{
	CLEAN_ALL(eit::Table *,_tables);
}

//	Getters
bool EITDemuxer::singleTableID() const {
	return false;
}

bool EITDemuxer::filterTableID( util::BYTE &tID, util::BYTE &mask ) const {
	tID = 0x40;
	mask = 0x40;
	return true;
}

//  Signals
void EITDemuxer::onParsed( const ParsedCallback &callback ) {
    _onParsed = callback;
}

//  TSSectionDemuxer Virtual method
bool EITDemuxer::checkTableID( ID tID ) const {
	return std::find(_validTables.begin(),_validTables.end(),tID) != _validTables.end();
}

TableInfo *EITDemuxer::getTable( BYTE *section ) {
	eit::TableID id;
	size_t offset = PSI_PAYLOAD_OFFSET;

	//	Subtable is: tableID+serviceID+tsID+nitID+version
	id.tableID     = PSI_TABLE(section);
	id.serviceID   = PSI_EXTENSION(section);
	id.tsID        = RW(section,offset);
	id.nitID       = RW(section,offset);
	offset++;
	id.lastTableID = RB(section,offset);

	return psi::getTable( _tables, id );
}

void EITDemuxer::onSection( TableInfo *ver, BYTE *section, size_t len ) {
	size_t offset = PSI_PAYLOAD_OFFSET;
	eit::Table *table = (eit::Table *)ver;
	WORD mjd;
	DWORD time;
	BYTE segment = 0xFF;
	bool isSched = table->isSchedule();

	//	Skip tsID + nitID
	offset += 4;
	WORD currentSection = PSI_NUMBER(section);

	//	Is not present/following table?
	if (isSched) {
		// This 8-bit field specifies the number of the last section (that is, the section
		// with the highest section_number) of the sub_table of which this section is part.
		WORD lastSection = util::WORD(PSI_LAST(section)+1);

		// This 8-bit field specifies the number of the last section of this segment
		// of the sub_table. For sub_tables which are not segmented, this field shall set to the same value
		// as the last_section_number field.
		BYTE segLastSection = RB(section,offset);

		segment = (util::BYTE)(currentSection / 8);
		int lastSecOfSeg = (segment+1)*8;

		//	Fix some values
		if (lastSecOfSeg > lastSection) {
			lastSecOfSeg = lastSection;
		}

		if (segLastSection > lastSecOfSeg) {
			segLastSection = (util::BYTE) lastSecOfSeg;
		}

		//	Mark empty secctions in the segment
		table->markSection( segLastSection+1, lastSecOfSeg );

#ifdef DEBUG_EIT
		// This 8-bit field identifies the last table_id used. If only one table is used this is set to
		// the table_id of this table. The chronological order of information is maintained across successive
		// table_id values
		BYTE lastTableID   = RB(section,offset);
		BYTE firstSecOfSeg = segment*8;
		BYTE sections      = (segLastSection+1) - firstSecOfSeg;
		int firstHour = segment*3;
		int firstDay = firstHour/24;
		int hoursInDay = firstHour - (firstDay*24);
		LTRACE( "eit", "On section: tableID=%04x, serviceID=%04x, cur=%02x, last=%02x, lastTable=%02x, seg=%02x, sections=(first=%02x,last=%02x,max=%02x,count=%02x), day=%d, hours=[%d,%d]",
			table->id().tableID, table->id().serviceID, currentSection, lastSection-1, lastTableID, segment, firstSecOfSeg, segLastSection, lastSecOfSeg, sections, firstDay, hoursInDay, hoursInDay+3 );
#else
		offset++;
#endif
	}
	else {
		//	Skip lastSegment + lastTableID
		offset += 2;
	}

	eit::Event *ev;
	while (offset < len) {
		ev = new eit::Event();

		ev->eventID = RW(section,offset);
		ev->segment = segment;

		//	40 bits -> start time
		mjd = RW(section,offset);
		time = RW(section,offset)
		time <<= 8;
		time  |= RB(section,offset);
		ev->startTime = MJDate(mjd,time);

		//	24 bits -> duratin
		time = RW(section,offset)
		time <<= 8;
		time  |= RB(section,offset);
		ev->duration = MJDate( 0, time );

		BYTE tmp = RB(section,offset);
		ev->status = (tmp & 0xE0) >> 5;
		ev->caMode = (tmp & 0x10) ? true : false;

		//	Parse descriptors
		offset --;
		offset += ev->descs.append( section+offset, len-offset );

		// LTRACE( "EITDemuxer", "Event: version=%04x, id=%04x, start=%s, duration=%s, status=%02x, caMode=%d, descs=%d",
		// 	ver->current(), ev->eventID, ev->startTime.asString().c_str(), ev->duration.asString().c_str(),
		// 	ev->status, ev->caMode, ev->descs.length() );

		table->addEvent( isSched, currentSection, ev );
	}
}

void EITDemuxer::onComplete( TableInfo *ver, BYTE * /*section*/, size_t /*len*/ ) {
	if (!_onParsed.empty()) {
		eit::Table *table = (eit::Table *)ver;
		const eit::TableID &id = table->id();

#ifdef DEBUG_EIT
		LTRACE( "eit", "On complete: tableID=%04x, serviceID=%04x, version=%04x",
			id.tableID, id.serviceID, ver->current() );
#endif

		//	Filter events from invalid segments
		table->validateEvents();

		//	Process
		Eit *eit = new Eit( ver->current(), id.tsID, id.nitID, id.serviceID, id.tableID, id.lastTableID, table->events() );
		notify<Eit>( _onParsed, eit );
	}
}

}
