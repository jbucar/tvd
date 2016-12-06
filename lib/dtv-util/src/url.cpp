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

#include "url.h"
#include "log.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

namespace util {

Url::Url()
{
}

Url::Url( const std::string &str )
{
	parse( str );
}

Url::~Url()
{
}

std::string Url::id() {
	//      type://params
	std::string tmp = _type;
	tmp += "://";

	std::string params;
	for (Params::const_iterator it=_params.begin(); it!=_params.end(); ++it) {
		if (!params.empty()) {
			params += ",";
		}
		params += (*it).first;
		params += "=";
		params += (*it).second;
	}
	LTRACE( "Url", "type:%s, params:%s", _type.c_str(), params.c_str() );
	return tmp + params;
}

const std::string &Url::type() const {
	return _type;
}

void Url::type( const std::string &type ) {
	_type = type;
}

bool Url::getCustomParam( const std::string &key, std::string &value ) const {
	Params::const_iterator it=_params.find( key );
	if (it != _params.end()) {
		value = (*it).second;
		return true;
	}
	return false;
}

void Url::addCustomParam( const std::string &key, const std::string &value ) {
	//	LDEBUG( "Url", "Add param: key=%s, value=%s", key.c_str(), value.c_str() );
	_params[key] = value;
}

void Url::rmParam( const std::string &key ) {
	_params.erase( key );
}

typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
void Url::parse( const std::string &str ) {
	size_t pos=str.find( "://" );
	if (pos != std::string::npos) {
		_type = str.substr( 0, pos );
		std::string body= str.substr( pos+3 );
		//LDEBUG( "URL", "type=%s, body=%s", _type.c_str(), body.c_str() );

		{	//	Parse body
			boost::char_separator<char> sep(",");
			tokenizer tok(body,sep);
			for (tokenizer::iterator beg=tok.begin(); beg!=tok.end(); ++beg) {
				size_t pos = (*beg).find( "=" );
				if (pos != std::string::npos) {
					addParam( (*beg).substr(0,pos), (*beg).substr(pos+1) );
				}
			}
		}
	}
}

}

