/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "xmlpersister.h"
#include "../channel.h"
#include <util/serializer/xml.h>
#include <util/assert.h>
#include <boost/filesystem.hpp>

namespace pvr {
namespace channels {
namespace xml {

namespace bfs = boost::filesystem;

//	XML channels
Persister::Persister( const std::string &xmlFile )
	: _xmlFile(xmlFile)
{
	DTV_ASSERT(!_xmlFile.empty());
}

Persister::~Persister()
{
}

bool Persister::load( ChannelList &list ) {
	util::serialize::XML ser(_xmlFile);
	if (!ser.load( list )) {
		bfs::remove(_xmlFile);
	}
	return true;
}

bool Persister::save( const ChannelList &list ) {
	util::serialize::XML ser(_xmlFile);
	return ser.save( list );
}

}
}
}

