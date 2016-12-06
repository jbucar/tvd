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

#include "update.h"
#include "info.h"
#include "provider.h"
#include <util/cfg/configregistrator.h>
#include <util/cfg/cfg.h>
#include <util/fs.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>

namespace zapper {
namespace update {

/**
 * Construye un objeto @c Update.
 * @param ramDisk String con el directorio donde esta la ramDisk.
 */
Update::Update( const std::string &ramDisk )
	: _updateDirectory( util::fs::make( ramDisk, "update" ) )
{
	_nextID = 0;
}

/**
 * Destruye la instancia de la clase @c Update.
 */
Update::~Update()
{
	DTV_ASSERT(_updates.empty());
}

bool Update::init() {
	DTV_ASSERT(_updates.empty());
	//	Clean update directory
	return util::safeCleanDirectory( updateDirectory() );
}

void Update::fin() {
	CLEAN_ALL( Info *, _updates );
}

/**
 * * @return Señal que se emitirá cuándo la una actualización sea recibida
 */
Update::UpdateSignal &Update::onUpdateReceived() {
	return _onUpdate;
}

/**
 * @return Señal que se emitirá cuándo la descarga progrese.
 */
Update::DownloadProgressSignal &Update::onDownloadProgress() {
	return _onDownloadProgress;
}

/**
 * Método invocado cuándo se produce un progreso en la descarga.
 * @param step Total de bits descargados.
 * @param total Total de bits a descargar.
 */
void Update::downloadProgress( unsigned long long step, unsigned long long total ) {
	if (!_onDownloadProgress.empty()) {
		_onDownloadProgress( step, total );
	}
}

void Update::addUpdate( Info *info ) {
	LINFO( "Update", "Update received" );
	info->id( _nextID++ );
	_updates.push_back( info );

	if (!_onUpdate.empty()) {
		_onUpdate( info );
	}
}

//	Getters
const std::string &Update::updateDirectory() const {
	return _updateDirectory;
}

/**
 * Registra como aplicada una actualización.
 * @param updateID Identificador de la actualización aplicada.
 * @param apply Si es true aplica la actualización, en caso contrario la descarta.
 */
void Update::apply( int updateID, bool apply ) {
	LDEBUG( "Update", "Apply update: id=%d, apply=%d", updateID, apply );

	//	Get update ...
	std::list<Info *>::iterator it=std::find_if(
		_updates.begin(),
		_updates.end(),
		boost::bind(&Info::id,_1) == updateID );
	if (it != _updates.end()) {
		//	Remove update from list
		Info *tmp = (*it);

		//	Apply update
		if (apply) {
			tmp->apply();
		}
		else {
			_updates.erase( it );
		}
	}
}

}
}

