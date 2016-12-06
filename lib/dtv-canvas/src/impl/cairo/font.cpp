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
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <fontconfig/fontconfig.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {
namespace cairo {

Font::Font( const std::string &filename, size_t size )
	: canvas::Font(filename, size)
{
	_cairoFont = NULL;
	_cairoScaledFont = NULL;
}

Font::~Font()
{
	if (_cairoFont) {
		cairo_font_face_destroy( _cairoFont );
		_cairoFont = NULL;
	}

	if (_cairoScaledFont) {
		cairo_scaled_font_destroy(_cairoScaledFont);
		_cairoScaledFont = NULL;
	}
}

bool Font::initialize() {
	FcPattern *pattern = FcPatternCreate();
	FcPatternAddString( pattern, FC_FILE, (FcChar8*)filename().c_str() );
	if (_cairoFont) {
		cairo_font_face_destroy( _cairoFont );
	}
	_cairoFont = cairo_ft_font_face_create_for_pattern( pattern );
	FcPatternDestroy( pattern );

	if (_cairoScaledFont) {
		cairo_scaled_font_destroy(_cairoScaledFont);
	}
	// Create scaled font and calculate font metrics
	cairo_matrix_t ctm;
	cairo_matrix_init_identity(&ctm);
	cairo_matrix_t matrix;
	double fontSize = double(size());
	cairo_matrix_init_scale(&matrix, fontSize, fontSize);
	cairo_font_options_t *opts = cairo_font_options_create();
	cairo_font_options_set_hint_style(opts, CAIRO_HINT_STYLE_SLIGHT);
	cairo_font_options_set_antialias(opts, CAIRO_ANTIALIAS_GRAY);
	cairo_font_options_set_hint_metrics( opts, CAIRO_HINT_METRICS_ON);
	_cairoScaledFont = cairo_scaled_font_create(_cairoFont, &matrix, &ctm, opts);
	cairo_font_options_destroy(opts);
	cairo_font_extents_t extents;
	cairo_scaled_font_extents( _cairoScaledFont, &extents );

	ascent( boost::math::iround(extents.ascent) );
	descent( boost::math::iround(extents.descent) );
	height( boost::math::iround(extents.ascent+extents.descent+1) );

	cairo_glyph_t *glyph = NULL;
	int numGlyphs;
	char utf8[2];
	int utf8len=0;
	for (int i=0; i<256; i++) {
		if (i<0x80) {
			utf8[0] = (char)(i&0xFF);
			utf8len = 1;
		} else if (i<0xc0) {
			utf8[0] = '\xc2';
			utf8[1] = (char)(i&0xFF);
			utf8len = 2;
		} else {
			utf8[0] = '\xc3';
			utf8[1] = (char)((i&0xFF)-64);
			utf8len = 2;
		}
		cairo_status_t status = cairo_scaled_font_text_to_glyphs(_cairoScaledFont, 0, 0, utf8, utf8len, &glyph, &numGlyphs, NULL, NULL, NULL);
		if (status == CAIRO_STATUS_SUCCESS && numGlyphs>0) {
			cairo_text_extents_t extents;
			cairo_scaled_font_glyph_extents(_cairoScaledFont, glyph, 1, &extents);
			if (cairo_scaled_font_status(_cairoScaledFont) != CAIRO_STATUS_SUCCESS) {
				LWARN("cairo::Font", "Fail to get glyph extents");
			} else {
				putGlyph( (unsigned char)(i&0xFF), new Glyph(glyph->index, boost::math::iround(extents.x_advance)) );
			}
			cairo_glyph_free(glyph);
			glyph = NULL;
		} else {
			LTRACE( "cairo::Font", "Glyph %d not present in charset", i );
		}
	}

	return true;
}

cairo_font_face_t *Font::cairoFont() const {
	return _cairoFont;
}

cairo_scaled_font_t *Font::cairoScaledFont() const {
	return _cairoScaledFont;
}

}
}
