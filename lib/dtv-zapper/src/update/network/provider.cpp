/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "provider.h"
#include "../update.h"
#include "../info.h"
#include "../../fetcher/fetcher.h"
#include "../../zapper.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/math/special_functions.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>
#include <zlib.h>

namespace zapper {
namespace update {
namespace network {

namespace fs = boost::filesystem;

Provider::Provider( Zapper *zapper )
	: update::Provider( "update::Network", zapper )
{
	_update = NULL;
}

Provider::~Provider()
{
}

bool Provider::doStart() {
	//	Get version
	const Version &ver = version();

	//	Setup updater
	_update = new fetcher::Fetcher( zapper() );
	_update->steps(20);
	_update->destDirectory( update()->updateDirectory() );
	_update->onUpdateReceived( boost::bind(&Provider::onUpdateReceived,this,_1) );
	_update->onProgress( boost::bind(&Update::downloadProgress,update(),_1,_2) );
	if (!_update->check( ver.makerID, ver.modelID, ver.version )) {
		DEL(_update);
		return false;
	}
	return true;
}

void Provider::doStop() {
	_update->cancel();
	DEL(_update);
}

void Provider::onUpdateReceived( fetcher::Info *fUpdate ) {
	LDEBUG( "Update", "On update received" );
	DTV_ASSERT(fUpdate);

	//	Setup info
	Info *info = new Info();
	info->addFile( fUpdate->fileName );
	info->description( fUpdate->description );
	info->isMandatory( false );

	update()->addUpdate( info );
}

}
}
}

