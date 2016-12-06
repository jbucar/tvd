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

#include "request.h"
#include "reply.h"
#include "requestinfo.h"
#include "manager.h"
#include "../log.h"
#include "../assert.h"
#include <boost/bind.hpp>

namespace util {
namespace netlib {

Request::Request( const id::Ident &id, RequestInfo *info, Reply *reply )
	: _id(id), _info(info), _reply(reply)
{
	DTV_ASSERT(info);
	DTV_ASSERT(reply);
	_status = request::idle;
	_progressBytes = 0;
	_done = true;
}

Request::~Request()
{
	DTV_ASSERT( !id::isValid(id()) );
	delete _info;
	delete _reply;
}

void Request::status( request::status st ) {
	LDEBUG( "netlib", "Change request status: old=%d, new=%d", _status, st );
	_status = st;
	if (_status > request::working) {
		mgr()->notifyCompleted(this);
	}
}

const id::Ident &Request::id() const {
	return _id;
}

void Request::start() {
	LDEBUG( "netlib", "Start request: url=%s", info()->url().c_str() );
	_done = false;
	status( request::working );
	_progressBytes = 0;
	_reply->start();
	startImpl();
}

void Request::stop() {
	LDEBUG( "netlib", "Stop request: url=%s", info()->url().c_str() );
	stopImpl();
	waitResources();
	_reply->stop();
	if (_status <= request::working) {
		_status = request::aborted;
	}
	if (info()->needProgress()) {
		info()->progressCallback()(_progressBytes);
	}
	info()->dispatchFinished( _id, _status == request::success );
	_id.reset();
}

void Request::startImpl() {
}

void Request::stopImpl() {
}

RequestInfo *Request::info() const {
	return _info;
}

size_t Request::onData( void *ptr, size_t size, size_t nmemb ) {
	//	IO thread
	DTV_ASSERT(_reply);
	dispatchProgress( size*nmemb );
	return _reply->write( ptr, size, nmemb );
}

size_t Request::onHeader( void *ptr, size_t size, size_t nmemb ) {
	//	IO thread
	DTV_ASSERT(_reply);
	return _reply->writeHeader( ptr, size, nmemb );
}

void Request::onResourcesFree() {
	//	IO thread
	_mutex.lock();
	_done = true;
	_cWakeup.notify_all();
	_mutex.unlock();
}

void Request::waitResources() {
	LTRACE( "netlib", "Wait resources begin" );
	{	//	Lock!
		boost::unique_lock<boost::mutex> lock( _mutex );
		while (!_done) {
			_cWakeup.wait( lock );
		}
	}	//	Unlock!
	LTRACE( "netlib", "Wait resources end" );
}

void Request::dispatchProgress( size_t bytes ) {
	//	IO thread
	if (info()->needProgress()) {
		unsigned long long oldStep = _progressBytes / info()->progressLowMarker();
		unsigned long long newStep = (_progressBytes+bytes) / info()->progressLowMarker();
		_progressBytes += bytes;

		if (oldStep != newStep) {
			LDEBUG( "netlib", "Progress: sum=%lld, old=%d, new=%d", _progressBytes, oldStep, newStep );
			boost::function<void (void)> fnc = boost::bind(info()->progressCallback(),_progressBytes);
			mgr()->notifyProgress( fnc );
		}

	}
}

}
}

