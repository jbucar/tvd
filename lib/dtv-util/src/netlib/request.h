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

#include "types.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <vector>

namespace util {
namespace netlib {

namespace request {
	enum status { idle, working, aborted, success, failure };
}

class Reply;

class Request {
public:
	Request( const id::Ident &id, RequestInfo *info, Reply *reply );
	virtual ~Request();

	//	Start/stop
	void start();
	void stop();

	//	Getters/Setters
	void status( request::status st );
	const id::Ident &id() const;
	RequestInfo *info() const;
	void notifyProgress( size_t bytes );

protected:
	virtual void startImpl();
	virtual void stopImpl();
	virtual Manager *mgr() const=0;

	void dispatchProgress( size_t bytes );

	size_t onData( void *ptr, size_t size, size_t nmemb );
	size_t onHeader( void *ptr, size_t size, size_t nmemb );

	void onResourcesFree();
	void waitResources();

private:
	request::status _status;
	bool _done;
	boost::mutex _mutex;
	boost::condition_variable _cWakeup;

	id::Ident _id;
	RequestInfo *_info;
	Reply *_reply;
	unsigned long long _progressBytes;
};

}
}

