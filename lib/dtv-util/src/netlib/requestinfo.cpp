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

#include "requestinfo.h"
#include "../log.h"
#include "../assert.h"

namespace util {
namespace netlib {

RequestInfo::RequestInfo()
{
	_verbose = false;
	_writeHeaders = false;
	_verifyPeer = false;
	_verifyHost = false;
	_timeout = -1;
}

RequestInfo::~RequestInfo()
{
}

bool RequestInfo::isValid() const {
	return !_url.empty();
}

void RequestInfo::onFinished( const OnFinished &callback ) {
	_onFinished = callback;
}

void RequestInfo::dispatchFinished( const id::Ident &id, bool result ) {
	LDEBUG( "netlib", "Dispatch result: result=%d", result );
	if (!_onFinished.empty()) {
		_onFinished(id,result);
	}
}

void RequestInfo::onProgress( unsigned int lowMarker, const ProgressCallback &callback ) {
	_progressLowMarker = lowMarker;
	_progress = callback;
}

bool RequestInfo::needProgress() const {
	return _progress.empty() ? false : true;
}

unsigned int RequestInfo::progressLowMarker() const {
	return _progressLowMarker;
}

const ProgressCallback &RequestInfo::progressCallback() const {
	return _progress;
}

void RequestInfo::url( const std::string &url ) {
	_url = url;
}

const std::string &RequestInfo::url() const {
	return _url;
}

void RequestInfo::verbose( bool verbose ) {
	_verbose = verbose;
}

bool RequestInfo::verbose() const {
	return _verbose;
}

void RequestInfo::writeHeaders( bool write ) {
	_writeHeaders = write;
}

bool RequestInfo::writeHeaders() const {
	return _writeHeaders;
}

void RequestInfo::verifyPeer( bool verify ) {
	_verifyPeer = verify;
}

bool RequestInfo::verifyPeer() const {
	return _verifyPeer;
}

void RequestInfo::verifyHost( bool verify ) {
	_verifyHost = verify;
}

bool RequestInfo::verifyHost() const {
	return _verifyHost;
}

void RequestInfo::caCertificate( const std::string &ca_in_form_of_pem ) {
	_ca = ca_in_form_of_pem;
}

const std::string &RequestInfo::caCertificate() const {
	return _ca;
}

void RequestInfo::headers( const std::vector<std::string> &headers ) {
	_headers = headers;
}

const std::vector<std::string> &RequestInfo::headers() const {
	return _headers;
}

void RequestInfo::method( const std::string &req ) {
	_method = req;
}

const std::string RequestInfo::method() const {
	return _method;
}

void RequestInfo::postFields( const std::string &fields ) {
	_fields = fields;
}

const std::string &RequestInfo::postFields() const {
	return _fields;
}

void RequestInfo::connectTimeout( int msTimeout ) {
	_timeout = msTimeout;
}

int RequestInfo::connectTimeout() const {
	return _timeout;
}

}
}

