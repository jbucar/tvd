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
#include "../buffer.h"

namespace util {
namespace netlib {

class Reply {
public:
	explicit Reply( Buffer *headers=NULL );
	virtual ~Reply();

	virtual bool start();
	virtual void stop();
	virtual int write( void *ptr, size_t size, size_t nmemb )=0;
	int writeHeader( void *ptr, size_t size, size_t nmemb );
	bool needHeaders() const;

private:
	Buffer *_headers;
};

class FileReply : public Reply {
public:
	explicit FileReply( const std::string &file, Buffer *headers=NULL );
	virtual ~FileReply();

	virtual bool start();
	virtual void stop();
	virtual int write( void *ptr, size_t size, size_t nmemb );

private:
	FILE *_file;
	std::string _fileName;
};

class BufferReply : public Reply {
public:
	BufferReply( Buffer *buf, Buffer *headers=NULL );
	virtual ~BufferReply();

	virtual bool start();
	virtual int write( void *ptr, size_t size, size_t nmemb );

private:
	Buffer *_buf;
};

}
}

