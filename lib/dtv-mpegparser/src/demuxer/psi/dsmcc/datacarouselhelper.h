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
#pragma once

#include "dsmcc.h"
#include <vector>
#include <map>

namespace tuner {

class Service;

namespace dsmcc {

class DataCarouselHelper {
public:
	DataCarouselHelper( void );
	virtual ~DataCarouselHelper( void );

	//	Types
	typedef std::pair<BYTE,ID> Tag;
	typedef std::vector<Tag> Tags;
	typedef std::map<ID,Tags> MapOfTags;

	bool collectTags( Service *srv, bool (*cmp)( BYTE type ) );
	ID findTag( BYTE tag ) const;
	ID findTag( ID serviceID, BYTE tag ) const;	
	std::vector<BYTE> clearTags( Service *srv );
	void clearTags();

protected:
	bool addTag( Tags &tags, BYTE tag, ID pid );
	ID findTag( const Tags &tags, BYTE tag ) const;

private:
	MapOfTags _tags;
};

}
}
