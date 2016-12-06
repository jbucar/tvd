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
#include "nitdemuxer.h"
#include "psi.h"
#include "tableinfo.h"
#include <util/mcr.h>

//	Implemented based on
//		- ETSI EN 300 468 v1.5.1

namespace tuner {

namespace nit {

struct TableID {
	ID tableID;
	ID networkID;

	bool operator==( const struct TableID &other ) const {
		return tableID == other.tableID && networkID == other.networkID;
	}
};

class Table : public TableImpl<struct TableID> {
public:
	Table( const struct TableID &id ) : TableImpl<struct TableID>(id) {}
	virtual ~Table() {}

	desc::Descriptors descriptors;
	std::vector<Nit::ElementaryInfo> elements;

protected:
	void reset() {
		descriptors.clear();
		elements.clear();
	}
};

}

NITDemuxer::NITDemuxer( ID pid, bool actual )
	: PSIDemuxer( pid ), _actual( actual )
{
}

NITDemuxer::~NITDemuxer()
{
	CLEAN_ALL(nit::Table *,_tables);
}

ID NITDemuxer::tableID() const {
	return _actual ? PSI_TID_NIT_ACTUAL : PSI_TID_NIT_OTHER;
}

bool NITDemuxer::supportCache() const {
	return true;
}

//	Signals
void NITDemuxer::onParsed( const ParsedCallback &callback ) {
	_onParsed = callback;
}

DWORD NITDemuxer::frequency() const {
	return 3000;
}

void NITDemuxer::onSection( TableInfo *ver, BYTE *section, size_t len ) {
	nit::Table *table = (nit::Table *)ver;
	size_t offset = PSI_PAYLOAD_OFFSET;

	//	Network descriptors
	offset += table->descriptors.append( section+offset, len-offset );
	offset += 2;	//	Transport stream loop length

	while (offset < len) {
		Nit::ElementaryInfo info;

		info.tsID  = RW(section,offset);
		info.nitID = RW(section,offset);

		//	Transport Stream Descriptors
		offset += info.descriptors.append( section+offset, len-offset );

		table->elements.push_back( info );
	}
}

void NITDemuxer::onComplete( TableInfo *ver, BYTE *section, size_t /*len*/ ) {
	nit::Table *table = (nit::Table *)ver;
	bool isActualNetwork = (PSI_TABLE(section) == PSI_TID_NIT_ACTUAL);
	WORD networkID	= PSI_EXTENSION(section);
	Version version = PSI_VERSION(section);

	Nit *nit = new Nit( version, networkID, isActualNetwork, table->descriptors, table->elements );
	notify<Nit>( _onParsed, nit );
}

TableInfo *NITDemuxer::getTable( BYTE *section ) {
	nit::TableID sub;

	//	Subtable is: tableID + originalNetworkID + version
	sub.tableID	  = PSI_TABLE(section);
	sub.networkID = PSI_EXTENSION(section);
	return psi::getTable( _tables, sub );
}

}

