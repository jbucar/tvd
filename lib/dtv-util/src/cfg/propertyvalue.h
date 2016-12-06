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

#include "../any.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>

#define BOOL_TO_STRING( value ) \
if (value) { \
	return "true"; \
} else { \
	return "false"; \
} \

#define CAST_TO_STRING( value ) \
std::string str; \
try { \
	str =  boost::lexical_cast<std::string> (value); \
} catch ( const boost::bad_lexical_cast & /*e*/ ) { \
	throw std::runtime_error("Error, property cannot be casted to a string: " + prop->name() ); \
} \
return str;

namespace util {
namespace cfg {

class PropertyValue;

namespace impl {

template<typename T>
inline std::string convertToString( const PropertyValue *option );
template<typename T>
inline std::string convertToStringDefault( const PropertyValue *option );
template<typename T>
inline void setString( PropertyValue *option, const char *value );
template<typename T>
bool isModified( const PropertyValue *option );

}

class Validator {
public:
	Validator() {}
	virtual ~Validator() {}
	
	virtual bool validate( const util::Any & /*value*/ ) {
		return true;
	}
};

template<typename Structor, typename T>
class ValidatorImpl : public Validator {
public:
	ValidatorImpl( const Structor &st ) {
		_st = st;
	}
	virtual ~ValidatorImpl() {}
	
	virtual bool validate( const util::Any &value ) {
		bool res = false;
		try {
			res = _st(value.get<T>());
		} catch ( util::bad_any_cast & /*e*/) {
		}
		return res;
	}

private:
	Structor _st;
};

class PropertyValue {
public:
	PropertyValue( const std::string &name, const char *def, const std::string &desc="" );
	template<typename T>
	PropertyValue( const std::string &name, T def, const std::string &desc="" );
	virtual ~PropertyValue();

	template<typename T>
	void set( T value );
	void setStr( const std::string &value );
	void setStr( const char *value );
	
	template<typename Structor, typename T>
	void setValidator( const Structor &st );

	template<typename T>
	const T &getDefault() const;
	template<typename T>
	const T &get() const;

	void reset();
	void makeVisible();
	void makeInvisible();
	
	bool isVisible() const;
	bool isModified() const;

	const std::type_info &type() const;
	const std::string asString() const;
	const std::string asStringDefault() const;
	const std::string &name() const;
	const std::string &description() const;

	// Signal on change
	typedef boost::function<void ( const std::string & )> Callback;

	void onChange( const Callback &callback );

protected:
	template<typename T>
	void init();
	
	template<typename T>
	const T &getFrom( const util::Any &value ) const;

	typedef std::string ( *ConvertToString )( const PropertyValue * );
	typedef bool ( *IsModified )( const PropertyValue * );
	typedef void ( *SetString )( PropertyValue *, const char * );

	void sendCallback();

private:
	PropertyValue() {
	}

	std::string _name;
	util::Any _default;
	util::Any _value;
	bool _visible;
	Validator *_validator;
	std::string _description;
	ConvertToString _asString;
	ConvertToString _asStringDefault;
	IsModified _isModified;
	SetString _setString;
	Callback _onChange;	
};

template<typename T>
PropertyValue::PropertyValue( const std::string &name, T def, const std::string &desc )
	: _name( name ), _default( def ), _value( def ), _description( desc ) {
	init<T>();
}

template<typename T>
void PropertyValue::init() {
	_validator = new Validator;
	_visible = true;
	_isModified = &impl::isModified<T>;
	_asString = &impl::convertToString<T>;
	_asStringDefault = &impl::convertToStringDefault<T>;
	_setString = &impl::setString<T>;
}

template<typename T>
const T &PropertyValue::get() const {
	return getFrom<T>( _value );
}

template<typename T>
const T &PropertyValue::getDefault() const {
	return getFrom<T>( _default );
}

template<typename T>
const T &PropertyValue::getFrom( const util::Any &value ) const {
	try {
		return value.get<T> ();
	} catch ( const util::bad_any_cast & /*e*/ ) {
		throw std::runtime_error( "Error trying to get an invalid type to property: " + name() );
	}
}

template<typename T>
void PropertyValue::set( T value ) {
	if (_value.type() != typeid( T )) {
		throw std::runtime_error( "Error trying to set an invalid type to property: " + name() );
	}
	if ( _validator->validate( value ) ) {
		_value = value;
		sendCallback();
	}
}

template<>
inline void PropertyValue::set<const char *>( const char *value ) {
	set(std::string(value));
}

template<typename Structor, typename T>
void PropertyValue::setValidator( const Structor &st ) {
	delete _validator;
	_validator = new ValidatorImpl<Structor,T>(st);
}

namespace impl {

template<typename T>
inline std::string convertToString( const PropertyValue *prop ) {
	CAST_TO_STRING( prop->get<T> ());
}

template<>
inline std::string convertToString<bool> ( const PropertyValue *prop ) {
	BOOL_TO_STRING( prop->get<bool> () );
}

template<typename T>
std::string convertToStringDefault( const PropertyValue *prop ) {
	CAST_TO_STRING( prop->getDefault<T> ());
}

template<>
inline std::string convertToStringDefault<bool> ( const PropertyValue *prop ) {
	BOOL_TO_STRING( prop->getDefault<bool> () );
}

template<typename T>
inline void setString( PropertyValue *prop, const char *value ) {
	try {
		prop->set( boost::lexical_cast<T> ( value ) );
	} catch ( const boost::bad_lexical_cast & /*e*/ ) {
		throw std::runtime_error("Error, value cannot be casted from a string: " + std::string(value) );
	}
}

template<>
inline void setString<bool> ( PropertyValue *prop, const char *value ) {
	if (( strcmp( value, "true" ) == 0 ) | ( strcmp( value, "false" ) == 0 )) {
		prop->set( strcmp( value, "true" ) == 0 );
	} else {
		throw std::runtime_error("Error, bool properties must have only true or false value : " + std::string(value) );
	}
}

template<typename T>
bool isModified( const PropertyValue *prop ) {
	return ( prop->get<T> () != prop->getDefault<T> ());
}

}
}
}
