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
#include "sdttdemuxer.h"
#include "../psi.h"
#include "../tableinfo.h"
#include "../dsmcc/dsmcc.h"
#include "../../text.h"
#include <util/mcr.h>
#include <util/log.h>

//	Implemented based on
//		- ARIB STD - B21

namespace tuner {

namespace sdtt {

struct TableID {
	ID modelID;
	ID tsID;
	ID nitID;
	ID serviceID;

	bool operator==( const struct TableID &sub ) const {
		return modelID == sub.modelID &&
		tsID == sub.tsID &&
		nitID == sub.nitID &&
		serviceID == sub.serviceID;
	}
};

}

SDTTDemuxer::SDTTDemuxer( ID pid )
	: PSIDemuxer( pid )
{
}

SDTTDemuxer::~SDTTDemuxer()
{
	CLEAN_ALL(sdtt::Table *,_tables);
}

ID SDTTDemuxer::tableID() const {
	return PSI_TID_SDTT;
}

//	Signals
void SDTTDemuxer::onParsed( const ParsedCallback &callback ) {
	_onParsed = callback;
}

//	TSSectionDemuxer Virtual method
//	Spec: 6-STD-B21v4-6-E1
void SDTTDemuxer::onSection( BYTE *section, size_t /*len*/ ) {
	std::vector<Sdtt::ElementaryInfo> elements;
	size_t offset = PSI_PAYLOAD_OFFSET;
	WORD wData;

    //  Marker/Model
    ID modelID = PSI_EXTENSION(section);

	ID tsID           = RW(section, offset);
	ID netID          = RW(section, offset);
	ID serviceID      = RW(section, offset);
	BYTE contentCount = RB(section, offset);
	//	LDEBUG( "SDTTDemuxer", "tsID=%x, netId=%x, serviceID=%x, contents=%d", tsID, netID, serviceID, contentCount );

	for (int cont=0; cont < contentCount; cont++) {
		Sdtt::ElementaryInfo element;

		wData = RW(section, offset);
		element.group         = util::BYTE((wData & 0xF000) >> 12);
		element.targetVersion = wData & 0x0FFF;

		wData = RW(section, offset);
		element.newVersion       = (wData & 0xFFF0) >> 4;
		element.downloadLevel    = util::BYTE((wData & 0x000F) >> 2);
		element.versionIndicator = wData & 0x0003;

		wData = RW(section, offset);
		WORD contentDescriptorLength = (wData & 0xFFF0) >> 4;
		//	reserved (4 bits)

		wData = RW(section,offset);
		element.scheduleTimeShiftInfo = wData & 0x000F;

		//	Parse content descriptor
		WORD schedDescLength = (wData & 0xFFF0) >> 4;
		size_t bytes = parseContentDescriptor( section+offset, schedDescLength, contentDescriptorLength, element );
		if (bytes != contentDescriptorLength) {
			LWARN( "SDTTDemuxer", "Content descriptor not parsed correctly: bytes=%ld, len=%d",
				bytes, contentDescriptorLength );
		}
		offset += contentDescriptorLength;

		elements.push_back(element);
	}

	Sdtt *sdtt = new Sdtt( PSI_VERSION(section), modelID, tsID, netID, serviceID, elements );
	notify<Sdtt>( _onParsed, sdtt );
}

size_t SDTTDemuxer::parseContentDescriptor( BYTE *data, WORD schedLen, WORD contentLen, Sdtt::ElementaryInfo &element ) {
	size_t offset=0;

	//	Parse schedule descriptors
	for (int i=0; i<static_cast<int>(schedLen/sizeof(QWORD)); i++) {
		Sdtt::ScheduleElementaryInfo schInfo;
		QWORD dataQW      = RQW(data, offset);
		schInfo.startTime = dataQW >> 24;
		schInfo.duration  = dataQW & 0x00FFFFFF;
		element.schedules.push_back( schInfo );
	}

	//	Parse download content descriptor
	while (offset < contentLen) {
		offset += parseDownloadContentDescriptor( data+offset, contentLen-offset, element.contents );
	}

	return offset;
}

TableInfo *SDTTDemuxer::getTable( BYTE *section ) {
	sdtt::TableID sub;
    size_t offset = PSI_PAYLOAD_OFFSET;

	//	Subtable is: modelID + version + tsID + nitID + serviceID
    sub.modelID   = PSI_EXTENSION(section);
	sub.tsID      = RW(section, offset);
	sub.nitID     = RW(section, offset);
	sub.serviceID = RW(section, offset);
	return psi::getTable( _tables, sub );
}

size_t SDTTDemuxer::parseDownloadContentDescriptor( BYTE *data, size_t len, std::vector<Sdtt::DownloadContentDescriptor> &contents ) {
	size_t offset = 0;
	Sdtt::DownloadContentDescriptor desc;

	BYTE tag	 = RB(data,offset);
	BYTE descLen = RB(data,offset);

	if (len < descLen) {
		LWARN( "SDTTDemuxer", "DownloadContentDescriptor len was wrong!: descLen=%d, len=%ld", descLen, len );
		desc.downloadID   = 0;
		desc.componentTag = 0;
		return descLen;
	}

	if (tag != 0xC9) {
		LWARN( "SDTTDemuxer", "DownloadContentDescriptor tag invalid!" );
		desc.downloadID   = 0;
		desc.componentTag = 0;
		return descLen;
	}

	BYTE aux			   = RB(data,offset);
	desc.reboot			   = (aux & 0x80) ? true : false;
	desc.addOn			   = (aux & 0x40) ? true : false;
	desc.hasCompatibility  = (aux & 0x20) ? true : false;
	desc.hasModuleInfo     = (aux & 0x10) ? true : false;
	bool textInfoFlag	   = (aux & 0x08) ? true : false;

	desc.componentSize	   = RDW(data, offset);
	desc.downloadID		   = RDW(data, offset);
	desc.timeoutDII		   = RDW(data, offset);

	DWORD auxDW			   = RDW(data, offset);
	desc.leakRate		   = (auxDW >> 10);
	desc.componentTag	   = (auxDW & 0x000000FF);

	if (desc.hasCompatibility) {
		//	Parse Compatibility descriptors
		offset += dsmcc::compatibility::parse( data+offset, len-offset, desc.compatibilities );
	}

	if (desc.hasModuleInfo) {
		offset += dsmcc::module::parse( data+offset, len-offset, desc.modules, true );
	}

	BYTE privateDataLen = RB(data,offset);
	offset += privateDataLen;

	if (textInfoFlag) {
		offset += parseLanguage( desc.lang, data+offset );
		offset += parseText( desc.text, data+offset );
	}

	contents.push_back( desc );

	return offset;
}

bool SDTTDemuxer::supportMultipleSections( void ) const {
	//	TODO: Complete!
	return false;
}

}
