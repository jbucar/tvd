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

#include "../../text/font.h"
#include "../../size.h"
#include <string>

typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

namespace canvas {
namespace gl {

class Atlas;

struct Glyph : canvas::Glyph {
	Glyph( unsigned int idx, int adv, unsigned char c ) : canvas::Glyph(idx, adv), code(c) {}
	virtual ~Glyph() {}

	// Unicode character
	unsigned char code;

	// Metrics
	Size size;
	int bearingX;
	int bearingY;

	// Texture coordinates
	float x1;
	float y1;
	float x2;
	float y2;
};

class Font : public canvas::Font {
public:
	Font( const std::string &filename, size_t size, FT_Library *ftLib );
	virtual ~Font();

	bool initialize();

	unsigned int textureId() const;

protected:
	bool initFace();
	Glyph *loadGlyph( unsigned char code, unsigned int idx );

private:
	FT_Library *_ftLib;
	FT_Face _face;
	Atlas *_atlas;
};

}
}

/* Glyph metrics:
                    x1                        x2
              |     |<-------- size.w ------->|
              |     +-------------------------+---------------- y1
              |     |    ggggggggg   ggggg    |     ^        ^
              |     |   g:::::::::ggg::::g    |     |        |
              |     |  g:::::::::::::::::g    |     |        |
              |     | g::::::ggggg::::::gg    |     |        |
              |     | g:::::g     g:::::g     |     |        |
 bearingX ----|---->| g:::::g     g:::::g     |  bearingY    |
              |     | g:::::g     g:::::g     |     |        |
              |     | g::::::g    g:::::g     |     |        |
              |     | g:::::::ggggg:::::g     |     |        |
              |     |  g::::::::::::::::g     |     |      size.h
              |     |   gg::::::::::::::g     |     |        |
----baseline--*-----|---- gggggggg::::::g-----*--------------|-------------
             /|     |             g:::::g     |              |
            / |     | gggggg      g:::::g     |              |
      origin  |     | g:::::gg   gg:::::g     |              |
              |     |  g::::::ggg:::::::g     |              |
              |     |   gg:::::::::::::g      |              |
              |     |     ggg::::::gggg       |              |
              |     |        gggggg           |              v
              |     +-------------------------+---------------- y2
              |--------- advance ------------>|
*/
