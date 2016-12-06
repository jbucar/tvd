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

#include "binary.h"

namespace util {
namespace serialize {

Binary::Binary( Buffer &buf )
	: _buf(buf), _pos(0), _save(false)
{
}

Binary::~Binary()
{
}

bool Binary::save( const std::vector<std::string> &data ) {
	_save=true;
	size_t s = data.size();
	marshall( s );
	for (size_t i=0; i<s; i++) {
		marshall(const_cast<std::string&>(data[i]));
	}
	return true;
}

bool Binary::load( std::vector<std::string> &data ) {
	_save=false;
	size_t s;
	unmarshall( s );
	data.clear();
	for (size_t i=0; i<s; i++) {
		std::string obj;
		unmarshall(obj);
		data.push_back( obj );
	}
	return true;
}

void Binary::marshall( const std::string &data ) {
	_buf.append( (char *)data.c_str(), data.length()+1 );
}

void Binary::unmarshall( std::string &data ) {
	const char *ptr = (const char *)_buf.data();
	data = ptr+_pos;
	_pos += data.length()+1;
}

void Binary::marshall( const std::vector<std::string> &data ) {
	size_t s = data.size();
	marshall( s );
	for (size_t i=0; i<s; i++) {
		marshall(data[i]);
	}
}

void Binary::unmarshall( std::vector<std::string> &data ) {
	size_t s;
	unmarshall( s );
	data.clear();
	for (size_t i=0; i<s; i++) {
		std::string str;
		unmarshall(str);
		data.push_back( str );
	}
}

}
}
