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
#include "catdemuxer.h"
#include "psi.h"
#include <util/log.h>

//	Based on ISO 13818-1

namespace tuner {

CATDemuxer::CATDemuxer( WORD pid )
	: PSIDemuxer( pid )
{
}

CATDemuxer::~CATDemuxer()
{
}

//	Getters
ID CATDemuxer::tableID() const {
	return PSI_TID_CAT;
}

bool CATDemuxer::supportCache() const {
	return true;
}

//  Signals
void CATDemuxer::onParsed( const ParsedCallback &callback ) {
	_onParsed = callback;
}

//  TSSectionDemuxer Virtual method
DWORD CATDemuxer::frequency() const {
	return 500;
}

void CATDemuxer::onSection( TableInfo * /*ver*/, BYTE *section, size_t len ) {
	// Parse conditional descriptor
	size_t offset = PSI_PAYLOAD_OFFSET;
	while (offset<len) {
		desc::ca::Descriptor desc;
		BYTE dTag = RB(section,offset);
		BYTE dLen = RB(section,offset);
		if (dTag == desc::ident::conditional_access) {
			offset += desc::ca::parse( desc, section+offset, dLen );
			_systems.push_back( desc );
		}
	}
}

void CATDemuxer::onComplete( TableInfo * /*ver*/, BYTE * /*section*/, size_t /*len*/ ) {
	Cat *cat = new Cat( _info.current(), _systems );
	notify<Cat>( _onParsed, cat );
}

TableInfo *CATDemuxer::getTable( BYTE * /*section*/ ) {
	return &_info;
}

void CATDemuxer::onVersionChanged() {
	_systems.clear();
}

}
