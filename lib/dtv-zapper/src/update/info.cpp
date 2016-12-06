/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "info.h"
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace zapper {
namespace update {

namespace fs = boost::filesystem;

Info::Info()
	: _id(-1), _apply(false), _mandatory( false )
{
}

Info::~Info()
{
	if (!_apply) {	//	Delete all files only if no need apply
		BOOST_FOREACH( const std::string &file, _files ) {
			fs::remove( file );
		}
	}
}

void Info::apply() {
	_apply = true;
}

void Info::description( const std::string &description ) {
	_description = description;
}

const std::string &Info::description() const {
	return _description;
}

void Info::isMandatory( bool val ) {
	_mandatory = val;
}

bool Info::isMandatory() const {
	return _mandatory;
}

void Info::addFile( const std::string &file ) {
	_files.push_back( file );
}

const std::vector<std::string> &Info::files() const {
	return _files;
}

int Info::id() const {
	return _id;
}

void Info::id( int i ) {
	_id = i;
}

}
}

