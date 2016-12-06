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
#include "atlas.h"
#include <util/log.h>
#include <util/mcr.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <boost/math/special_functions.hpp>

namespace canvas {
namespace gl {

Font::Font( const std::string &filename, size_t size, FT_Library *ftLib )
	: canvas::Font(filename, size)
{
	_ftLib = ftLib;
	_atlas = new Atlas();
}

Font::~Font() {
	if (_atlas) {
		_atlas->fin();
		DEL( _atlas );
	}
}

bool Font::initialize() {
	if (!initFace()) {
		LERROR("gl::Font", "Fail to initialize FreeType face");
		return false;
	}

	float asc = (float) _face->size->metrics.ascender / 60.0f;
	float des = (float) -_face->size->metrics.descender / 60.0f;
	ascent( (int) std::floor(asc) );
	descent( (int) std::floor(des) );
	height( boost::math::iround(asc+des) );

	std::map<unsigned char, FT_UInt> glyphIndices;
	for (int i=0; i<256; i++) {
		FT_UInt idx = FT_Get_Char_Index( _face, i & 0xFF );
		if (idx) {
			glyphIndices.insert( std::pair<unsigned char,FT_UInt>( (unsigned char) (i&0xFF), idx) );
		} else {
			LTRACE( "gl::Font", "Glyph %d not present in charset", i );
		}
	}

	bool result=false;
	int size = 256;
	std::vector<Glyph*> glyphs(256);
	while (!result && _atlas->init(size)) {
		LTRACE("gl::Font", "Init atlas of size %d", size);
		std::map<unsigned char, FT_UInt>::const_iterator it = glyphIndices.begin();
		for( ; it!=glyphIndices.end(); it++ ) {
			Glyph *glyph = loadGlyph( it->first, it->second );
			result = glyph != NULL;
			if (result) {
				glyphs[it->first] = glyph;
			} else {
				for(int i=0; i<256; i++) {
					if (glyphs[i] != NULL) {
						delete glyphs[i];
						glyphs[i] = NULL;
					}
				}
				_atlas->fin();
				size *= 2;
				LTRACE("gl::Font", "Atlas not big enough to fit all glyphs");
				break;
			}
		}
	}

	if (result) {
		BOOST_FOREACH( Glyph *glyph, glyphs ) {
			if (glyph) {
				putGlyph(glyph->code, glyph);
			}
		}
		_atlas->upload();
	}
	FT_Done_Face( _face );

	return result;
}

Glyph *Font::loadGlyph( unsigned char code, unsigned int idx ) {
	if (FT_Load_Glyph( _face, idx, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT )) {
		LERROR( "gl::Font", "Fail to load glyph for character code: %c", code);
		return NULL;
	}

	FT_GlyphSlot slot = _face->glyph;

	size_t w = slot->bitmap.width;
	size_t h = slot->bitmap.rows;
	// We want each glyph to be separated by at least one black pixel
	Rect region = _atlas->getRegion( Size(w+1, h+1) );
	if ( region.x < 0 ) {
		LTRACE("gl::Font", "Font fail to get region from atlas");
		return NULL;
	}
	size_t x = region.x;
	size_t y = region.y;
	_atlas->setRegion( Rect(x, y, w, h), slot->bitmap.buffer, slot->bitmap.pitch );

	int bl = slot->bitmap_left;
	int bt = slot->bitmap_top;
	FT_Load_Glyph( _face, idx, FT_LOAD_RENDER | FT_LOAD_NO_HINTING );

	Glyph *glyph = new Glyph( idx, boost::math::iround( (float)_face->glyph->advance.x / 64.0f ), code );
	glyph->size.w = w;
	glyph->size.h = h;
	glyph->bearingX = bl;
	glyph->bearingY = bt;
	float size = float(_atlas->size());
	glyph->x1 = float(x)/size;
	glyph->y1 = float(y)/size;
	glyph->x2 = float(x+w)/size;
	glyph->y2 = float(y+h)/size;

	return glyph;
}

bool Font::initFace() {
	if (FT_New_Face( *_ftLib, filename().c_str(), 0, &_face)) {
		LERROR("gl::Font", "Font fail to create face");
		return false;
	}

	if (FT_Select_Charmap( _face, FT_ENCODING_UNICODE )) {
		LERROR("gl::Font", "Font fail to select charmap");
		FT_Done_Face( _face );
		return false;
	}

	if (FT_Set_Char_Size( _face, size()*64, 0, 72, 72 )) {
		LERROR("gl::Font", "Font fail to set char size");
		FT_Done_Face( _face );
		return false;
	}

	LTRACE( "gl::Font", "Loaded %s with size %u: family=%s, style=%s", filename().c_str(), size(), _face->family_name, _face->style_name );

	return true;
}

unsigned int Font::textureId() const {
	return _atlas->id();
}

}
}
