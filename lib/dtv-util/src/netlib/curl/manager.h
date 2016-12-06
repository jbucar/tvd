/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../manager.h"
#include <boost/thread.hpp>
#include <curl/curl.h>

namespace util {
namespace netlib {
namespace curl {

class Request;

class Manager : public netlib::Manager {
public:
	Manager();
	virtual ~Manager();

	//	Virtual methods
	virtual bool initialize();
	virtual void finalize();

	void addRequest( Request *req );
	void removeRequest( Request *req );

protected:
	typedef std::vector<Request *> RequestList;

	virtual netlib::Request *createRequest( const id::Ident &id, RequestInfo *info, Reply *reply );

	void loop();
	bool updateHandles( CURLM *_handle );

private:
	boost::thread _thread;
	boost::mutex _mutex;
	boost::condition_variable _cWakeup;
	bool _exit;
	int _activeHandles;

	RequestList _cancelList;
	RequestList _pendingList;
};

}
}
}

