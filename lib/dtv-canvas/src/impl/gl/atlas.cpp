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

#include "atlas.h"
#include "generated/config.h"
#include <util/log.h>
#include <util/mcr.h>
#if USE_GLES2
#	ifdef __APPLE__
#		include <ES2/gl.h>
#	else
#		include <GLES2/gl2.h>
#	endif
#else
#	include <GL/glew.h>
#endif
#include <climits>
#include <cstring>
#include <cstdlib>

namespace canvas {
namespace gl {

struct Vec3 {
	Vec3() : x(0), y(0), z(0) {}
	Vec3( int px, int py, int pz ) : x(px), y(py), z(pz) {}

	int x, y, z;
};

Atlas::Atlas()
	:_id(0)
{
	_data = NULL;
}

Atlas::~Atlas()
{
}

bool Atlas::init( int size ) {
	int maxSize;
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxSize );
	if (size>maxSize) {
		LERROR( "gl::Atlas", "Fail to create atlas of size %d. maxSize=%d", size, maxSize );
		return false;
	}
	_size = size;

	// We want a one pixel border around the whole atlas to avoid any artefact when sampling texture
	_nodes.push_back( new Vec3(1, 1, size-2) );
	_data = (util::BYTE*) calloc( size*size, sizeof(util::BYTE) );
	if (!_data) {
		LWARN( "gl::Atlas", "Atlas fail to alloc data");
		return false;
	}
	return true;
}

void Atlas::fin() {
	CLEAN_ALL( Vec3*, _nodes );

	free( _data );
	_data=NULL;

	if (_id) {
		glDeleteTextures( 1, &_id );
		_id = 0;
	}
	_size = 0;
}

Rect Atlas::getRegion( const Size &size ) {
	int y, best_height, best_width, best_index;
	Vec3 *node, *prev;
	Rect region( 0,0, size.w, size.h );

	best_height = INT_MAX;
	best_index  = -1;
	best_width = INT_MAX;
	for (size_t i=0; i<_nodes.size(); i++) {
		y = fit( i, Size(size.w, size.h) );
		if (y >= 0) {
			node = _nodes[i];
			if (((y + size.h)<best_height) || (((y+size.h)==best_height) && (node->z<best_width))) {
				best_height = y + size.h;
				best_index = i;
				best_width = node->z;
				region.x = node->x;
				region.y = y;
			}
		}
	}

	if (best_index == -1) {
		region.x = -1;
		region.y = -1;
		region.w = 0;
		region.h = 0;
		return region;
	}

	_nodes.insert( _nodes.begin()+best_index, new Vec3( region.x, region.y + size.h, size.w ) );
	for (size_t i=best_index+1; i<_nodes.size(); ++i) {
		node = _nodes[i];
		prev = _nodes[i-1];

		if (node->x < (prev->x + prev->z)) {
			int shrink = prev->x + prev->z - node->x;
			node->x += shrink;
			node->z -= shrink;
			if (node->z <= 0) {
				delete( *(_nodes.begin()+i) );
				_nodes.erase( _nodes.begin()+i );
				--i;
			} else {
				break;
			}
		} else {
			break;
		}
	}
	for (size_t i=0; i<_nodes.size()-1; ++i) {
		Vec3 *node = _nodes[i];
		Vec3 *next = _nodes[i+1];
		if (node->y == next->y) {
			node->z += next->z;
			delete( *(_nodes.begin()+(i+1)) );
			_nodes.erase( _nodes.begin()+(i+1) );
			--i;
		}
	}
	return region;
}

void Atlas::setRegion( const Rect &region, const util::BYTE *data, int stride ) {
	size_t size = sizeof( util::BYTE );
	for (int i=0; i<region.h; i++) {
		memcpy(
			_data + ((region.y+i)*_size+region.x )*size,
			data + (i*stride)*size,
			region.w*size );
	}
}

void Atlas::upload() {
	if (_data) {
		if (!_id) {
			glGenTextures( 1, &_id );
		}
		glBindTexture( GL_TEXTURE_2D, _id );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, _size, _size, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data );
	} else {
		LWARN("gl::Atlas", "Atlas fail to upload texture data");
	}
}

int Atlas::fit( const size_t index, const Size &size ) {
	Vec3 *node = _nodes[index];
	int x = node->x;
	int y = node->y;
	int width_left = size.w;

	if ((x + size.w) > (_size-1)) {
		return -1;
	}
	size_t i = index;
	while (width_left > 0) {
		node = _nodes[i++];
		if (node->y > y) {
			y = node->y;
		}
		if ((y+size.h) > (_size-1)) {
			return -1;
		}
		width_left -= node->z;
	}
	return y;
}

GLuint Atlas::id() const {
	return _id;
}

int Atlas::size() const {
	return _size;
}

}
}
