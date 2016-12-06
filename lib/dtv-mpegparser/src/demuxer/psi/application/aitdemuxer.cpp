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
#include "aitdemuxer.h"
#include "../psi.h"
#include "../tableinfo.h"
#include <util/string.h>
#include <util/mcr.h>
#include <util/log.h>

//	Implemented based on
//		- ABNT NBR 15606-3:2010
//		- ARIB STD - B23 Part2
//		- ETSI TS 102 809 V.1.1.1 (2010-01)

namespace tuner {

namespace ait {

struct TableID {
	ID appType;
	bool operator==( const struct TableID &other ) const {
		return appType == other.appType;
	}
};

}

AITDemuxer::AITDemuxer( ID pid )
: PSIDemuxer( pid )
{
}

AITDemuxer::~AITDemuxer()
{
	CLEAN_ALL(ait::Table *,_tables);
}

//	Getters
ID AITDemuxer::tableID() const {
	return PSI_TID_AIT;
}

//  Signals
void AITDemuxer::onParsed( const ParsedCallback &callback ) {
    _onParsed = callback;
}

//  TSSectionDemuxer Virtual method
void AITDemuxer::onSection( BYTE *section, size_t len ) {
	size_t offset=PSI_PAYLOAD_OFFSET;
	std::vector<ait::Application> applications;
	desc::Descriptors descriptors;

	//  Application Type
	ID appType = PSI_EXTENSION(section);

	//	Common descriptors
	offset += descriptors.append( section+offset, len-offset );

	//	Parse Applications
	WORD appLoopEnd = RW(section,offset);
	appLoopEnd &= 0x0FFF;
	size_t end = offset + appLoopEnd;
	while (offset < end) {
		ait::Application app;

		//	Parse Application Identifier
		app.orgID    = RDW(section,offset);
		app.id       = RW(section,offset);
		app.ctrlCode = RB(section,offset);
		offset += app.descriptors.append( section+offset, len-offset );

		applications.push_back( app );
	}

	Ait *ait = new Ait( PSI_VERSION(section), appType, applications, descriptors );
	notify<Ait>( _onParsed, ait );
}

template<typename T>
struct SubtableFinder {
	SubtableFinder( const T &table ) : _table(table) {}
	bool operator()( const T &table ) {
		return _table.isEqual( table );
	}
	const T &_table;
};

template<class T>
Version chgVersion( std::vector<T> &tables, const T &subtable ) {
	SubtableFinder<T> finder( subtable );
	Version old = PSI_INVALID_VERSION;

	//	Find subtable
	typename std::vector<T>::iterator it = std::find_if( tables.begin(), tables.end(), finder );
	if (it != tables.end()) {
		//	Found sub table: check version!
		if ((*it).version != subtable.version) {
			//	Version changed!
			old = (*it).version;
			(*it).version = subtable.version;
		}
    }
	else {
		tables.push_back( subtable );
	}
	return old;
}

TableInfo *AITDemuxer::getTable( BYTE *section ) {
	ait::TableID sub;
	//      Subtable is: appType+version
	sub.appType = PSI_EXTENSION(section);
	return psi::getTable( _tables, sub );
}

bool AITDemuxer::supportMultipleSections( void ) const {
	//	TODO: Complete!
	return false;
}

}
