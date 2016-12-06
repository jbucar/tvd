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
#include <cstdlib>

namespace canvas {
namespace gl {

GlyphRun::GlyphRun( size_t len, int maxHeight )
	: _maxHeight(maxHeight)
{
	_vIdx = _tIdx = _iIdx = 0;
	_vertices = (GLfloat*) malloc(len*8*sizeof(GLfloat));
	_texVertices = (GLfloat*) malloc(len*8*sizeof(GLfloat));
	_indices = (GLushort*) malloc(len*6*sizeof(GLushort));
}

GlyphRun::~GlyphRun()
{
	free(_vertices);
	free(_texVertices);
	free(_indices);
}

void GlyphRun::backward( int cant ) {
	_vIdx-=cant*4;
	_tIdx-=cant*4;
	_iIdx-=cant*6;
}

void GlyphRun::processGlyph( const canvas::Glyph *glyph, const Point &pos ) {
	const Glyph *g = dynamic_cast<const Glyph*>(glyph);

	float x0 = (float) (pos.x + g->bearingX);
	float y0 = (float) ((_maxHeight-pos.y) + g->bearingY);
	float x1 = x0 + (float) g->size.w;
	float y1 = y0 - (float) g->size.h;

	// Indices
	GLushort first = (GLushort) _vIdx;
	_indices[_iIdx++] = first;
	_indices[_iIdx++] = (GLushort) (first+1u);
	_indices[_iIdx++] = (GLushort) (first+2u);
	_indices[_iIdx++] = first;
	_indices[_iIdx++] = (GLushort) (first+2u);
	_indices[_iIdx++] = (GLushort) (first+3u);

	// Glyph vertices
	addVertex( _vertices, _vIdx, x0, y0 );
	addVertex( _vertices, _vIdx, x0, y1 );
	addVertex( _vertices, _vIdx, x1, y1 );
	addVertex( _vertices, _vIdx, x1, y0 );

	// Texture vertices
	addVertex( _texVertices, _tIdx, g->x1, g->y1 );
	addVertex( _texVertices, _tIdx, g->x1, g->y2 );
	addVertex( _texVertices, _tIdx, g->x2, g->y2 );
	addVertex( _texVertices, _tIdx, g->x2, g->y1 );
}

void GlyphRun::addVertex( GLfloat *vertices, int &idx, GLfloat x, GLfloat y ) const {
	vertices[idx*2] = x;
	vertices[idx*2+1] = y;
	idx++;
}

int GlyphRun::cant() const {
	return _iIdx;
}

GLfloat *GlyphRun::vertices() const {
	return _vertices;
}

GLfloat *GlyphRun::texVertices() const {
	return _texVertices;
}

GLushort *GlyphRun::indices() const {
	return _indices;
}

}
}
