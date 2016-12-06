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

#include "reply.h"
#include "../assert.h"
#include <stdio.h>

#define MAX_HEADERS_LEN 2048

namespace util {
namespace netlib {

Reply::Reply( Buffer *headers/*=NULL*/ )
	: _headers(headers)
{
}

Reply::~Reply()
{
}

bool Reply::start() {
	if (_headers) {
		_headers->resize(0);
	}
	return true;
}

void Reply::stop() {
}

bool Reply::needHeaders() const {
	return _headers ? true : false;
}

int Reply::writeHeader( void *ptr, size_t size, size_t nmemb ) {
	if (_headers && _headers->length() < MAX_HEADERS_LEN) {
		_headers->append( ptr, size*nmemb );
		return size * nmemb;
	}
	return 0;
}

FileReply::FileReply( const std::string &file, Buffer *headers/*=NULL*/ )
	: Reply(headers), _fileName(file)
{
	_file = NULL;
}

FileReply::~FileReply()
{
	DTV_ASSERT(!_file);
}

bool FileReply::start() {
	//	Open file
	_file = fopen( _fileName.c_str(), "wb" );
	if (_file) {
		return Reply::start();
	}
	return false;
}

void FileReply::stop() {
	fclose(_file);
	_file = NULL;
}

int FileReply::write( void *ptr, size_t size, size_t nmemb ) {
	DTV_ASSERT(_file);
	return fwrite( ptr, size, nmemb, _file );
}

BufferReply::BufferReply( Buffer *buf, Buffer *headers/*=NULL*/ )
	: Reply(headers), _buf(buf)
{
	DTV_ASSERT(buf);
}

BufferReply::~BufferReply()
{
}

bool BufferReply::start() {
	_buf->resize(0);
	return Reply::start();
}

int BufferReply::write( void *ptr, size_t size, size_t nmemb ) {
	DTV_ASSERT(_buf);
	_buf->append( ptr, size*nmemb );
	return size*nmemb;
}

}
}

