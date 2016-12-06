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

#include "render.h"
#include "../../../canvas.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <va/va.h>
#include <va/va_x11.h>
#include <X11/Xlib.h>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <stdio.h>

#define MAX_SURFACES 16

namespace canvas {
namespace x11 {
namespace vaapi {

struct _VideoSurface {
	VASurfaceID surface;
	VAImage image;
};

Render::Render( Window *win )
	: canvas::x11::Render( win )
{
	_layer = NULL;
	_layerImage = NULL;
	_layerSubpicture = VA_INVALID_ID;
	_surface = NULL;
	_blackSurface = NULL;
	_lastShow = NULL;
	_vaDisplay = NULL;
}

Render::~Render()
{
	DTV_ASSERT(!_vaDisplay);
}

//	Aux initialization
bool Render::init() {
	LDEBUG( "vaapi", "Initialize" );
	
	//	Get VA-API display
	_vaDisplay = vaGetDisplay( display() );
	if (!_vaDisplay) {
		LERROR( "vaapi", "Cannot get VA-API display" );
		return false;
	}

	{	//	Initialize
		int major_ver, minor_ver;
		VAStatus st = vaInitialize( _vaDisplay, &major_ver, &minor_ver);
		if (st != VA_STATUS_SUCCESS) {
			LERROR( "vaapi", "Cannot initialize va-api: st=%d", st );
			return false;
		}
	}

	{	//	Set background color
		VADisplayAttribute attr;
		memset( &attr, 0, sizeof(attr) );
		attr.type  = VADisplayAttribBackgroundColor;
		attr.value = 0x000000;
		VAStatus st = vaSetDisplayAttributes( _vaDisplay, &attr, 1 );
		if (st != VA_STATUS_SUCCESS) {
			LWARN( "vaapi", "Cannot set background color: st=%d", st );
		}
	}

	return true;
}

void Render::fin() {
	LDEBUG( "vaapi", "Finalize" );
	
	if (_vaDisplay) {
		vaTerminate( _vaDisplay );
		_vaDisplay = NULL;
	}
}

//	Layer methods
bool Render::initLayer( Canvas *canvas ) {
	LDEBUG( "vaapi", "Initialize layer" );
	
	//	Try alloc layer
	const Size &s = canvas->size();
	if (!createLayer( s )) {
		return false;
	}

	//	Setup image data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = s;
	img.length = s.w*s.h*4;
	img.data = _layer = (unsigned char *)malloc( img.length );
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = img.size.w*4;
	img.dataOffset = 0;

	_surface = canvas->createSurface( &img );
	if (!_surface) {
		LERROR("vaapi", "Cannot create a canvas surface");
		return false;
	}

	return true;
}

void Render::finLayer( Canvas * /*canvas*/ ) {
	LDEBUG( "vaapi", "Finalize layer" );
	
	DEL(_surface);
	free( _layer );
	
	if (_layerSubpicture != VA_INVALID_ID) {
		vaDestroySubpicture( _vaDisplay, _layerSubpicture );
	}
	if (_layerImage) {
		vaDestroyImage( _vaDisplay, _layerImage->image_id );
		free(_layerImage);
	}
}

Surface *Render::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Render::blit( Surface * /*surface*/, const std::vector<Rect> & /*dirtyRegions*/, Pixmap /*dest*/ ) {
	VAStatus st;
	util::BYTE *ptr;

	//	Map layer image into memory
	st = vaMapBuffer( _vaDisplay, _layerImage->buf, (void **)&ptr );
	if (st != VA_STATUS_SUCCESS) {
		LWARN( "vaapi", "Cannot map layer image: st=%d", st );
		return;
	}

	//	Copy layer data to layer image 
	int lWidth = _layerImage->width;
	int lHeight = _layerImage->height;
	int layerSize = lWidth * lHeight * 4;

	//	Copy from layer to image layer
	memcpy( ptr, _layer, layerSize );

	//	Unmap image layer
	st = vaUnmapBuffer( _vaDisplay, _layerImage->buf );
	if (st != VA_STATUS_SUCCESS) {
		LWARN( "vaapi", "Cannot unmap layer image: st=%d", st );
	}

	//	Force show the last frame or the blackSurface
	show( NULL );
}

//	Video methods
bool Render::initRender() {
	LDEBUG( "vaapi", "Initialize render" );

	//	Get window size
	const Size &s = size();

	boost::interprocess::scoped_lock<boost::mutex> lock( _mutex );

	//	Create screen surface
	if (!createBlackSurface( s )) {
		return false;
	}
	_lastShow = _blackSurface;

	{	//	Allocate video surfaces
		VideoSurface *sur;
		for (int i=0; i<MAX_SURFACES; i++) {
			sur = createSurface( s );
			if (!sur) {
				return false;
			}
			_surfaces.push_back( sur );
		}
	}

	LDEBUG( "vaapi", "Init render: size=(%d,%d), surfaces=%d",
		s.w, s.h, _surfaces.size() );

	return x11::Render::initRender();
}

void Render::finRender() {
	LDEBUG( "vaapi", "Finalize render" );

	_mutex.lock();

	BOOST_FOREACH( VideoSurface *frame, _surfaces ) {
		destroySurface( frame );
	}
	_surfaces.clear();	
	
	if (_blackSurface) {
		destroySurface( _blackSurface );
		_blackSurface = NULL;		
	}
	_lastShow = NULL;

	_mutex.unlock();

	x11::Render::finRender();
}

int Render::getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines ) {
	DTV_ASSERT( _blackSurface );

	VideoSurface *sur = _blackSurface;

	//	Get window resolution
	const Size &s = size();

	*width = s.w;
	*height = s.h;
	pitches[0] = sur->image.pitches[0];
	pitches[1] = sur->image.pitches[1];
	pitches[2] = sur->image.pitches[2];
	lines  [0] = lines  [1] = lines  [2] = s.h;
	strcpy( chroma, _blackSurfaceChroma );

	return MAX_SURFACES;
}

void *Render::allocFrame( void **pixels ) {
	VideoSurface *frame = NULL;

	_mutex.lock();
	if (!_surfaces.empty()) {
		frame = _surfaces.back();
		_surfaces.pop_back();
	}
	_mutex.unlock();	

	if (frame) {
		//	Map image surface to user address
		uint8_t *planes;
		VAStatus st = vaMapBuffer( _vaDisplay, frame->image.buf, (void **)&planes );
		if (st != VA_STATUS_SUCCESS) {
			LERROR( "vaapi", "Cannot map image surface: st=%d", st );
			destroySurface( frame );
			frame = NULL;
		}
		else {
			for (unsigned int i=0; i<frame->image.num_planes; i++) {
				pixels[i] = planes + frame->image.offsets[i];
			}
		}
	}

	return frame;
}

void Render::freeFrame( void *frame ) {
	VideoSurface *sur = (VideoSurface *)frame;

	//	Unmap buffer
	VAStatus st = vaUnmapBuffer( _vaDisplay, sur->image.buf );
	DTV_ASSERT( st == VA_STATUS_SUCCESS );
	UNUSED(st);

	show( sur );
}

void Render::renderFrame( void * /*id*/ ) {
}

void Render::cleanup() {
	show( _blackSurface );
}

//	Aux
void Render::show( VideoSurface *s ) {
	VAStatus st;

	_mutex.lock();
	if (!s) {
		//	Show last frame, if no current frame
		DTV_ASSERT(_lastShow);
		s = _lastShow;
	}
	else {
		//	A new frame received, free last
		if (_lastShow != _blackSurface) {
			_surfaces.push_back( _lastShow );
		}
		_lastShow = s;		
	}
	_mutex.unlock();
	
	int sWidth = s->image.width;
	int sHeight = s->image.height;
		
	int lWidth = _layerImage->width;
	int lHeight = _layerImage->height;

	const Rect &out = videoBounds();

	{	//	Associate OSD to screen surface
		unsigned int flags = (out.w != sWidth || out.h != sHeight) ? VA_SUBPICTURE_DESTINATION_IS_SCREEN_COORD : 0;

		st = vaAssociateSubpicture(
			_vaDisplay,
			_layerSubpicture,
			&s->surface,
			1,
			0, 0, (unsigned short)lWidth, (unsigned short)lHeight,
			0, 0, (unsigned short)sWidth, (unsigned short)sHeight,
			flags
		);
		if (st != VA_STATUS_SUCCESS) {
			LWARN( "vaapi", "Cannot associate layer to frame: st=%d", st );
		}
	}
	
	//	Display screen surface
	st = vaPutSurface(
		_vaDisplay,
		s->surface,
		win(),
		0, 0, (unsigned short)sWidth, (unsigned short)sHeight,
		(short)out.x, (short)out.y, (unsigned short)out.w, (unsigned short)out.h,
		NULL, 0,
		0
	);
	if (st != VA_STATUS_SUCCESS) {
		LWARN( "vaapi", "Cannot render frame: st=%d", st );
	}
}

bool Render::createBlackSurface( const Size &s ) {
	_blackSurface = createSurface( s );
	if (!_blackSurface) {
		return false;
	}

	unsigned int fourcc = _blackSurface->image.format.fourcc;

	//	Sets the surface to black
	paintSurface( _blackSurface, 0, 128, 128 );

	switch (fourcc) {
		case VA_FOURCC_NV12:
			strcpy( _blackSurfaceChroma, "NV12" );
			break;
		case VA_FOURCC_AI44:
			strcpy( _blackSurfaceChroma, "AI44" );
			break;
		case VA_FOURCC_RGBA:
			strcpy( _blackSurfaceChroma, "RGBA" );
			break;
		case VA_FOURCC_BGRA:
			strcpy( _blackSurfaceChroma, "BGRA" );
			break;
		case VA_FOURCC_UYVY:
			strcpy( _blackSurfaceChroma, "UYVY" );
			break;
		case VA_FOURCC_YUY2:
			strcpy( _blackSurfaceChroma, "YUY2" );
			break;
		case VA_FOURCC_AYUV:
			strcpy( _blackSurfaceChroma, "AYUV" );
			break;
		case VA_FOURCC_NV11:
			strcpy( _blackSurfaceChroma, "NV11" );
			break;
		case VA_FOURCC_YV12:
			strcpy( _blackSurfaceChroma, "YV12" );
			break;
		case VA_FOURCC_P208:
			strcpy( _blackSurfaceChroma, "P208" );
			break;
		case VA_FOURCC_IYUV:
			strcpy( _blackSurfaceChroma, "IYUV" );
			break;
		default: {
			LERROR( "vaapi", "Invalid fourcc from image" );
			destroySurface( _blackSurface );
			_blackSurface = NULL;
			return false;
		}
	};

	return true;
}

void Render::paintSurface ( VideoSurface *s, unsigned char plane0, unsigned char plane1, unsigned char plane2 ) {
	VAStatus vaStatus;
	void *p_base = NULL;

	vaStatus = vaMapBuffer( _vaDisplay, s->image.buf, (void **)&p_base );
	if (vaStatus != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot map image surface: st=%d", vaStatus );
	}

	memset((uint8_t*)p_base + s->image.offsets[0], plane0, s->image.pitches[0] * s->image.height);
	memset((uint8_t*)p_base + s->image.offsets[1], plane1, s->image.pitches[1] * (s->image.height/2));
	memset((uint8_t*)p_base + s->image.offsets[2], plane2, s->image.pitches[2] * (s->image.height/2));

	vaStatus = vaUnmapBuffer( _vaDisplay, s->image.buf );
	if (vaStatus != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot unmap image surface: st=%d", vaStatus );
	}
}

VideoSurface *Render::createSurface( const Size &s ) {
	//	Alloc a new video surface
	VideoSurface *frame = new VideoSurface();

	//	Create surface
#if VA_MINOR_VERSION > 32
	VAStatus st = vaCreateSurfaces(
		_vaDisplay,
		VA_RT_FORMAT_YUV420,
		s.w, s.h,
		&frame->surface, 1,
		NULL, 0
	);
#else
	VAStatus st = vaCreateSurfaces(
		_vaDisplay,
		s.w, s.h,
		VA_RT_FORMAT_YUV420,
		1,
		&frame->surface
	);
#endif

	if (st != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot create video surface: st=%d", st );
		delete frame;
		return NULL;
	}

	//	Create image from surface
	st = vaDeriveImage(
		_vaDisplay,
		frame->surface,
		&frame->image
	);
	if (st != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot derive video surface: st=%d", st );
		vaDestroySurfaces( _vaDisplay, &frame->surface, 1 );
		delete frame;
		return NULL;
	}

	return frame;
}

void Render::destroySurface( VideoSurface *frame ) {
	vaSyncSurface( _vaDisplay, frame->surface );
	vaDestroyImage( _vaDisplay, frame->image.image_id );
	vaDestroySurfaces( _vaDisplay, &frame->surface, 1 );
	delete frame;
}

bool Render::createLayer( const Size &s ) {
	//	Get list of formats
	VAImageFormat *formats;
	unsigned int maxFormats = vaMaxNumSubpictureFormats( _vaDisplay );
	if (!maxFormats) {
		LERROR( "vaapi", "Cannot get maximum subpicture formats" );
		return false;
	}

	//	Alloc memory to get all subpicture formats
	formats = (VAImageFormat *)calloc( maxFormats, sizeof(*formats) );
	DTV_ASSERT(formats);

	//	Alloc memory get get all subpicture flags
	unsigned int *flags = (unsigned int *)calloc( maxFormats, sizeof(*flags));
	DTV_ASSERT(flags);

	//	Get all formats supported
	unsigned int numFormats=0;
	VAStatus st = vaQuerySubpictureFormats(
		_vaDisplay,
		formats,
		flags,
		&numFormats
	);
	if (st != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot get subpicture formats" );
		free(formats);
		free(flags);
		return false;
	}

	if (!(*flags & VA_SUBPICTURE_DESTINATION_IS_SCREEN_COORD)) {
		LERROR( "vaapi", "Subpicture not support screen coordinates" );
		free(formats);
		free(flags);
		return false;
	}		

	//	Find RGBA format
	unsigned int i;
	for (i=0; i<numFormats; i++) {
		LDEBUG( "vaapi", "Subpicture format: format=%x, bpp=%d",
			formats[i].fourcc, formats[i].bits_per_pixel );

		if (formats[i].fourcc == VA_FOURCC_BGRA) {
			break;
		}
	}
	//	Format not found?
	if (i >= maxFormats) {
		LERROR( "vaapi", "Cannot find RGBA format for subpicture" );
		free(formats);
		free(flags);
		return false;
	}

	//	Alloc memory for layer image
	_layerImage = (VAImage *)calloc( 1, sizeof(*_layerImage) );
	DTV_ASSERT(_layerImage);

	//	Create layer image
	st = vaCreateImage(
		_vaDisplay,
		&formats[i],
		s.w, s.h,
		_layerImage
	);
	if (st != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot create layer image" );
		free(_layerImage); _layerImage=NULL;
		free(formats);
		free(flags);
		return false;
	}

	//	Free formats
	free(formats);
	free(flags);

	//	Create layer subpicture
	st = vaCreateSubpicture(
		_vaDisplay,
		_layerImage->image_id,
		&_layerSubpicture );
	if (st != VA_STATUS_SUCCESS) {
		LERROR( "vaapi", "Cannot create layer subpicture" );
		vaDestroyImage( _vaDisplay, _layerImage->image_id );
		free(_layerImage); _layerImage=NULL;
		return false;
	}

	return true;
}

}
}
}
