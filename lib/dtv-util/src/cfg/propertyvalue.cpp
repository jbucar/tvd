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

#include "propertyvalue.h"

#include <string>

namespace util {
namespace cfg {


PropertyValue::PropertyValue( const std::string &name, const char *def, const std::string &desc )
	: _name( name ), _default(std::string(def)), _value(std::string(def)), _description(desc) {
	init<std::string>();
}

PropertyValue::~PropertyValue( void ) {
	delete _validator;
}

const std::string &PropertyValue::name() const {
	return _name;
}

const std::string &PropertyValue::description() const {
	return _description;
}

bool PropertyValue::isModified() const {
	return ( *_isModified )( this );
}
const std::type_info &PropertyValue::type() const {
	return _value.type();
}
const std::string PropertyValue::asString() const {
	return ( *_asString )( this );
}
const std::string PropertyValue::asStringDefault() const {
	return ( *_asStringDefault )( this );
}

void PropertyValue::setStr( const std::string &val ) {
	( *_setString )( this, val.c_str() );
}

void PropertyValue::setStr( const char *val ) {
	( *_setString )( this, val );
}

void PropertyValue::makeVisible() {
	_visible = true;
}
void PropertyValue::makeInvisible() {
	_visible = false;
}
bool PropertyValue::isVisible() const {
	return _visible;
}

void PropertyValue::reset() {
	_value = _default;
}

void PropertyValue::onChange( const Callback &callback ) {
	_onChange = callback;
}

void PropertyValue::sendCallback() {
	if (!_onChange.empty()) {
		_onChange( _name );
	}
}

}
}
