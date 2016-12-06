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

#include "propertynode.h"
#include "treepath.h"

namespace util {
namespace cfg {

PropertyNode::PropertyNode()
	: _name ( "root" ) {
}

PropertyNode::PropertyNode( const std::string &name )
	: _name( name ) {
}

PropertyNode::~PropertyNode() {
	clear();
}

PropertyNode &PropertyNode::operator()( const std::string &name ) const {
	PropertyNode *node = getNodeRecursively( name );
	if (!node) {
		throw std::runtime_error( "Node not exists: " + name );
	}
	return *node;
}

PropertyNode &PropertyNode::addNode( const std::string &name ) {
	if (!hasChild( name )) {
		PropertyNode *node = new PropertyNode( name );
		node->setPath(path());
		_pnodes.push_back( node );
		return *node;
	} else {
		throw std::runtime_error( "Node already exists: " + name );
	}
}

void PropertyNode::removeNode( const std::string &name ) {
	removeFrom<PropertyNode *> (_pnodes, name);
}

void PropertyNode::removeProp( const std::string &name ) {
	removeFrom<PropertyValue *> (_pvalues, name);
}

void PropertyNode::setName( const std::string &name ) {
	_name = name;
}
void PropertyNode::setStr( const std::string &name, const std::string &val ) const{
	PropertyValue *property = getPropertyRecursively( name );
	if (!property) {
		throw std::runtime_error( "Value not exists: " + name );
	}
	return property->setStr(val);
}

bool PropertyNode::isVisible( const std::string &name ) const {
	return getPropertyValue(name).isVisible();
}
void PropertyNode::makeVisible( const std::string &name ) const {
	getPropertyValue(name).makeVisible();
}
void PropertyNode::makeInvisible( const std::string &name ) const {
	getPropertyValue(name).makeInvisible();
}

void PropertyNode::listen( const std::string &name, const PropertyValue::Callback &callback ) const {
	getPropertyValue( name ).onChange( callback );
}

const std::type_info &PropertyNode::type( const std::string &name ) const {
	return getPropertyValue( name ).type();
}

const std::string PropertyNode::asString( const std::string &name ) const {
	return getPropertyValue( name ).asString();
}
const std::string PropertyNode::defaultAsString( const std::string &name ) const {
	return getPropertyValue( name ).asStringDefault();
}

const std::string &PropertyNode::desc( const std::string &name ) const {
	return getPropertyValue( name ).description();
}

void PropertyNode::reset( const std::string &name ) const {
	getPropertyValue( name ).reset();
}

const std::string &PropertyNode::name() const {
	return _name;
}

void PropertyNode::setPath( const std::string &path ) {
	if (path.empty()) {
		_path = name();
	} else {
		_path = path + "." + name();
	}
}

const std::string &PropertyNode::path() const {
	return _path;
}

bool PropertyNode::addPropertyValue( PropertyValue *property ) {
	// returned if the value has been added or not
	bool found = hasValue( property->name() );
	if (!found) {
		_pvalues.push_back( property );
	}
	return !found;
}

PropertyValue &PropertyNode::getPropertyValue( const std::string &name ) const {
	PropertyValue *prop = getPropertyRecursively( name );
	if (!prop) {
		throw std::runtime_error( "Value not exists: " + name );
	}
	return *prop;
}

PropertyValue *PropertyNode::getPropertyValueOrNull( const std::string &name ) const {
	return getFrom<PropertyValue *> (_pvalues, name);
}

PropertyValue *PropertyNode::getPropertyRecursively( const std::string &name ) const {
	TreePath path(name);
	PropertyValue *property = NULL;
	const PropertyNode *node = this;
	if ( path.walkable() ) {
		node = getNodeRecursively( path.path() );
	}
	if (node) {
		property = node->getPropertyValueOrNull( path.property() );
	}
	return property;
}

PropertyNode &PropertyNode::getChild( const std::string &name ) const {
	PropertyNode *node = getChildOrNull(name);
	if (node) {
		return *node;
	}
	throw std::runtime_error( "Node not exists: " + name );
}

PropertyNode *PropertyNode::getChildOrNull( const std::string &name ) const {
	return getFrom<PropertyNode *>( _pnodes, name);
}

PropertyNode *PropertyNode::getNodeRecursively( const std::string &name ) const {
	TreePath path(name);
	PropertyNode *res = NULL;
	if ( path.walkable() ) {
		PropertyNode *child = getChildOrNull(path.root());
		if (child) {
			res = child->getNodeRecursively( path.withoutRoot() );
		}
	} else {
		res = getChildOrNull( path.string() );
	}
	return res;
}

bool PropertyNode::existsValue( const std::string &name ) const {
	return (getPropertyRecursively(name) != NULL);
}

bool PropertyNode::existsNode( const std::string &name ) const {
	return (getNodeRecursively(name) != NULL);
}

bool PropertyNode::hasValue( const std::string &name ) const {
	return (getFrom<PropertyValue *>( _pvalues, name) != NULL);
}

bool PropertyNode::hasChild( const std::string &name ) const {
	return (getFrom<PropertyNode *>( _pnodes, name) != NULL);
}

bool PropertyNode::hasChildren() const {
	return _pnodes.size() > 0;
}

void PropertyNode::clear() {
	CLEAN_ALL(PropertyValue *, _pvalues);
	CLEAN_ALL(PropertyNode *, _pnodes);
}

}
}
