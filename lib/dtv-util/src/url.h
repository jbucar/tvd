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

#include <boost/lexical_cast.hpp>
#include <string>
#include <map>

namespace util {

class Url {
public:
	Url();
	explicit Url( const std::string &str );
	virtual ~Url();

	std::string id();
	const std::string &type() const;
	void type( const std::string &type );	

	template<typename T>
	bool getParam( const std::string &key, T &value ) const {
		std::string tmp;
		if (getCustomParam( key, tmp )) {
			try {
				value = boost::lexical_cast<T>( tmp );
				return true;
			} catch( ... ) {
			}
		}
		return false;
	}
	
	inline void addParam( const std::string &key, const unsigned char &value ) {
		int tmp = value;
		addCustomParam( key, boost::lexical_cast<std::string>(tmp) );
	}
	template<typename T>
	inline void addParam( const std::string &key, const T &value ) {
		addCustomParam( key, boost::lexical_cast<std::string>(value) );
	}

	void rmParam( const std::string &key );

protected:
	typedef std::map<std::string,std::string> Params;
	void parse( const std::string &str );
	bool getCustomParam( const std::string &key, std::string &value ) const;
	void addCustomParam( const std::string &key, const std::string &value );
	
private:
	std::string _type;
	Params _params;
	std::string _default;
};

}

