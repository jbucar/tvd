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

#include "font.h"
#include <util/mcr.h>
#include <util/log.h>

namespace canvas {

Font::Font( const std::string &filename, size_t size )
	: _filename(filename), _size(size)
{
	_ascent = 0;
	_descent = 0;
	_height = 0;
	_glyphs.resize(256);
}

Font::~Font()
{
	CLEAN_ALL(Glyph*, _glyphs);
}

const std::string &Font::filename() const {
	return _filename;
}

size_t Font::size() const {
	return _size;
}

const Glyph *Font::getGlyph( unsigned char code ) const {
	return _glyphs[code];
}

void Font::putGlyph( unsigned char code, Glyph *glyph ) {
	if (getGlyph(code)!=NULL) {
		LWARN("Font", "Glyph %c already exists", code );
	} else {
		_glyphs[code] = glyph;
	}
}

int Font::ascent() const {
	return _ascent;
}

void Font::ascent( int ascent ) {
	_ascent = ascent;
}

int Font::descent() const {
	return _descent;
}

void Font::descent( int descent ) {
	_descent = descent;
}

int Font::height() const {
	return _height;
}

void Font::height( int height ) {
	_height = height;
}

}
