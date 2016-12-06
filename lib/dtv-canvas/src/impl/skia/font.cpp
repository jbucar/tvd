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
#include <util/log.h>
#include <SkTypeface.h>
#include <SkPaint.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {
namespace skia {

Font::Font( const std::string &filename, size_t size )
	: canvas::Font(filename, size)
{
	_typeface = NULL;
}

Font::~Font()
{
	if (_typeface) {
		_typeface->unref();
		_typeface = NULL;
	}
}

bool Font::initialize() {
	_typeface = SkTypeface::CreateFromFile( filename().c_str() );
	if (_typeface) {
		SkPaint p;
		p.setFilterLevel(SkPaint::kHigh_FilterLevel);
		p.setTextEncoding(SkPaint::kUTF8_TextEncoding);
		p.setTypeface( _typeface );
		p.setTextSize((SkScalar)size());
		p.setAntiAlias(true);

		SkPaint::FontMetrics metrics;
		p.getFontMetrics( &metrics );
		ascent( (int)std::ceil(std::abs(metrics.fAscent)) );
		descent( (int)std::ceil(metrics.fDescent) );
		height( descent()+ascent()+1 );
		for (int i=0; i<256; i++) {
			uint16_t idx;
			char tmp = (char)(i&0xFF);
			char c[2];
			if (i<128) {
				c[0] = tmp;
			} else if (i<192) {
				c[0] = (char)(0xc2);
				c[1] = tmp;
			} else {
				c[0] = (char)(0xc3);
				c[1] = tmp;
			}
			int numGlyphs = _typeface->charsToGlyphs( c, SkTypeface::kUTF8_Encoding, &idx, 1 );
			if (numGlyphs>0) {
				SkScalar adv;
				p.getTextWidths(&c, i<128 ? 1 : 2, &adv );
				Glyph *glyph = new Glyph(idx, boost::math::iround(adv));
				putGlyph(tmp, glyph);
			} else {
				LTRACE( "skia::Font", "Glyph %d not present in charset", i );
			}
		}
	}
	return _typeface!=NULL;
}

SkTypeface *Font::skFont() const {
	return _typeface;
}

}
}
