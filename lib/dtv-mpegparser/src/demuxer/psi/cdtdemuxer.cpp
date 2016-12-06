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
#include "cdtdemuxer.h"
#include "psi.h"
#include "tableinfo.h"
#include "../../service/types.h"
#include <util/mcr.h>

//	Implemented based on
//		- ETSI EN 300 468 v1.5.1

namespace tuner {

namespace cdt {

//	Types
struct TableID {
	ID tableID;
	ID downloadID;
	ID networkID;

	bool operator==( const struct TableID &other ) const {
		return tableID == other.tableID &&
		downloadID == other.downloadID &&
		networkID == other.networkID;
	}
};

class Table : public TableImpl<struct TableID> {
public:
	Table( const struct TableID &id ) : TableImpl<struct TableID>(id) {}
	virtual ~Table() {}

	std::vector<Cdt::Download> downloads;

protected:
	virtual void reset() {
		downloads.clear();
	}
};

}


CDTDemuxer::CDTDemuxer( ID pid )
	: PSIDemuxer( pid )
{
}

CDTDemuxer::~CDTDemuxer()
{
	CLEAN_ALL(cdt::Table *,_tables);
}

//  Signals
void CDTDemuxer::onParsed( const CDTDemuxer::ParsedCallback &callback ) {
	_onParsed = callback;
}

//  TSSectionDemuxer Virtual method
DWORD CDTDemuxer::frequency() const {
	return 5000;
}

ID CDTDemuxer::tableID() const {
	return PSI_TID_CDT;
}

void CDTDemuxer::onSection( TableInfo *ver, BYTE *section, size_t len ) {
	size_t offset = PSI_PAYLOAD_OFFSET;
	cdt::Table *table = (cdt::Table *)ver;

	offset += 2;	//	NetworkID
	Cdt::Download download;

	//	data_type
	download.dataType = RB(section,offset);

	//  Descriptors
	offset += download.descriptors.append( section+offset, len-offset );

	//	Copy data
	download.data.copy( section+offset, len-offset );

	table->downloads.push_back( download );
}

void CDTDemuxer::onComplete( TableInfo *ver, BYTE *section, size_t /*len*/ ) {
	size_t offset = PSI_PAYLOAD_OFFSET;
	cdt::Table *table = (cdt::Table *)ver;

	Version version = PSI_VERSION(section);
	ID downloadID   = PSI_EXTENSION(section);
	ID networkID    = RW(section,offset);

	Cdt *cdt = new Cdt( version, downloadID, networkID, table->downloads );
	notify<Cdt>( _onParsed, cdt );
}

TableInfo *CDTDemuxer::getTable( BYTE *section ) {
 	cdt::TableID sub;
    size_t offset = PSI_PAYLOAD_OFFSET;

	//	Subtable is: tableID + downloadID + originalNetworkID + version
	sub.tableID    = PSI_TABLE(section);
	sub.downloadID = PSI_EXTENSION(section);
	sub.networkID  = RW(section,offset);
	return psi::getTable( _tables, sub );
}

}
