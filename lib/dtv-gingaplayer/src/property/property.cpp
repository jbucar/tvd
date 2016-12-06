/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "property.h"
#include <canvas/canvas.h>
#include <canvas/color.h>
#include <canvas/system.h>
#include <boost/filesystem.hpp>

namespace player {

namespace fs = boost::filesystem;

namespace check {

bool color( const std::string &color ) {
	canvas::Color tmp;
	return canvas::color::get( color.c_str(), tmp );
}

bool fileExists( const std::string &file ) {
	return fs::exists( file );
}

bool position( canvas::Canvas *canvas, const canvas::Point &point ) {
	const canvas::Size &max = canvas->size();
	return point.x <= max.w && point.x >= 0 && point.y <= max.h && point.y >= 0;
}

bool size( canvas::Canvas *canvas, const canvas::Size &size ) {
	const canvas::Size &max = canvas->size();	
	return size.w <= max.w && size.w > 0 && size.h <= max.h && size.h > 0;
}

bool bounds( canvas::Canvas *canvas, const canvas::Rect &rect ) {
	const canvas::Size &max = canvas->size();	
	return rect.x+rect.w <= max.w && rect.w > 0 && rect.y+rect.h <= max.h && rect.h > 0;
}
	
}

/**
 * Construye un objeto @c Property.
 */
Property::Property()
	: _changed( true )
{
}

/**
 * Destruye un objeto @c Property.
 */
Property::~Property() {
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const bool &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const int &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );	
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const util::DWORD &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const float &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );	
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const std::string &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );	
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const char *value ) {
	if (!value) {
		return false;
	}
	return assign( std::string(value) );
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const canvas::Size &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );	
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const canvas::Point &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );	
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const canvas::Rect &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );		
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const std::pair<std::string,std::string> &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );
}

/**
 * Asigna un valor a una propiedad. Puede producir una excepción si la propiedad no soporta el tipo del valor.
 * @param value Valor a asignar.
 * @return True si se ha podido asignar el valor de la propiedad, false caso contrario.
 * @throw std::runtime\_error
 */
bool Property::assign( const std::pair<util::key::type,bool> &/*value*/ ) {
	throw std::runtime_error( "Invalid cast" );
}

/**
 * Marca a la propiedad indicando que ha cambiado su valor.
 */
void Property::markModified() {
	_changed = true;
}

/**
 * @return True si se necesita refrescar a la propiedad, false caso contrario.
 */
bool Property::needRefresh() const {
	return false;
}

/**
 * @return True si la propiedad puede ser iniciada, de otra manera false.
 */
bool Property::canStart() const {
	return true;
}

/**
 * Aplica el cambio de la propiedad al @em player.
 * @return True si se necesita refrescar a la propiedad, false caso contrario.
 * @see Property::needRefresh.
 */
bool Property::apply() {
	if (_changed) {
		applyChanges();
		_changed=false;
		return needRefresh();
	}
	return false;
}

void Property::applyChanges() {
}

/**
 * @return True si el valor de la propiedad a cambiado, false de otra manera.
 */
bool Property::changed() const {
	return _changed;
}

}
