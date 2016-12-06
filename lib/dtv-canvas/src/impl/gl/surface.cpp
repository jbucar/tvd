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

#include "surface.h"
#include "error.h"
#include "canvas.h"
#include "font.h"
#include "glyphrun.h"
#include "../../size.h"
#include "../../rect.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/registrator.h>
#include <boost/foreach.hpp>
#include <boost/math/special_functions.hpp>
#include "generated/config.h"
#if USE_GLES2
#	ifdef __APPLE__
#		include <ES2/gl.h>
#	else
#		include <GLES2/gl2.h>
#	endif
#else
#	include <GL/glew.h>
#endif
#include <FreeImage.h>
#include <cfloat>
#include <cstdlib>

#define SET_CANVAS(c)	\
	_canvas = dynamic_cast<Canvas*>(c);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
#	define SWAP_PIXEL_COLOR \
			pixel |= ((srcStride[j] & 0x00FF0000) >> 16) << FI_RGBA_RED_SHIFT; \
			pixel |= ((srcStride[j] & 0x000000FF) <<  0) << FI_RGBA_BLUE_SHIFT;
#else
#	define SWAP_PIXEL_COLOR \
			pixel |= ((srcStride[j] & 0x00FF0000) >> 16) << FI_RGBA_BLUE_SHIFT; \
			pixel |= ((srcStride[j] & 0x000000FF) <<  0) << FI_RGBA_RED_SHIFT;
#endif

#if USE_GLES2 || (FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB)
#	define SRC_PIXEL_FORMAT GL_RGBA
#else
#	define SRC_PIXEL_FORMAT GL_BGRA
#endif

#if USE_GLES2
#	define COPY_STRIDE \
		for (int j=0; j<_size.w; j++) { \
			util::DWORD pixel = 0; \
			pixel |= ((srcStride[j] & 0xFF000000) >> 24) << FI_RGBA_ALPHA_SHIFT; \
			pixel |= ((srcStride[j] & 0x0000FF00) >>  8) << FI_RGBA_GREEN_SHIFT; \
			SWAP_PIXEL_COLOR \
			dstStride[j] = pixel; \
		}
#else
#	define COPY_STRIDE memcpy(dstStride, srcStride, _size.w*sizeof(util::DWORD));
#endif

namespace canvas {
namespace gl {

class System;

REGISTER_INIT( surface_gl_freeimage ) {
	FreeImage_Initialise();
}

REGISTER_FIN( surface_gl_freeimage ) {
	FreeImage_DeInitialise();
}

namespace impl {

static bool loadTexture( const std::string &filename, GLuint *texID, Size &size ) {
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType( filename.c_str(), 0 );
	if (fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename( filename.c_str() );
	}
	if ( fif == FIF_UNKNOWN ) {
		return false;
	}

	FIBITMAP *tmpDib=NULL;
	if (FreeImage_FIFSupportsReading(fif)) {
		tmpDib = FreeImage_Load(fif, filename.c_str());
	}
	if (!tmpDib) {
		LERROR("gl::Surface", "Fail to load image: %s", filename.c_str());
		return false;
	}

	FIBITMAP *dib = FreeImage_ConvertTo32Bits( tmpDib );
	FreeImage_Unload( tmpDib );

	FreeImage_PreMultiplyWithAlpha(dib);

	// Allocate a raw buffer
	size.w = FreeImage_GetWidth(dib);
	size.h = FreeImage_GetHeight(dib);
	if ((size.w==0) || (size.h==0)) {
		LERROR("gl::Surface", "Invalid image dimensions: size=(%d,%d)", size.w, size.h );
		return false;
	}

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
	// Convert format from BGRA to RGBA
	for( int y=0; y<size.h; y++) {
		BYTE *bits = FreeImage_GetScanLine( dib, y );
		for( int x=0; x<size.w; x++) {
			BYTE tmp = bits[FI_RGBA_RED];
			bits[FI_RGBA_RED] = bits[FI_RGBA_BLUE];
			bits[FI_RGBA_BLUE] = tmp;
			bits += 4;
		}
	}
#endif

	int scan_width = FreeImage_GetPitch( dib );
	BYTE *bits = (BYTE*) malloc( size.h * scan_width );
	if (!bits) {
		LERROR("gl::Surface", "Fail to alloc memory for image");
		FreeImage_Unload( dib );
		return false;
	}
	// convert the bitmap to raw bits (top-left pixel first)
	FreeImage_ConvertToRawBits( bits, dib, scan_width, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE );
	FreeImage_Unload( dib );

	glGenTextures( 1, texID );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenTextures");

	glBindTexture( GL_TEXTURE_2D, *texID );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindTexture");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glTexImage2D");

	free( bits );

	return true;
}

}

Surface::Surface( canvas::Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) )
{
	SET_CANVAS(canvas);

	_size = img->size;
	_clip = Rect(0, 0, _size.w, _size.h);
	_alpha = 1.0f;
	_frameBuffer = 0;
	_texture = 0;

	glGenFramebuffers( 1, &_frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenFramebuffers");

	glBindFramebuffer( GL_FRAMEBUFFER, _frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindFramebuffer");

	glGenTextures( 1, &_texture );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenTextures");

	glBindTexture( GL_TEXTURE_2D, _texture );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindTexture");

	util::DWORD *data = (util::DWORD*) malloc( _size.w * _size.h * sizeof(util::DWORD));
	if (!data) {
		LERROR("gl::Surface", "Fail to alloc memory for image data");
		throw std::runtime_error("[canvas::gl::Surface] Fail to alloc memory for image data");
	}

	// Change pixel order so bottom left pixel is first (OpenGL pixel order)
	for (int i=_size.h-1, pos=0; i>=0; i--, pos++) {
		util::DWORD *srcStride = ((util::DWORD*) img->data) + (_size.w * i);
		util::DWORD *dstStride = data + (_size.w * pos);
		COPY_STRIDE
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _size.w, _size.h, 0, SRC_PIXEL_FORMAT, GL_UNSIGNED_BYTE, data );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glTexImage2D");
	free( data );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glFramebufferTexture2D");

	GLenum result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		LERROR("gl::Surface", "Unable to create Surface from image data");
		throw std::runtime_error("[canvas::gl::Surface] Unable to create Surface from image data. Fail to create FrameBuffer object.");
	}

	glFlush();
}

Surface::Surface( canvas::Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) )
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::gl::Surface] Cannot create surface!!! Invalid bounds");
	}

	SET_CANVAS(canvas);

	_alpha = 1.0f;
	_size.w = rect.w;
	_size.h = rect.h;
	_clip = Rect(0, 0, _size.w, _size.h);

	_frameBuffer = 0;
	_texture = 0;

	glGenFramebuffers( 1, &_frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenFramebuffers");

	glBindFramebuffer( GL_FRAMEBUFFER, _frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindFramebuffer");

	glGenTextures(1, &_texture);
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenTextures");

	glBindTexture(GL_TEXTURE_2D, _texture);
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindTexture");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size.w, _size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glTexImage2D");

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glFramebufferTexture2D");

	GLenum result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		LERROR("gl::Surface", "Unable to create Surface from rect=[%d,%d,%d,%d], result=%x, error=%x", rect.x, rect.y, rect.w, rect.h, result, glGetError());
		throw std::runtime_error("[canvas::gl::Surface] Unable to create FrameBuffer object");
	}

	setupFramebuffer( _frameBuffer, _size, _clip );

	glClear( GL_COLOR_BUFFER_BIT );

	glFlush();
}

Surface::Surface( canvas::Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) )
{
	SET_CANVAS(canvas);

	_frameBuffer=0;
	glGenFramebuffers( 1, &_frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glGenFramebuffers");

	glBindFramebuffer( GL_FRAMEBUFFER, _frameBuffer );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glBindFramebuffer");

	impl::loadTexture( file, &_texture, _size );
	_clip = Rect(0, 0, _size.w, _size.h);

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );
	CHECK_GL_ERROR_THROW( "[canvas::gl::Surface] GL error!", "glFramebufferTexture2D");

	GLenum result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		LERROR("gl::Surface", "Unable to create Surface from image file='%s'", file.c_str());
		throw std::runtime_error("[canvas::gl::Surface] Unable to create Surface from image. Fail to create FrameBuffer object.");
	}

	_alpha = 1.0f;

	glFlush();
}

Surface::~Surface()
{
	glDeleteTextures( 1, &_texture );
	CHECK_GL_ERROR_LOG("glDeleteTextures");

	if (_frameBuffer!=0) {
		glDeleteFramebuffers( 1, &_frameBuffer );
		CHECK_GL_ERROR_LOG("glDeleteFramebuffers");
	}
}

canvas::Canvas *Surface::canvas() const {
	DTV_ASSERT(_canvas);
	return _canvas;
}

/****************************** Size functions ********************************/

Size Surface::getSize() const {
	return _size;
}

/****************************** Clip functions ********************************/

bool Surface::getClip( Rect &rect ) {
	rect = _clip;
	return true;
}

void Surface::setClipImpl( const Rect &rect ) {
	_clip = rect;
}

/****************************** Line functions ********************************/

void Surface::addLine( GLfloat *vertices, int &idx, int x1, int y1, int x2, int y2 ) const {
	int i = idx * 2;
	vertices[i]   = float(x1)+0.125f;
	vertices[i+1] = float(y1)+0.125f;
	vertices[i+2] = float(x2)+0.125f;
	vertices[i+3] = float(y2)+0.125f;

	if (x1<x2) {
		vertices[i+2] += 0.75f;
	} else if (x1>x2) {
		vertices[i] += 0.75f;
	}
	if (y1<y2) {
		vertices[i+3] += 0.75f;
	} else {
		vertices[i+1] += 0.75f;
	}

	idx+=2;
}

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	GLfloat vertices[4];
	int idx = 0;
	addLine( vertices, idx, x1, y1, x2, y2 );

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINES, 0, idx );

	glFlush();
}

/****************************** Rect functions ********************************/

void Surface::drawRectImpl( const Rect &rect ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	float x1 = float(rect.x)+0.825f;
	float x2 = float(rect.x+rect.w-1)+0.125f;
	float y1 = float(rect.y)+0.125f;
	float y2 = float(rect.y+rect.h)-0.125f;
	GLfloat vertices[] = {
		x1, y1,
		x2, y1,
		x2, y2,
		x1, y2
	};

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINE_LOOP, 0, 4 );

	glFlush();
}

void Surface::fillRectImpl( const Rect &rect ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	GLfloat x2 = (float)(rect.x + rect.w);
	GLfloat y2 = (float)(rect.y + rect.h);
	GLfloat vertices[] = {
		float(rect.x),            y2,
		           x2,            y2,
		float(rect.x), float(rect.y),
		           x2, float(rect.y)
	};

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glFlush();
}

/*************************** RoundRect functions ******************************
Vertex positions:
       ___________
      /1         2\
     /             \
     0             3  -- y1
    |               |
    |               |
     7             4  -- y2
     \             /
      \6_________5/

       |         |
       x1       x2
*/
void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	if (arcW==0 || arcH==0) {
		drawRectImpl( rect );
		return;
	}

	setupFramebuffer( _frameBuffer, _size, _clip );

	float mw = float(rect.w) / 2.0f;
	float mh = float(rect.h) / 2.0f;
	float aw = (float(arcW)>mw) ? mw : float(arcW);
	float ah = (float(arcH)>mh) ? mh : float(arcH);

	int x1 = rect.x + boost::math::iround(aw);
	int x2 = rect.x + rect.w - boost::math::iround(aw) - 1;
	int y1 = rect.y + boost::math::iround(ah);
	int y2 = rect.y + rect.h - boost::math::iround(ah) - 1;

	const int step = 6;
	const int nv = 4 * (90 / step); // 4 curves of 90 degrees with vertex separated by step degrees
	GLfloat vertices[2*nv];
	int idx=0;

	Point center(x1,y1);
	addArcVertices( vertices, idx, center, aw, ah, 180, 270, step ); // Top-Left arc (0-1)
	center.x = x2;
	addArcVertices( vertices, idx, center, aw, ah, 270, 360, step ); // Top-Right arc (2-3)
	center.y = y2;
	addArcVertices( vertices, idx, center, aw, ah, 0, 90, step ); // Bottom-Right arc (4-5)
	center.x = x1;
	addArcVertices( vertices, idx, center, aw, ah, 90, 180, step ); // Bottom-Left arc (6-7)

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINE_LOOP, 0, idx );

	glFlush();
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	int mw = boost::math::iround( (float)rect.w / 2.0f );
	int mh = boost::math::iround( (float)rect.h / 2.0f );
	int aw = (arcW>mw) ? mw : arcW;
	int ah = (arcH>mh) ? mh : arcH;

	int x1 = rect.x + aw;
	int x2 = rect.x + rect.w - aw - 1;
	int y1 = rect.y + ah;
	int y2 = rect.y + rect.h - ah - 1;

	const int step = 6;
	const int nv = (4 * (90 / step)) + 2; // 4 curves of 90 degrees with vertex separated by step degrees plus center and final vertices
	GLfloat vertices[2*nv];
	int idx=0;

	addVertex( vertices, idx, (float(x1+x2))/2.0f+0.5f, (float(y1+y2)/2.0f)+0.5f );    // Middle
	Point center(x1,y1);
	addArcVertices( vertices, idx, center, (float)aw, (float)ah, 180, 270, step, true ); // Top-Left arc (0-1)
	center.x = x2;
	addArcVertices( vertices, idx, center, (float)aw, (float)ah, 270, 360, step, true ); // Top-Right arc (2-3)
	center.y = y2;
	addArcVertices( vertices, idx, center, (float)aw, (float)ah, 0, 90, step, true ); // Bottom-Right arc (4-5)
	center.x = x1;
	addArcVertices( vertices, idx, center, (float)aw, (float)ah, 90, 180, step, true ); // Bottom-Left arc (6-7)
	addVertex( vertices, idx, float(rect.x)+0.125f, float(y1)+0.125f );

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_TRIANGLE_FAN, 0, idx );

	glFlush();
}

/**************************** Polygon functions ******************************/
struct FPoint {
	float x;
	float y;
};

void Surface::fixPoint( GLfloat *vertices, int &idx, const Point &cp, const Point &lp ) const {
	if (cp.x>lp.x) {
		if (cp.y>lp.y) {
			// Down-Right
			addLine( vertices, idx, lp.x, lp.y, cp.x-1, cp.y-1 );
		} else if(cp.y<lp.y) {
			// Up-Right
			addLine( vertices, idx, lp.x, lp.y, cp.x-1, cp.y+1 );
		} else {
			// Horizontal-Right
			addLine( vertices, idx, lp.x, lp.y, cp.x-1, cp.y );
		}
	} else if(cp.x<lp.x) {
		if (cp.y>lp.y) {
			// Down-Left
			addLine( vertices, idx, lp.x, lp.y, cp.x+1, cp.y-1 );
		} else if(cp.y<lp.y) {
			// Up-Left
			addLine( vertices, idx, lp.x, lp.y, cp.x+1, cp.y+1 );
		} else {
			// Horizontal-Left
			addLine( vertices, idx, lp.x, lp.y, cp.x+1, cp.y );
		}
	} else {
		if (cp.y>lp.y) {
			// Vertical-Down
			addLine( vertices, idx, lp.x, lp.y, cp.x, cp.y-1 );
		} else if (cp.y<lp.y) {
			// Vertical-Up
			addLine( vertices, idx, lp.x, lp.y, cp.x, cp.y+1 );
		} else {
			return; // Same point
		}
	}
}

void Surface::drawPolygonImpl( const std::vector<Point>&vertices, bool closed ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	GLfloat glVertices[4 * vertices.size()];
	int idx=0;

	std::vector<Point>::const_iterator it = vertices.begin();
	Point fp = *(it);
	Point lp = fp;
	it++;
	while (it != vertices.end()) {
		Point cp = *(it);  // Current point
		fixPoint( glVertices, idx, cp, lp );
		lp = cp;
		it++;
	}
	if (closed) {
		if (fp!=lp) {
			fixPoint( glVertices, idx, fp, lp );
		}
	}

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, glVertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINES , 0, idx );

	glFlush();
}

// FillPolygon helpers
struct Edge {
	int minY;
	int maxY;
	float xVal;
	float invM;
};

static bool sortEdges( Edge *e1, Edge *e2 ) {
	if (e1->minY<e2->minY) {
		return true;
	} else if (e1->minY==e2->minY) {
		return e1->xVal <= e2->xVal;
	}
	return false;
}

static Edge *calculateEdge( const Point &p1, const Point &p2 ) {
	Edge *edge = new Edge();
	if (p1.y<p2.y) {
		edge->minY = p1.y;
		edge->maxY = p2.y;
		edge->xVal = float(p1.x);
	} else {
		edge->minY = p2.y;
		edge->maxY = p1.y;
		edge->xVal = float(p2.x);
	}
	int x = p1.x - p2.x;
	int y = p1.y - p2.y;
	edge->invM = (y!=0) ? float(x)/float(y) : FLT_MAX;
	if (edge->invM==-0.0f) {
		edge->invM=0.0f;
	}
	return edge;
}

#define MAX_POLYGON_POINTS 1000
void Surface::fillPolygonImpl( const std::vector<Point> &vertices ) {
	// Implementation of http://www.cs.rit.edu/~icss571/filling/
	std::list<Edge*> allEdges;
	std::list<Edge*> globalEdges;
	std::list<Edge*> activeEdges;

	// 1. Initializing All of the Edges
	Point lp = (*vertices.rbegin());
	BOOST_FOREACH( Point p, vertices ) {
		allEdges.push_back( calculateEdge( lp, p ) );
		lp = p;
	}
	allEdges.sort( sortEdges );

	// 2. Initializing the Global Edge Table
	std::list<Edge*>::iterator it = allEdges.begin();
	BOOST_FOREACH( Edge *e, allEdges ) {
		if (e->invM!=FLT_MAX) {
			globalEdges.push_back( e );
		}
	}

	// 3 y 4. Initializing Scan-Line (Parity is implicit)
	int scanLine = (*globalEdges.begin())->minY;

	// 5. Initializing the Active Edge Table
	it = globalEdges.begin();
	while( it != globalEdges.end() ) {
		if ((*it)->minY==scanLine) {
			activeEdges.push_back(*it);
			it = globalEdges.erase( it );
		}
		if ((*it)->minY>scanLine) {
			break;
		}
	}

	setupFramebuffer( _frameBuffer, _size, _clip );

	GLfloat glVertices[2 * MAX_POLYGON_POINTS];
	int idx=0;

	// 6. Filling the polygon
	while (activeEdges.size()>0) {
		// Draw all pixels from the x value of odd to the x value of even parity edge pairs.
		it = activeEdges.begin();
		int last = -1;
		while (it!=activeEdges.end()) {
			int x1 = boost::math::iround((*it)->xVal);
			it++;
			if (it!=activeEdges.end()) {
				int x2 = boost::math::iround((*it)->xVal);
				if (x1>x2) {
					int tmp = x1;
					x1 = x2;
					x2 = tmp;
				}
				if (x1>last) {
					if (idx>=MAX_POLYGON_POINTS) {
						LERROR("gl::Surface", "FillPolygon fail. Exceeded max polygon points");
						globalEdges.clear();
						activeEdges.clear();
						CLEAN_ALL( Edge*, allEdges );
						return;
					}
					addLine( glVertices, idx, x1, scanLine, x2, scanLine );
					last = x2;
				}
				it++;
			}
		}

		// Remove any edges from the active edge table for which the maximum y value is equal to the scan_line.
		it = activeEdges.begin();
		while (it!=activeEdges.end()) {
			if ((*it)->maxY == scanLine) {
				it = activeEdges.erase( it );
			} else {
				it++;
			}
		}

		scanLine++;

		// Update the x value for for each edge in the active edge table using the formula x1 = x0 + 1/m. (This is based on the line formula and the fact that the next scan-line equals the old scan-line plus one.)
		BOOST_FOREACH( Edge *e, activeEdges ) {
			e->xVal = e->xVal + e->invM;
		}

		// Remove any edges from the global edge table for which the minimum y value is equal to the scan-line and place them in the active edge table.
		it = globalEdges.begin();
		while (it!=globalEdges.end()) {
			if ((*it)->minY==scanLine) {
				activeEdges.push_back(*it);
				it = globalEdges.erase( it );
			} else {
				it++;
			}
		}

		// Reorder the edges in the active edge table according to increasing x value. This is done in case edges have crossed.
		activeEdges.sort( sortEdges );
	}

	globalEdges.clear();
	activeEdges.clear();
	CLEAN_ALL( Edge*, allEdges );

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, glVertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINES , 0, idx );

	glFlush();
}

/**************************** Ellipse functions ******************************/
void Surface::addArcVertices( GLfloat *vertices, int &idx, const Point &center, float rw, float rh, int aStart, int aStop, int step, bool fill/*=false*/ ) const {
	const float degToGrad = 3.14159f/180.0f;
	for ( int i=aStart; i<aStop; i+=step ) {
		float rad = degToGrad * (float)i;
		float x = (float) boost::math::iround(float(center.x) + (cos(rad)*rw));
		float y = (float) boost::math::iround(float(center.y) + (sin(rad)*rh));
		if (i<90) {
			x += 0.875f;
			y += fill ? 0.875f : 0.25f;
		} else if (i<180) {
			x += 0.125f;
			y += fill ? 0.875f : 0.25f;
		} else if (i<270) {
			x += 0.125f;
			y += fill ? 0.125f : 0.75f;
		} else {
			x += 0.875f;
			y += fill ? 0.125f : 0.75f;
		}
		addVertex( vertices, idx, x, y );
	}
}

void Surface::drawEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	int nv = boost::math::iround( float(angStop-angStart)/2.0f );
	GLfloat vertices[nv*2];
	int idx=0;

	addArcVertices( vertices, idx, center, float(rw), float(rh), angStart, angStop, 2 );

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_LINE_LOOP, 0, idx );

	glFlush();
}

void Surface::fillEllipseImpl( const canvas::Point &center, int rw, int rh, int angStart, int angStop ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	int nv = boost::math::iround( (float)(angStop-angStart)/2.0f );
	GLfloat vertices[nv*2+2];
	int idx=0;

	addVertex( vertices, idx, float(center.x) + 0.5f, float(center.y) + 0.5f );
	addArcVertices( vertices, idx, center, float(rw), float(rh), angStart, angStop, 2, true );
	addVertex( vertices, idx, float(center.x)+0.875f+float(rw), float(center.y)+0.875f );

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, getColor() );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_TRIANGLE_FAN, 0, idx );

	glFlush();
}

bool Surface::hackDejaVuFont() const {
	return false;
}

/****************************** Text functions ********************************/
canvas::GlyphRun *Surface::createGlyphRun( const std::string &text, const Point &/*pos*/ ) {
	return new GlyphRun(text.length(), _size.h);
}

void Surface::renderText( canvas::GlyphRun *glyphRun, const Point &/*pos*/ ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	GlyphRun *gr = dynamic_cast<GlyphRun*>(glyphRun);

	GLuint prog = _canvas->textShaders();
	glUseProgram( prog );
	setupAttribute( prog, gr->vertices(), "aPos" );
	setupAttribute( prog, gr->texVertices(), "aTexCoord" );
	setupColor( prog, getColor() );
	setupMVPMatrix( prog, _size, false );
	setupTexture( prog );

	glBindTexture( GL_TEXTURE_2D, dynamic_cast<const Font*>(font())->textureId() );
	glDrawElements( GL_TRIANGLES, gr->cant(), GL_UNSIGNED_SHORT, gr->indices() );

	glFlush();
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = ((float)alpha) / 255.0f;
	markDirty();
	return true;
}

util::BYTE Surface::getOpacity() const{
	return (util::BYTE) boost::math::iround( _alpha * 255.0f );
}

float Surface::alpha() const {
	return _alpha;
}

void Surface::setCompositionModeImpl( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA ); break;
		case composition::source: glBlendFunc( GL_ONE, GL_ZERO ); break;
		case composition::clear: glBlendFunc( GL_ZERO, GL_ZERO ); break;
		default: return;
	}
}

void Surface::blitImpl( const Point &targetPoint, canvas::Surface *srcSurface, const Rect &source ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);
	DTV_ASSERT(src);

	Size srcSize = src->getSize();
	GLfloat x1 = float(source.x)/float(srcSize.w);
	GLfloat x2 = float(source.x+source.w)/float(srcSize.w);
	GLfloat y1 = 1.0f - (float(source.y)/float(srcSize.h));
	GLfloat y2 = 1.0f - (float(source.y+source.h)/float(srcSize.h));

	GLfloat texVertices[] = {
		x1, y1, // Top-Left
		x1, y2, // Bottom-Left
		x2, y2, // Bottom-Right
		x2, y1  // Top-Right
	};

	Rect dr( targetPoint.x, targetPoint.y, source.w, source.h );
	setupFramebuffer( _frameBuffer, _size, _clip );

	doBlit( src->texture(), dr, texVertices, _size, src->alpha() );
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw/*=false*/, bool fliph/*=false*/ ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);

	Size srcSize = src->getSize();
	float x1 = float(sourceRect.x)/float(srcSize.w); // Left
	float x2 = float(sourceRect.x+sourceRect.w)/float(srcSize.w); // Right
	float y1 = 1.0f - (float(sourceRect.y)/float(srcSize.h)); // Top
	float y2 = 1.0f - (float(sourceRect.y+sourceRect.h)/float(srcSize.h)); // Bottom

	GLfloat texVertices[] = {
		flipw ? x2 : x1, fliph ? y2 : y1,
		flipw ? x2 : x1, fliph ? y1 : y2,
		flipw ? x1 : x2, fliph ? y1 : y2,
		flipw ? x1 : x2, fliph ? y2 : y1
	};

	setupFramebuffer( _frameBuffer, _size, _clip );

	doBlit( src->texture(), targetRect, texVertices, _size, src->alpha() );
}

void Surface::doBlit( GLuint srcTexture, const Rect &destRect, GLfloat *texVertices, const Size &size, float alpha/*=1.0f*/ ) {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, srcTexture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	GLfloat x2 = (float)(destRect.x + destRect.w);
	GLfloat y2 = (float)(destRect.y + destRect.h);

	GLfloat vertices[] = {
		float(destRect.x), float(destRect.y),
		float(destRect.x),                y2,
		               x2,                y2,
		               x2, float(destRect.y)
	};
	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	GLuint prog = _canvas->blitingShaders();
	glUseProgram( prog );
	setupAttribute( prog, vertices, "aPos" );
	setupAttribute( prog, texVertices, "aTexCoord" );
	setupMVPMatrix( prog, size );
	setupTexture( prog );

	GLint alphaCoordLoc = glGetUniformLocation( prog, "uAlpha" );
	glUniform1f( alphaCoordLoc, alpha );

	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

	glFlush();
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	canvas::Rect rect;
	if (degrees==90 || degrees==270) {
		canvas::Point pos = getLocation();
		rect = canvas::Rect(pos.x,pos.y,_size.h,_size.w);
	} else {
		rect = getBounds();
	}
	Surface *newSurface = dynamic_cast<Surface *>( createSimilar( rect ) );

	GLfloat texVertices[] = {
		/*TexCoord 0*/
		(degrees==0 || degrees==90) ? 0.0f : 1.0f,
		(degrees==0 || degrees==270) ? 1.0f : 0.0f,

		/*TexCoord 1*/
		(degrees==0 || degrees==270) ? 0.0f : 1.0f,
		(degrees==0 || degrees==90) ? 0.0f : 1.0f,

		/*TexCoord 2*/
		(degrees==0 || degrees==90) ? 1.0f : 0.0f,
		(degrees==0 || degrees==270) ? 0.0f : 1.0f,

		/*TexCoord 3*/
		(degrees==0 || degrees==270) ? 1.0f : 0.0f,
		(degrees==0 || degrees==90) ? 1.0f : 0.0f,
	};

	Size size = newSurface->getSize();
	setupFramebuffer( newSurface->frameBuffer(), size, Rect(0,0,size.w,size.h) );
	newSurface->doBlit( _texture, Rect(0,0,size.w,size.h), texVertices, size );

	return newSurface;
}

/*********************** Pixel manipulation functions *************************/
void Surface::getColorFromPixel( util::DWORD pixel, Color &color ) {
	setupFramebuffer( _frameBuffer, _size, _clip );
	GLenum readFormat;
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, (GLint*) &readFormat);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
	color.alpha = (util::BYTE) ((pixel & 0x000000FF) >>  0); // Alpha
	color.g     = (util::BYTE) ((pixel & 0x00FF0000) >> 16); // Green
	if (readFormat==GL_RGBA) {
		color.r     = (util::BYTE) ((pixel & 0x0000FF00) >>  8); // Red
		color.b     = (util::BYTE) ((pixel & 0xFF000000) >> 24); // Blue
	} else {
		color.b     = (util::BYTE) ((pixel & 0x0000FF00) >>  8); // Blue
		color.r     = (util::BYTE) ((pixel & 0xFF000000) >> 24); // Red
	}
#else
	color.alpha = (util::BYTE) ((pixel & 0xFF000000) >> 24); // Alpha
	color.g     = (util::BYTE) ((pixel & 0x0000FF00) >>  8); // Green
	if (readFormat==GL_RGBA) {
		color.r     = (util::BYTE) ((pixel & 0x000000FF) >>  0); // Red
		color.b     = (util::BYTE) ((pixel & 0x00FF0000) >> 16); // Blue
	} else {
		color.b     = (util::BYTE) ((pixel & 0x000000FF) >>  0); // Blue
		color.r     = (util::BYTE) ((pixel & 0x00FF0000) >> 16); // Red
	}
#endif
}

#define THRESHOLD 20
bool Surface::equalsImage( const std::string &file ) {
	bool check = true;
	canvas::Surface *image = canvas()->createSurfaceFromPath( file );
	if (!image) {
		LWARN("gl::Surface", "Fail to create image to comparte: %s", file.c_str());
		return false;
	}

	Size i_size = image->getSize();

	if (_size.w != i_size.w || _size.h != i_size.h) {
		canvas()->destroy( image );
		LWARN("gl::Surface", "Compearing images with different sizes. Img1=(%d,%d), Img2=(%d,%d)", _size.w, _size.h, i_size.w, i_size.h);
		return false;
	} else {
		util::DWORD *data1 = rawPixels();
		util::DWORD *data2 = dynamic_cast<Surface*>(image)->rawPixels();
		for ( int y=i_size.h-1; y>=0; y-- ) {
			util::DWORD *stride1=data1 + y*_size.w;
			util::DWORD *stride2=data2 + y*i_size.w;
			for ( int x=0; x<i_size.w; x++ ) {
				util::DWORD pixelptr1 = stride1[x];
				util::DWORD pixelptr2 = stride2[x];
				if (pixelptr1 != pixelptr2) {
					Color c1,c2;
					getColorFromPixel(pixelptr1, c1);
					getColorFromPixel(pixelptr2, c2);
					if (!c1.equals(c2, THRESHOLD ) ) {
						LINFO("gl::Surface", "Difference found in pixel (%d, %d): s_color=(%d,%d,%d,%d), i_color=(%d,%d,%d,%d).",
							x, i_size.h-y-1, c1.r, c1.g, c1.b, c1.alpha, c2.r, c2.g, c2.b, c2.alpha );
						check=false;
						break;
					}
				}
			}
			if (!check) {
				break;
			}
		}
		free( data1 );
		free( data2 );
	}
	canvas()->destroy( image );
	return check;
}

util::DWORD Surface::getPixel( const Point &pos ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	util::DWORD pixel=0;
	GLenum readType, readFormat;
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, (GLint*) &readType);
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, (GLint*) &readFormat);
	glReadPixels( pos.x, _size.h-pos.y-1, 1, 1, readFormat, readType, &pixel );

	return pixel;
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	util::DWORD pixel = getPixel( pos );
	getColorFromPixel( pixel, color );

	if (color.alpha!=255 && color.alpha!=0) {
		double factor = 255.0/double(color.alpha);
		color.r = (util::BYTE) boost::math::iround(color.r*factor);
		color.g = (util::BYTE) boost::math::iround(color.g*factor);
		color.b = (util::BYTE) boost::math::iround(color.b*factor);
	}
}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	setupFramebuffer( _frameBuffer, _size, _clip );

	GLfloat vertices[] = { float(pos.x)+0.5f, float(pos.y)+0.5f };

	GLuint prog = _canvas->drawingShaders();
	glUseProgram( prog );
	setupColor( prog, color );
	setupAttribute( prog, vertices, "aPos" );
	setupMVPMatrix( prog, _size );

	glDrawArrays( GL_POINTS, 0, 1 );

	glFlush();
}

bool Surface::saveAsImage( const std::string &file ) {
	bool result = true;
	util::DWORD *pxls = pixels();

	FIBITMAP *dib = FreeImage_ConvertFromRawBits( (BYTE*)pxls, _size.w, _size.h, _size.w*4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE );
	if (!FreeImage_Save( FIF_PNG, dib, file.c_str(), PNG_DEFAULT)) {
		LWARN("gl::Surface", "Fail to save surface to image: %s", file.c_str());
		result = false;
	}
	FreeImage_Unload(dib);

	free( pxls );

	return result;
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	GLuint newFB=0;
	glGenFramebuffers( 1, &newFB );
	glBindFramebuffer( GL_FRAMEBUFFER, newFB );

	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTex, 0 );

	GLenum result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		LERROR("gl::Surface", "Unable to resize surface, error=%x", glGetError());
		throw std::runtime_error("[canvas::gl::Surface] Unable to resize surface");
	}
	setupFramebuffer( newFB, size, Rect(0,0,size.w,size.h));
	glClear( GL_COLOR_BUFFER_BIT );

	_size = size;
	_clip.w = size.w;
	_clip.h = size.h;

	if (scaleContent) {
		GLfloat texVertices[] = {
			0.0f, 1.0f, // Top-Left
			0.0f, 0.0f, // Bottom-Left
			1.0f, 0.0f, // Bottom-Right
			1.0f, 1.0f  // Top-Right
		};
		doBlit( _texture, Rect(0,0,_size.w,_size.h), texVertices, _size );
	}

	glDeleteFramebuffers( 1, &_frameBuffer );
	glDeleteTextures( 1, &_texture );
	_frameBuffer = newFB;
	_texture = newTex;

	glFlush();
}

util::DWORD *Surface::rawPixels() {
	size_t bytes = _size.w * _size.h * sizeof(util::DWORD);
	util::DWORD *pixels = (util::DWORD*) malloc( bytes );

	Rect clip( 0, 0, _size.w, _size.h );
	setupFramebuffer( _frameBuffer, _size, clip );
	GLenum readType, readFormat;
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, (GLint*) &readType);
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, (GLint*) &readFormat);
	glReadPixels( 0, 0, _size.w, _size.h, readFormat, readType, pixels );

	return pixels;
}

util::DWORD *Surface::pixels() {
	size_t bytes = _size.w * _size.h * sizeof(util::DWORD);
	util::DWORD *tmp = (util::DWORD*) malloc( bytes );
	util::DWORD *pixels = (util::DWORD*) malloc( bytes );

	Rect clip( 0, 0, _size.w, _size.h );
	setupFramebuffer( _frameBuffer, _size, clip );
	GLenum readType, readFormat;
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, (GLint*) &readType);
	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, (GLint*) &readFormat);
	glReadPixels( 0, 0, _size.w, _size.h, readFormat, readType, tmp );

	int p=0;
	for (int i=_size.h-1; i>=0; i--) {
		util::DWORD *stride = tmp + (_size.w * i);
		for (int j=0; j<_size.w; j++) {
			util::DWORD pixel=0;
			util::DWORD tmpPxl=stride[j];
			int alpha = (tmpPxl & 0xFF000000) >> 24;
			float factor = alpha ? 255.0f / float(alpha) : 255.0f;
			pixel |= alpha << FI_RGBA_ALPHA_SHIFT;
			pixel |= boost::math::iround( (float)((tmpPxl & 0x0000FF00) >>  8) * factor ) << FI_RGBA_GREEN_SHIFT;
			if (readFormat == GL_RGBA) {
				pixel |= boost::math::iround( (float)((tmpPxl & 0x00FF0000) >> 16) * factor ) << FI_RGBA_BLUE_SHIFT;
				pixel |= boost::math::iround( (float)((tmpPxl & 0x000000FF) >>  0) * factor ) << FI_RGBA_RED_SHIFT;
			} else { // Format is BGR
				pixel |= boost::math::iround( (float)((tmpPxl & 0x00FF0000) >> 16) * factor ) << FI_RGBA_RED_SHIFT;
				pixel |= boost::math::iround( (float)((tmpPxl & 0x000000FF) >>  0) * factor ) << FI_RGBA_BLUE_SHIFT;
			}
			pixels[p++] = pixel;
		}
	}
	free( tmp );

	return pixels;
}

void Surface::addVertex( GLfloat *vertices, int &idx, GLfloat x, GLfloat y ) const {
	vertices[idx*2] = x;
	vertices[idx*2+1] = y;
	idx++;
}

void Surface::setupFramebuffer( GLuint fb, const Size &size, const Rect &clip ) {
	glBindFramebuffer( GL_FRAMEBUFFER, fb );
	glViewport( 0, 0, size.w, size.h );
	glScissor( clip.x, size.h-clip.y-clip.h, clip.w, clip.h );
	setCompositionModeImpl( getCompositionMode() );
}

void Surface::setupColor( GLuint program, const Color &color ) const {
	GLint uColor = glGetUniformLocation( program, "uColor" );
	float alpha = (color.alpha) ? (float)color.alpha/255.0f : 0.0039f;
	glUniform4f( uColor, color.r/255.0f*alpha, color.g/255.0f*alpha, color.b/255.0f*alpha, alpha );
}

void Surface::setupMVPMatrix( GLuint program, const Size &size, bool reflectY/*=true*/ ) const {
	GLint uMVP = glGetUniformLocation( program, "uMVP" );
	GLfloat sx = 2.0f/float(size.w);
	GLfloat sy = 2.0f/float(size.h);
	GLfloat mvMatrix[4][4] = {
		{   sx,  0.0f, 0.0f, 0.0f},
		{ 0.0f,    sy, 0.0f, 0.0f},
		{ 0.0f,  0.0f, 1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f, 1.0f}
	};
	if (reflectY) {
		mvMatrix[1][1] = -mvMatrix[1][1];
		mvMatrix[3][1] = -mvMatrix[3][1];
	}
	glUniformMatrix4fv( uMVP, 1, GL_FALSE, (GLfloat*)mvMatrix );
}

void Surface::setupAttribute( GLuint program, GLvoid *vertices, const std::string &name ) const {
	GLint location = glGetAttribLocation( program, name.c_str() );
	glVertexAttribPointer( location, 2, GL_FLOAT, GL_FALSE, 0, vertices );
	glEnableVertexAttribArray( location );
}

void Surface::setupTexture( GLuint program ) const {
	GLint location = glGetUniformLocation( program, "uTex" );
	glUniform1i( location, 0 );
}

GLuint Surface::texture() const {
	return _texture;
}

GLuint Surface::frameBuffer() const {
	return _frameBuffer;
}

void Surface::renderToFB( GLuint fb, const Rect &bounds, const Size &winSize, bool clearLayer/*=false*/ ) {
	setupFramebuffer( fb, winSize, bounds );
	if (!fb && clearLayer) {	//	Clear main layer
		glClear( GL_COLOR_BUFFER_BIT );
	}
	GLfloat texVertices[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };
	doBlit( _texture, bounds, texVertices, winSize );
}

}
}
