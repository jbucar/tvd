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
#include <vector>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace util {

namespace id {
	class Pool;
}

namespace task {
	class Dispatcher;
}

namespace netlib {

class Request;
class Reply;

class Manager {
public:
	Manager();
	virtual ~Manager();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Getter and Setter (before initialization)
	void dispatcher( task::Dispatcher *disp );

	//	Operations
	id::Ident start( RequestInfo *info, Buffer *data, Buffer *headers=NULL );
	id::Ident start( RequestInfo *info, const std::string &file, Buffer *headers=NULL );
	void stop( const id::Ident &id );
	void stopAll();

	//	Instance creation
	static Manager *create( const std::string &use="" );

	//	Implementation
	void notifyProgress( const boost::function<void (void)> &fnc );
	void notifyCompleted( Request *req );

protected:
	typedef std::list<Request *> Requests;

	//	Instance creation
	virtual Request *createRequest( const id::Ident &id, RequestInfo *info, Reply *reply )=0;
	id::Ident start( RequestInfo *info, Reply *reply );

private:
	id::Pool *_pool;
	task::Dispatcher *_disp;
	Requests _requests;
};

}
}

