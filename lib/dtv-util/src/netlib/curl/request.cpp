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
#include "manager.h"
#include "../requestinfo.h"
#include "../reply.h"
#include "../../string.h"
#include "../../buffer.h"
#include "../../log.h"
#include "../../assert.h"
#include <string.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

namespace util {
namespace netlib {
namespace curl {

namespace fs = boost::filesystem;

namespace impl {

static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *ctx) {
	Request *r = (Request *)ctx;
	DTV_ASSERT(r);
	return r->writeCallback( ptr, size, nmemb );
}

static size_t headerCallback( void *ptr, size_t size, size_t nmemb, void *ctx) {
	Request *r = (Request *)ctx;
	DTV_ASSERT(r);
	return r->headerCallback( ptr, size, nmemb );
}

}

Request::Request( Manager *mgr, const id::Ident &id, RequestInfo *info, Reply *reply )
	: netlib::Request( id, info, reply ), _mgr(mgr)
{
	_handle = NULL;
	_reqHeaders = NULL;
}

Request::~Request()
{
	DTV_ASSERT(!_handle);
	DTV_ASSERT(!_reqHeaders);
}

void Request::startImpl() {
	LDEBUG( "curl", "Start request" );
	_mgr->addRequest( this );
}

void Request::stopImpl() {
	LDEBUG( "curl", "Stop request" );
	_mgr->removeRequest( this );
}

netlib::Manager *Request::mgr() const {
	return _mgr;
}

#define SET(opt,val)	\
	if (curl_easy_setopt( _handle, CURLOPT_##opt, val ) != CURLE_OK) { LERROR( "curl", "Set curl easy option error: option=%s", CURLOPT_##opt ); return false; }
#define SET_STR(opt,str) { const std::string &tmp = str; if (!tmp.empty()) { SET(opt,tmp.c_str()) } }

bool Request::init() {
	LDEBUG( "curl", "Init request" );

	//	Setup handle
	_handle = curl_easy_init();
	if (!_handle) {
		return false;
	}

	//	Basic setup
	SET( VERBOSE, info()->verbose() ? 1 : 0 );
	SET( PRIVATE, this );
	SET( FOLLOWLOCATION, 1 );
	SET( MAXREDIRS, 10 );
	SET( HTTPAUTH, CURLAUTH_ANY );
	SET( NOPROGRESS, 1 );

	{	//	Set timeout
		int timeout = info()->connectTimeout();
		if (timeout != -1) {
			SET( CONNECTTIMEOUT, timeout );
		}
	}

	//	CA/SSL
	SET( SSL_VERIFYPEER, info()->verifyPeer() ? 1 : 0 );
	SET( SSL_VERIFYHOST, info()->verifyHost() ? 1 : 0 );
	SET_STR( CAINFO, info()->caCertificate() );

	//	Request
	SET_STR( URL, info()->url().c_str() );
	SET_STR( CUSTOMREQUEST, info()->method() );
	SET_STR( POSTFIELDS, info()->postFields() );

	{	//	Set headers
		BOOST_FOREACH( const std::string &header, info()->headers() ) {
			_reqHeaders = curl_slist_append( _reqHeaders, header.c_str() );
		}

		if (_reqHeaders) {
			SET( HTTPHEADER, _reqHeaders );
		}
	}

	//	Setup response
	SET( WRITEFUNCTION, impl::writeCallback );
	SET( WRITEDATA, this );

	if (info()->writeHeaders()) {
		SET( HEADERFUNCTION, impl::headerCallback );
		SET( HEADERDATA, this );
	}

	return true;
}
#undef SET

void Request::fin() {
	LDEBUG( "curl", "Fin request" );
	//	Cleanup curl handle
	curl_easy_cleanup(_handle);
	_handle = NULL;
	curl_slist_free_all(_reqHeaders); _reqHeaders = NULL;
	onResourcesFree();
}

size_t Request::writeCallback( void *ptr, size_t size, size_t nmemb ) {
	return onData( ptr, size, nmemb );
}

size_t Request::headerCallback( void *ptr, size_t size, size_t nmemb) {
	return onHeader( ptr, size, nmemb );
}

CURL *Request::handle() {
	return _handle;
}

}
}
}

