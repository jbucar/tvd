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

#pragma once

#include <pvr/service/service.h>
#include <boost/signals2.hpp>
#include <list>

namespace zapper {
namespace update {

class Info;
class Provider;

/**
 * Ésta clase es una utilidad para descargar @c actualizaciones.
 */
class Update : public pvr::service::Service<Provider> {
public:
	explicit Update( const std::string &ramDisk );
	virtual ~Update();

	//	Notifications
	typedef boost::signals2::signal<void (Info *)> UpdateSignal;
	UpdateSignal &onUpdateReceived();

	typedef boost::signals2::signal<void (unsigned long long step, unsigned long long total)> DownloadProgressSignal;
	DownloadProgressSignal &onDownloadProgress();

	//	Update methods
	void apply( int updateID, bool apply );

	//	Implementation
	void addUpdate( Info *info );
	void downloadProgress( unsigned long long step, unsigned long long total );

	//	Getters
	const std::string &updateDirectory() const;

protected:
	virtual bool init();
	virtual void fin();

private:
	std::string _updateDirectory;
	int _nextID;
	UpdateSignal _onUpdate;
	DownloadProgressSignal _onDownloadProgress;
	std::list<Info *> _updates;
};

}
}

