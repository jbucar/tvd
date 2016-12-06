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
#include <stdlib.h>

namespace zapper {
namespace middleware {

class Application;
class Provider;
typedef boost::signals2::signal<void (int id, int step, int total)> OnDownloadProgress;

/**
 * Clase que contiene a las aplicaciones y provee funcionalidad para iniciar/cancelar aplicaciones.
 */
class Middleware : public pvr::service::Service<Provider> {
public:
	Middleware();
	virtual ~Middleware();

	//	Public apps methods
	const std::vector<Application *> &getApplications() const;
	bool startApplication( int id );
	void stopApplication( int id );
	void stopAll();
	bool isAnyRunning() const;
	void lockApplications( bool state );
	template<typename O>
	inline Application *findApplication( const O &oper ) const;

	//	Public notifications
	OnDownloadProgress &onDownloadProgress();

	//	Application provider methods (implementation)
	int addApplication( Application *info );
	void rmApplication( int appID );
	void sendDownloadProgress( int appID, int step, int total );

protected:
	typedef std::vector<Application *> Applications;

private:
	int _id;
	Applications _apps;
	OnDownloadProgress _onDownloadProgress;
};

template<typename O>
inline Application *Middleware::findApplication( const O &oper ) const {
	for (size_t i=0; i<_apps.size(); i++) {
		Application *app = _apps[i];
		if (oper(app)) {
			return app;
		}
	}
	return NULL;
}

}
}

