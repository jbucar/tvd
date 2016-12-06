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
#include "../xml/documenthandler.h"
#include "../xml/nodehandler.h"
#include "../log.h"
#include "../mcr.h"
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <string.h>

namespace util {
namespace serialize {

class XML {
public:
	explicit XML( const std::string &file );
	explicit XML( Buffer *buffer );
	~XML();

	// save/load complex types
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const T &data ) {
		bool result = false;
		if (create()) {
			_save = true;
			util::serialize::Serialize<XML,T>()(*this, const_cast<T&>(data));
			result = saveImpl();
			_document->finalize();
		}
		return result;
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( T &data ) {
		bool result = false;
		if (open()) {
			_save = false;
			util::serialize::Serialize<XML,T>()(*this, data);
			_document->finalize();
			result = true;
		}
		return result;
	}

	// save/load vector of complex type ptr
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const std::vector<T*> &data ) {
		bool result = false;
		if (create()) {
			_save = true;
			saveVector( "Items", data );
			result = saveImpl();
			_document->finalize();
		}
		return result;
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( std::vector<T*> &data ) {
		bool result = false;
		if (open()) {
			_save = false;
			loadVector( "Items", data );
			_document->finalize();
			result = true;
		}
		return result;
	}

	// save/load vector of fundamental types
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	save( const std::vector<T> &data ) {
		bool result = false;
		if (create()) {
			_save = true;
			marshall( "Items", data );
			result = saveImpl();
			_document->finalize();
		}
		return result;
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value, bool >::type
	load( std::vector<T> &data ) {
		bool result = false;
		if (open()) {
			_save = false;
			unmarshall( "Items", data );
			_document->finalize();
			result = true;
		}
		return result;
	}

	// save/load vector of string
	bool save( const std::vector<std::string> &data );
	bool load( std::vector<std::string> &data );

	//	Implementation
	template<typename T>
	void serialize( const std::string &key, T &data ) {
		if (_save) {
			marshall( key, data );
		}
		else {
			unmarshall( key, data );
		}
	}

protected:
	bool create();
	bool open();

	bool saveImpl();

	//	strings
	void marshall( const std::string &key, const std::string &data );
	void unmarshall( const std::string &key, std::string &data );

	//	unsigned char
	void marshall( const std::string &key, const unsigned char data );
	void unmarshall( const std::string &key, unsigned char &data );

	//	char
	void marshall( const std::string &key, const char data );
	void unmarshall( const std::string &key, char &data );

	//	bool
	void marshall( const std::string &key, const bool &data );
	void unmarshall( const std::string &key, bool &data );

	// vector of strings
	void marshall( const std::string &key, const std::vector<std::string> &data );
	void unmarshall( const std::string &key, std::vector<std::string> &data );

	// save/load enum types
	template<typename T>
	typename boost::enable_if_c< boost::is_enum<T>::value >::type
	marshall( const std::string &key, const T &data ) {
		int intData = data;
		marshall( key, intData );
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_enum<T>::value >::type
	unmarshall( const std::string &key, T &data ) {
		std::string strData;
		unmarshall( key, strData );
		try {
			data = (T) boost::lexical_cast<int>( strData );
		}
		catch(boost::bad_lexical_cast &) {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Bad lexical cast: key=%s", key.c_str() );
		}
	}

	// save/load primitive types
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	marshall( const std::string &key, const T &data ) {
		try {
			marshall( key, boost::lexical_cast<std::string>( data ) );
		}
		catch(boost::bad_lexical_cast &) {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Bad lexical cast: key=%s", key.c_str() );
		}
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	unmarshall( const std::string &key, T &data ) {
		std::string strData;
		unmarshall( key, strData );
		try {
			data = boost::lexical_cast<T>( strData );
		}
		catch(boost::bad_lexical_cast &) {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Bad lexical cast: key=%s", key.c_str() );
		}
	}

	//	Save/Load complex types
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	marshall( const std::string &key, const T &data ) {
		util::xml::dom::Node parent = _currentNode;
		_currentNode = _document->createElement( key );
		_document->handler()->appendChild( parent, _currentNode );
		Serialize<XML,T>()(*this, const_cast<T&>(data));
		_currentNode = parent;
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	unmarshall( const std::string &key, T &data ) {
		util::xml::dom::Node parent = _currentNode;
		_currentNode = _document->handler()->child(_currentNode, key);
		if (_currentNode) {
			Serialize<XML,T>()(*this, data);
		} else {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Fail to unmarshall data. Key %s does not exist", key.c_str() );
		}
		_currentNode = parent;
	}

	// Save/Load vector of pointers
	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	marshall( const std::string &key, const std::vector<T*> &data ) {
		saveVector( key, data );
	}

	template<typename T>
	typename boost::disable_if_c< boost::is_fundamental<T>::value || boost::is_enum<T>::value >::type
	unmarshall( const std::string &key, std::vector<T*> &data ) {
		loadVector( key, data );
	}

	// Save/Load vector of fundamental types
	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	marshall( const std::string &key, const std::vector<T> &data ) {
		util::xml::dom::Node parent = _currentNode;
		_currentNode = _document->createElement( key );
		_document->handler()->appendChild( parent, _currentNode );

		//	Add each item
		for (size_t i=0; i<data.size(); i++) {
			marshall("item", data[i]);
		}
		_currentNode = parent;
	}

	template<typename T>
	typename boost::enable_if_c< boost::is_fundamental<T>::value >::type
	unmarshall( const std::string &key, std::vector<T> &data ) {
		data.clear();
		util::xml::dom::Node vecNode = _document->handler()->child(_currentNode, key);
		if (vecNode) {
			std::vector<xml::dom::Node> children;
			_document->handler()->children(vecNode, children);
			for (size_t i=0; i<children.size(); i++) {
				T obj = boost::lexical_cast<T>(_document->handler()->textContent(children[i]));
				data.push_back( obj );
			}
		} else {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Fail to unmarshall vector. Key %s does not exist", key.c_str() );
		}
	}

	template<typename T>
	void saveVector( const std::string &key, const std::vector<T*> &data ) {
		//	Add Items element
		util::xml::dom::Node vecNode = _document->createElement( key );
		_document->handler()->appendChild( _currentNode, vecNode );

		//	Add each item
		for (size_t i=0; i<data.size(); i++) {
			_currentNode = _document->createElement( "item" );
			_document->handler()->appendChild( vecNode, _currentNode );
			util::serialize::Serialize<XML,T>()(*this, const_cast<T&>(*data[i]));
		}
		_currentNode = _document->handler()->parent( vecNode );
	}

	template<typename T>
	void loadVector( const std::string &key, std::vector<T*> &data ) {
		CLEAN_ALL(T *, data);
		util::xml::dom::Node node = _document->handler()->child(_currentNode, key);
		if (node) {
			std::vector<xml::dom::Node> childrens;
			_document->handler()->children(node, childrens);
			for (size_t i=0; i<childrens.size(); i++) {
				_currentNode = childrens[i];
				T *obj = new T();
				util::serialize::Serialize<XML,T>()(*this, *obj);
				data.push_back( obj );
			}
		} else {
			LOG_PUBLISH( WARN, "util", "serializer::xml", "Fail to unmarshall vector. Key %s does not exist", key.c_str());
		}
	}

private:
	std::string _filename;
	Buffer *_buf;
	xml::dom::DocumentHandler *_document;
	xml::dom::Node _currentNode;
	bool _save;
};

}
}

