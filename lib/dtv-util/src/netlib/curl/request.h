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

#include "../request.h"
#include <curl/curl.h>

namespace util {
namespace netlib {
namespace curl {

class Manager;

class Request : public netlib::Request {
public:
	Request( Manager *mgr, const id::Ident &id, RequestInfo *info, Reply *reply );
	virtual ~Request();

	//	Aux curl
	CURL *handle();

	bool init();
	void fin();

	size_t writeCallback( void *ptr, size_t size, size_t nmemb );
	size_t headerCallback( void *ptr, size_t size, size_t nmemb);
	int progressCallback( int ulnow );

protected:
	virtual void startImpl();
	virtual void stopImpl();
	virtual netlib::Manager *mgr() const;

private:
	Manager *_mgr;
	CURL *_handle;
	struct curl_slist *_reqHeaders;
};

}
}
}

