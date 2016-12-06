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

#include "serializer.h"
#include "../buffer.h"
#include "../mcr.h"
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <vector>
#include <string.h>

namespace util {
namespace serialize {

class Binary {
public:
	explicit Binary( Buffer &buf );
	~Binary();

	// save/load complex types
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const T &data ) {
		_save = true;
		util::serialize::Serialize<Binary,T>()(*this, const_cast<T&>(data));
		return true;
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( T &data ) {
		_save = false;
		util::serialize::Serialize<Binary,T>()(*this, data);
		return true;
	}

	//	Save/load vector of T*
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const std::vector<T*> &data ) {
		_save=true;
		size_t s = data.size();
		marshall( s );
		for (size_t i=0; i<s; i++) {
			util::serialize::Serialize<Binary,T>()(*this, const_cast<T&>(*data[i]));
		}
		return true;
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( std::vector<T*> &data ) {
		_save=false;
		size_t s;
		unmarshall( s );
		CLEAN_ALL(T *, data);
		for (size_t i=0; i<s; i++) {
			T *obj = new T();
			util::serialize::Serialize<Binary,T>()(*this, *obj);
			data.push_back( obj );
		}
		return true;
	}

	//	Save/load vector of fundamental
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const std::vector<T> &data ) {
		_save=true;
		size_t s = data.size();
		marshall( s );
		for (size_t i=0; i<s; i++) {
			marshall(const_cast<T&>(data[i]));
		}
		return true;
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( std::vector<T> &data ) {
		_save=false;
		size_t s;
		unmarshall( s );
		data.clear();
		for (size_t i=0; i<s; i++) {
			T obj;
			unmarshall(obj);
			data.push_back( obj );
		}
		return true;
	}

	//	Save/load vector of string
	bool save( const std::vector<std::string> &data );
	bool load( std::vector<std::string> &data );

	//	Implementation
	template<typename T>
	void serialize( const std::string & /*comp*/, T &data ) {
		if (_save) {
			marshall( data );
		}
		else {
			unmarshall( data );
		}
	}

protected:
	//	strings
	void marshall( const std::string &data );
	void unmarshall( std::string &data );
	void marshall( const std::vector<std::string> &data );
	void unmarshall( std::vector<std::string> &data );

	// save/load primitive types
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	marshall( const T &data ) {
		_buf.append( (char *)&data, sizeof(data) );
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	unmarshall( T &data ) {
		memcpy( &data, _buf.bytes()+_pos, sizeof(data) );
		_pos += sizeof(data);
	}

	// save/load complex types
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	marshall( const T &data ) {
		Serialize<Binary,T>()(*this, const_cast<T&>(data));
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	unmarshall( T &data ) {
		Serialize<Binary,T>()(*this, data);
	}

	// Save/Load vector of pointers
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	marshall( const std::vector<T*> &data ) {
		size_t s = data.size();
		marshall( s );
		for (size_t i=0; i<s; i++) {
			util::serialize::Serialize<Binary,T>()(*this, const_cast<T&>(*data[i]));
		}
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	unmarshall( std::vector<T*> &data ) {
		size_t s;
		unmarshall( s );
		CLEAN_ALL(T *, data);
		for (size_t i=0; i<s; i++) {
			T *obj = new T();
			util::serialize::Serialize<Binary,T>()(*this, *obj);
			data.push_back( obj );
		}
	}

	// Save/Load vector of fundamental types
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	marshall( const std::vector<T> &data ) {
		size_t s = data.size();
		marshall( s );
		for (size_t i=0; i<s; i++) {
			marshall(data[i]);
		}
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	unmarshall( std::vector<T> &data ) {
		size_t s;
		unmarshall( s );
		data.clear();
		for (size_t i=0; i<s; i++) {
			T obj;
			unmarshall(obj);
			data.push_back( obj );
		}
	}

private:
	Buffer &_buf;
	size_t _pos;
	bool _save;

	Binary();
	Binary& operator=( const Binary &other );
};

}
}

