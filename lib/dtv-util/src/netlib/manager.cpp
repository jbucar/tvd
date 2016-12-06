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

#include "manager.h"
#include "requestinfo.h"
#include "request.h"
#include "reply.h"
#include "../task/dispatcher.h"
#include "../id/pool.h"
#include "../cfg/configregistrator.h"
#include "../log.h"
#include "../assert.h"
#include "../mcr.h"
#include "generated/config.h"
#if NETLIB_USE_CURL
#	include "curl/manager.h"
#endif

#if NETLIB_USE_CURL
#	define DEFAULT_NETLIB "curl"
#else
#	define DEFAULT_NETLIB "dummy"
#endif

namespace util {
namespace netlib {

REGISTER_INIT_CONFIG( netlib ) {
	root().addNode( "netlib" )
		.addValue( "use", "Network library to instance", std::string(DEFAULT_NETLIB) );
}

Manager *Manager::create( const std::string &useParam ) {
	std::string use;

	if (useParam.empty()) {
		use = util::cfg::getValue<std::string>("netlib.use");
	}
	else {
		use = useParam;
	}

	LINFO( "netlib", "Using network library: %s", use.c_str() );

#if NETLIB_USE_CURL
	if (use == "curl") {
		return new curl::Manager();
	}
#endif

	LERROR("netlib", "No network manager specified");
	DTV_ASSERT(false);
	return NULL;
}

struct FindByID {
	FindByID( const id::Ident &id ) : _id(id) {}
	bool operator()( const Request *req ) const { return req->id() == _id; }
	const id::Ident &_id;

private:
	FindByID &operator=( const FindByID & /*other*/ ) {	return *this; }
};


Manager::Manager()
{
	_pool = new id::Pool( "util::netlib" );
	_disp = NULL;
}

Manager::~Manager()
{
	delete _pool;
}

//	Initialization
bool Manager::initialize() {
	DTV_ASSERT(_disp);
	DTV_ASSERT(_requests.empty());

	_disp->registerTarget( this, "netlib::Manager" );
	return true;
}

void Manager::finalize() {
	_disp->unregisterTarget( this );
	if (!_requests.empty()) {
		LWARN( "netlib", "There are some requests not released" );
	}
}

id::Ident Manager::start( RequestInfo *info, Buffer *data, Buffer *headers ) {
	return start( info, new BufferReply(data,headers) );
}

id::Ident Manager::start( RequestInfo *info, const std::string &file, Buffer *headers ) {
	return start( info, new FileReply(file,headers) );
}

id::Ident Manager::start( RequestInfo *info, Reply *reply ) {
	id::Ident id;

	if (info->isValid()) {
		//	Alloc a new id for request
		id = _pool->alloc();

		//	Write headers if necesary
		info->writeHeaders( reply->needHeaders() );

		//	Create a new request
		Request *req = createRequest(id,info,reply);
		DTV_ASSERT(req);

		_requests.push_back(req);

		//	Start
		req->start();
	}
	else {
		delete info;
		delete reply;
		LWARN( "netlib", "Cannot start request; request invalid" );
	}
	return id;
}

void Manager::stop( const id::Ident &id ) {
	LDEBUG( "netlib", "Stop request" );

	Requests::iterator it=std::find_if(
		_requests.begin(),
		_requests.end(),
		FindByID(id)
	);
	if (it != _requests.end()) {
		Request *req = (*it);
		req->stop();

		delete req;
		_requests.erase(it);
	}
}

void Manager::stopAll() {
	LTRACE( "netlib", "Stop all requests begin" );
	for (Requests::iterator it=_requests.begin(); it!=_requests.end(); ++it) {
		Request *req = (*it);
		req->stop();
	}
	CLEAN_ALL( Request *, _requests );
	LTRACE( "netlib", "Stop all requests end" );
}

void Manager::notifyProgress( const boost::function<void (void)> &fnc ) {
	_disp->post( this, fnc );
}

void Manager::notifyCompleted( Request *req ) {
	_disp->post( this, boost::bind(&Manager::stop,this,req->id()) );
}

void Manager::dispatcher( task::Dispatcher *disp ) {
	DTV_ASSERT(disp);
	_disp = disp;
}

}
}

