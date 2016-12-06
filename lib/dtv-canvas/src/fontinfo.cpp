/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "fontinfo.h"
#include <util/log.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <algorithm>

namespace canvas {

  
/**
 * Crea una instancia de la clase, iniciandose con tamaño en 10.
 */
FontInfo::FontInfo()
{
	_size = 10;
	_bold = false;
	_italic = false;
	_smallCaps = false;

	_families.push_back(DEFAULT_FONT);
}

/**
 * Crea una instancia de la clase.
 * @param faces Un vector de strings con las familias de la fuente.
 * @param size El tamaño a aplicarse.
 * @param bold Indica si esta en bold o no
 * @param italic Indica si esta en italic.
 * @param smallCaps Indica si usa smallCaps.
 */
FontInfo::FontInfo( const font::family::Type &faces, size_t size, bool bold, bool italic, bool smallCaps )
{
	_size = size;
	_bold = bold;
	_italic = italic;
	_smallCaps = smallCaps;

	families( faces );
}

/**
 * Crea una instancia de la clase.
 * @param faces Un puntero a char donde se especifican las familias. Estas deben estar separadas por coma.
 * @param size El tamaño a aplicarse.
 * @param bold Indica si esta en bold o no
 * @param italic Indica si esta en italic.
 * @param smallCaps Indica si usa smallCaps.
 */
FontInfo::FontInfo( const char *faces, size_t size, bool bold, bool italic, bool smallCaps )
{
	_size = size;
	_bold = bold;
	_italic = italic;
	_smallCaps = smallCaps;

	//	Add families
	families( faces );
}

/**
 * Crea una instancia de la clase. Se inicializa con todos los parámetros de la fuente dada.
 * @param other Otra instancia de la clase FontInfo.
 */
FontInfo::FontInfo( const FontInfo &other )
	: _families(other._families)
{
	_size = other._size;
	_bold = other._bold;
	_italic = other._italic;
	_smallCaps = other._smallCaps;	
}

/**
 * Destructor de la clase.
 */
FontInfo::~FontInfo()
{
}

/**
 * Imprime todos los parámetros de la instancia.
 */
void FontInfo::show() const {
	LDEBUG( "font", "Family=%s, size=%d, bold=%d, italic=%d, smallCaps=%d",
		familiesAsString().c_str(), _size, _bold, _italic, _smallCaps );
}

/**
 * Agrega nuevas familias de fuentes.
 * @param faces Un vector de strings con los nombres de las familias.
 */
void FontInfo::families( const font::family::Type &faces ) {
	_families.clear();
	BOOST_FOREACH( const std::string &face, faces ) {
		addFamily( face );
	}
	addFamily( DEFAULT_FONT );
}

/**
 * Agrega nuevas familias de fuentes.
 * @param faces Un string separado por coma con las familias de fuentes a agregar.
 */
void FontInfo::families( const std::string &param ) {
	font::family::Type faces;
	boost::split( faces, param, boost::is_any_of( "," ) );
	families( faces );
}

/**
 * Agrega una familia de fuentes.
 * @param family Un string indicando la familia a agregar.
 */
void FontInfo::addFamily( const std::string &family ) {
	std::string tmp = family;
	boost::trim(tmp);
	font::family::Type::const_iterator it = std::find(
		_families.begin(),
		_families.end(),
		tmp
	);
	if (it == _families.end()) {
		_families.push_back( tmp );
	}
}

void FontInfo::fixDejaVuFont() {
	BOOST_FOREACH( std::string &face, _families ) {
		if (face == "DejaVuSans") {
			face = "DejaVu Sans";
		}
	}
}

/**
 * @param size El tamaño de la fuente a setear.
 */
void FontInfo::size( size_t size ) {
	_size = size;
}

/**
 * @param enable Indica si se debe setear la fuenta para que use bold.
 */
void FontInfo::bold( bool enable ) {
	_bold = enable;
}

/**
 * @param enable Indica si se debe setear la fuente para que use italic.
 */
void FontInfo::italic( bool enable ) {
	_italic = enable;
}

/**
 * @param enable Indica si se debe setear la fuente para que use smallCaps.
 */
void FontInfo::smallCaps( bool enable ) {
	_smallCaps = enable;
}

/**
 * @return Un vector con las familias de la fuente.
 */
const font::family::Type& FontInfo::families() const {
	return _families;
}

/**
 * @return Un string separado por coma de las familias de la fuente.
 */
std::string FontInfo::familiesAsString() const {
	std::string tmp;
	font::family::Type::const_iterator it=_families.begin();
	if (it != _families.end()) {
		tmp += (*it);
		it++;
	}
	while (it != _families.end()) {
		tmp += ",";
		tmp += (*it);
		it++;
	}
	return tmp;
}

/**
 * @return El tamaño.
 */
size_t FontInfo::size() const {
	return _size;
}

/**
 * @return True en caso de que este en bold, false de lo contrario.
 */
bool FontInfo::bold() const {
	return _bold;
}

/**
 * @return True en caso de que este en italic, false de lo contrario.
 */
bool FontInfo::italic() const {
	return _italic;
}

/**
 * @return True en caso de que este en smallCaps, false de lo contrario.
 */
bool FontInfo::smallCaps() const {
	return _smallCaps;
}

/**
 * Indica si dos fuentes tienen los mismos parámetros.
 * @param font Fuente a comparar.
 * @return True en caso de que sean iguales, false de lo conteario.
 */
bool FontInfo::operator==(const FontInfo &font) const {
	bool check = true;
	check &= (familiesAsString() == font.familiesAsString());
	check &= (_size == font.size());
	check &= (_bold == font.bold());
	check &= (_italic == font.italic());
	check &= (_smallCaps == font.smallCaps());
	return check;
}

/**
 * India si dos fuentes son distintas.
 * @param font Fuente a comparar.
 * @return True en caso de que sean distintas, true de lo contrario.
 */
bool FontInfo::operator!=(const FontInfo &font) const {
	return !((*this) == font);
}

/**
 * Indica si la fuente es menor que la pasada como parámetro.
 * @param other Una instancia de la clase @c FontInfo.
 * @return True en caso de que sea menor, false de lo contrario.
 */
bool FontInfo::operator<(const FontInfo &other) const {
	if (_families == other._families) {
		if (_size == other._size) {
			if (_bold == other._bold) {
				if (_italic == other._italic) {
					return _smallCaps < other._smallCaps;
				}
				return _italic < other._italic;
			}
			return _bold < other._bold;
		}
		return _size < other._size;
	}
	return _families < other._families;
}

}
