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

#include "glyphrun.h"
#include "font.h"
#include <SkPoint.h>

namespace canvas {
namespace skia {

GlyphRun::GlyphRun( size_t len )
{
	_glyphs = (uint16_t*) malloc(len * sizeof(uint16_t));
	_positions = (SkPoint*) malloc(len * sizeof(SkPoint));
	_cant = 0;
}

GlyphRun::~GlyphRun()
{
	free(_glyphs);
	free(_positions);
}

void GlyphRun::backward( int cant ) {
	_cant -= cant;
}

void GlyphRun::processGlyph( const Glyph *glyph, const Point &pos ) {
	_positions[_cant].fX = (SkScalar)pos.x;
	_positions[_cant].fY = (SkScalar)pos.y;
	_glyphs[_cant] = (uint16_t)glyph->index;
	_cant++;
}

int GlyphRun::cant() const {
	return _cant;
}

uint16_t *GlyphRun::glyphs() const {
	return _glyphs;
}

SkPoint *GlyphRun::positions() const {
	return _positions;
}

}
}
