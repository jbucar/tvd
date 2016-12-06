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

#pragma once

#include <string>
#include <vector>

namespace canvas {

struct Glyph {
	Glyph( unsigned int idx, int adv ) : index(idx), advance(adv) {}
	virtual ~Glyph() {}

	unsigned int index;
	int advance;
};

/**
 * Esta clase proporciona una abstracción de un archivo de fuente.
 * Permite acceder a los caracteres y a las medidas de la fuente.
 */
class Font {
public:
	Font( const std::string &filename, size_t size );
	virtual ~Font();

	virtual bool initialize()=0;

	const std::string &filename() const;
	size_t size() const;

	int ascent() const;
	int descent() const;
	int height() const;

	const Glyph *getGlyph( unsigned char code ) const;

protected:
	void ascent( int ascent );
	void descent( int descent );
	void height( int height );

	void putGlyph( unsigned char code, Glyph *glyph );

private:
	std::string _filename;
	size_t _size;

	// Font metrics
	int _ascent;
	int _descent;
	int _height;

	std::vector<Glyph*> _glyphs;
};

}
