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
#include "sdtdemuxer.h"
#include "psi.h"
#include "tableinfo.h"
#include "../../service/types.h"
#include <util/mcr.h>

//	Implemented based on
//		- ETSI EN 300 468 v1.5.1

namespace tuner {

namespace sdt {

//	Types
struct TableID {
	ID tableID;
	ID tsID;
	ID networkID;

	bool operator==( const struct TableID &other ) const {
		return tableID == other.tableID &&
		tsID == other.tsID &&
		networkID == other.networkID;
	}
};

class Table : public TableImpl<struct TableID> {
public:
	Table( const struct TableID &id ) : TableImpl<struct TableID>(id) {}
	virtual ~Table() {}

	std::vector<Sdt::Service> services;

protected:
	virtual void reset() {
		services.clear();
	}
};

}


SDTDemuxer::SDTDemuxer( ID pid, bool actual )
	: PSIDemuxer( pid ), _actual(actual)
{
}

SDTDemuxer::~SDTDemuxer()
{
	CLEAN_ALL(sdt::Table *,_tables);
}

bool SDTDemuxer::supportCache() const {
	return true;
}

//  Signals
void SDTDemuxer::onParsed( const SDTDemuxer::ParsedCallback &callback ) {
	_onParsed = callback;
}

//  TSSectionDemuxer Virtual method
DWORD SDTDemuxer::frequency() const {
	return 3000;
}

ID SDTDemuxer::tableID() const {
	return _actual ? PSI_TID_SDT_ACTUAL : PSI_TID_SDT_OTHER;
}

void SDTDemuxer::onSection( TableInfo *ver, BYTE *section, size_t len ) {
	size_t offset = PSI_PAYLOAD_OFFSET;
	sdt::Table *table = (sdt::Table *)ver;

	offset += 3;	//	NetworkID+Reserved
	while (offset < len) {
		Sdt::Service srv;
		BYTE tmp;

		//	ServiceID
		srv.serviceID = RW(section,offset);

		//	EIT flags
		tmp = RB(section,offset);
		srv.eitSchedule = (tmp & 0x02) ? true : false;
		srv.eitPresentFollowing = tmp & 0x01;

		//	Running status & free CA mode
		tmp = RB(section,offset);

		//	Sanity the run status
		srv.status = service::status::type(tmp & 0xE0);
		if (srv.status > service::status::running) {
			srv.status = service::status::running;
		}

		srv.caMode = (tmp & 0x10) ? true : false;

		//  Descriptors
		offset -= 1;
		offset += srv.descriptors.append( section+offset, len-offset );

		table->services.push_back( srv );
	}
}

void SDTDemuxer::onComplete( TableInfo *ver, BYTE *section, size_t /*len*/ ) {
	size_t offset = PSI_PAYLOAD_OFFSET;
	sdt::Table *table = (sdt::Table *)ver;

	Version version = PSI_VERSION(section);
	bool actualTS   = (PSI_TABLE(section) == PSI_TID_SDT_ACTUAL) ? true :false;
	ID tsID         = PSI_EXTENSION(section);
	ID networkID    = RW(section,offset);

	Sdt *sdt = new Sdt( version, tsID, networkID, actualTS, table->services );
	notify<Sdt>( _onParsed, sdt );
}

TableInfo *SDTDemuxer::getTable( BYTE *section ) {
 	sdt::TableID sub;
    size_t offset = PSI_PAYLOAD_OFFSET;

	//	Subtable is: tableID + tsID + originalNetworkID + version
	sub.tableID   = PSI_TABLE(section);
	sub.tsID      = PSI_EXTENSION(section);
	sub.networkID = RW(section,offset);
	return psi::getTable( _tables, sub );
}

}
