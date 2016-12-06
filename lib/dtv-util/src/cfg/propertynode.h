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

#include "propertyvalue.h"
#include <boost/foreach.hpp>
#include <vector>
#include <stdexcept>
#include <string>

namespace util {
namespace cfg {

namespace impl {

template<typename Ptr>
struct Finder {
	Finder( const std::string &name )
		: _name( name ) {
	}

	bool operator()( const Ptr &ptr ) const {
		return ptr->name() == _name;
	}

	const std::string &_name;
private:
	Finder &operator=( const Finder & /*f*/ ) {
		return *this;
	}
};

}

class PropertyNode {
public:
	PropertyNode();
	explicit PropertyNode( const std::string &name );
	virtual ~PropertyNode();

	template<class T>
	PropertyNode &addValue( const std::string &name, const std::string &desc, T val );
	template<class T>
	PropertyNode &addValue( const std::string &name, T val );
	template<class T>
	void set( const std::string &name, T val ) const;
	template<class T>
	const T &get( const std::string &name ) const;
	template<class T>
	const T &getDefault( const std::string &name ) const;
	
	PropertyNode &operator()( const std::string &name ) const;
	PropertyNode &addNode( const std::string &name );

	void removeNode( const std::string &name );
	void removeProp( const std::string &name );

	// Signal on change
	void listen( const std::string &name, const PropertyValue::Callback &callback ) const;

	// visitor
	template<class Operand>
	void visitValues( Operand &visitor ) const;
	template<class Operand>
	void visitNodes( Operand &visitor ) const;

	bool existsValue( const std::string &name ) const;
	bool existsNode( const std::string &name ) const;
	bool hasChild( const std::string &name ) const;
	bool hasValue( const std::string &name ) const;
	bool hasChildren() const;

	template<typename Structor, typename T>
	void setValidator( const std::string &name, const Structor &st );
	
	const std::type_info &type( const std::string &name ) const;
	const std::string asString( const std::string &name ) const;
	const std::string defaultAsString( const std::string &name ) const;
	const std::string &desc( const std::string &name ) const;
	void setStr( const std::string &name, const std::string &val ) const;
	void makeVisible( const std::string &name ) const;
	void makeInvisible( const std::string &name ) const;
	bool isVisible( const std::string &name ) const;
	void reset( const std::string &name ) const;

	void setName( const std::string &name );
	const std::string &name() const;

	void setPath( const std::string &path );
	const std::string &path() const;

	//	Remove and free all children and values
	void clear();

protected:
	bool addPropertyValue( PropertyValue *property);
	
	PropertyValue &getPropertyValue( const std::string &name ) const;
	PropertyValue *getPropertyValueOrNull( const std::string &name ) const;
	PropertyValue *getPropertyRecursively( const std::string &name ) const;

	PropertyNode &getChild( const std::string &name ) const;
	PropertyNode *getChildOrNull( const std::string &name ) const;
	PropertyNode *getNodeRecursively( const std::string &name ) const;

	template<class T>
	T getFrom( const std::vector<T> &vec, const std::string &name ) const;

	template<class T>
	void removeFrom( std::vector<T> &vec, const std::string &name );

private:
	PropertyNode( const PropertyNode & /*node*/ ) {}
	void operator=( const PropertyNode & /*node*/ ) {}
	
	std::string _name;
	std::string _path;
	std::vector<PropertyValue *> _pvalues;
	std::vector<PropertyNode *> _pnodes;

};

template<class T>
PropertyNode &PropertyNode::addValue( const std::string &name, const std::string &desc, T val ) {
	PropertyValue *property = new PropertyValue(name, val, desc);
	if (!addPropertyValue( property )) {
		delete property;
		throw std::runtime_error( "Property " + path() + "." + name + " already exists");
	}
	return *this;
}
template<class T>
PropertyNode &PropertyNode::addValue( const std::string &name, T val ) {
	PropertyValue *property = new PropertyValue(name, val);
	if (!addPropertyValue( property )) {
		delete property;
		throw std::runtime_error( "Value already exists: " + path() + "." + name );
	}
	return *this;
}

template<class T>
void PropertyNode::set( const std::string &name, T val ) const {
	getPropertyValue(name).set(val);
}

template<class T>
const T &PropertyNode::get( const std::string &name ) const {
	return getPropertyValue(name).get<T> ();
}

template<class T>
const T &PropertyNode::getDefault( const std::string &name ) const {
	return getPropertyValue(name).getDefault<T> ();
}

template<class T>
T PropertyNode::getFrom( const std::vector<T> &vec, const std::string &name ) const {
	typename std::vector<T>::const_iterator it = std::find_if( vec.begin(), vec.end(), impl::Finder<T> (name) );
	T res = NULL;
	if (it != vec.end()) {
		res = *it;
	}
	return res;
}

template<typename Structor, typename T>
void PropertyNode::setValidator( const std::string &name, const Structor &st ) {
	getPropertyValue(name).setValidator<Structor, T>(st);
}

template<class T>
void PropertyNode::removeFrom( std::vector<T> &vec, const std::string &name ) {
	typename std::vector<T>::iterator it = std::find_if( vec.begin(), vec.end(), impl::Finder<T> (name) );
	if (it != vec.end()) {
		delete ( *it );
		vec.erase( it );
	}
}

template<class Operand>
void PropertyNode::visitValues( Operand &visitor ) const {
	BOOST_FOREACH( PropertyValue *val, _pvalues ) {
		visitor( val );
	}
}
template<class Operand>
void PropertyNode::visitNodes( Operand &visitor ) const {
	BOOST_FOREACH( PropertyNode *node, _pnodes ) {
		visitor( node );
	}
}

}
}
