/*******************************************************************************

Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

This file is part of DTV-connector implementation.

DTV-connector is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2 of the License.

DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

Este archivo es parte de la implementación de DTV-connector.

DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
Free Software Foundation, según la versión 2 de la licencia.

DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
General Reducida GNU.

Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "file.h"
#include "../module.h"
#include "../../../../resourcemanager.h"
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <stdio.h>

namespace tuner {
namespace dsmcc {
namespace biop {

namespace fs = boost::filesystem;

File::File( const ObjectLocation &loc )
  : Object( loc )
{
	_contentLen = 0;
}

File::~File()
{
	clean( _tmpFileName );
}

bool File::parseBody( ResourceManager *resMgr, Module *module, size_t offset, DWORD /*bodyLen*/ ) {
	//  Read content length
	if (!module->readDW( offset, _contentLen )) {
		LWARN( "biop::File", "Cannot read content length" );
		return false;
	}

	if (_contentLen) {
		//  Create temporary file to write to disk
		std::string templateFile = "file_%%%%%%%%";
		FILE *file = resMgr->openTempFileName( templateFile, _tmpFileName );
		if (!file) {
			LWARN( "biop::File", "cannot create temporary file %s", _tmpFileName.c_str() );
			return false;
		}

		//  Write content to disk
		if (module->sendFile( file, 0, offset, _contentLen ) != _contentLen) {
			LWARN( "biop::File", "cannot write file content: file=%s", _tmpFileName.c_str() );
			return false;
		}
		fclose( file );

		//LDEBUG( "dsmcc::File", "key=%lx, filename: %s, size=%ld, offset=%ld", key(), _tmpFileName.c_str(), _contentLen, offset );
	}
	return true;
}

void File::process( Object *parent, const std::string &objName, std::list<Object *> & /*unprocessed*/ ) {
	if (_contentLen) {
		name( getPath(parent,objName) );

		LDEBUG( "dsmcc::File", "        Object name %s processed", name().c_str() );

		//  Rename temporary file to name
		fs::rename( _tmpFileName, name() );
		_tmpFileName = "";
	}
}

void File::clean( const std::string &file ) {
	if (!file.empty() && fs::exists( file.c_str() )) {
		fs::remove( file.c_str() );
	}
}

void File::freeResources( void ) {
	clean( _tmpFileName );
	clean( name() );
}

BYTE File::type() const {
	return 1;
}

bool File::isComplete() const {
	return _tmpFileName.empty();
}

}
}
}
