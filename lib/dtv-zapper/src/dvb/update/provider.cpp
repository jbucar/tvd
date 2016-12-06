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
#include "../../update/info.h"
#include "../../update/update.h"
#include "../../zapper.h"
#include <pvr/dvb/tuner.h>
#include <mpegparser/service/extension/download/downloadinfo.h>
#include <mpegparser/service/extension/download/downloaderextension.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>

namespace zapper {
namespace update {
namespace ota {

namespace fs = boost::filesystem;

Provider::Provider( Zapper *zapper, pvr::dvb::Tuner *tuner )
	: update::Provider( "update::OTA", zapper ), _tuner(tuner)
{
	DTV_ASSERT(tuner);
}

Provider::~Provider()
{
}

bool Provider::doStart() {
	//	Create extension
	const Version &ver = version();
	DTV_ASSERT( ver.makerID < 255 && ver.modelID < 255 );
	_ext = new tuner::DownloaderExtension( (util::BYTE)ver.makerID, (util::BYTE)ver.modelID, (tuner::WORD)ver.version );
	_ext->onDownload( boost::bind(&Provider::onUpdate,this,_1) );
	_ext->onProgressChanged( boost::bind(&Update::downloadProgress,update(),_1,_2) );

	//	Attach extension to tuner
	_tuner->attach( _ext );

	return true;
}

void Provider::doStop() {
	_tuner->detach( _ext );
	DEL(_ext);
}

//	Extension notification
void Provider::onUpdate( tuner::DownloadInfo *download ) {
	Info *info = new Info();

	//	Move files to update directory
	try {
		info->isMandatory( download->compulsory() );

		BOOST_FOREACH( const std::string &file, download->files() ) {
			//	Move file to update directory
			fs::path src = file;
			fs::path dst = update()->updateDirectory();
			dst /= src.filename();
			fs::rename( src, dst );

			//	Add to list
			info->addFile( dst.string() );
		}
	} catch ( ... ) {
		LWARN( "Update", "Cannot move file, skip update" );
		DEL(info);
	}

	if (info) {
		update()->addUpdate( info );
	}
}

}
}
}

