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
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <vdpau/vdpau_x11.h>
#include <X11/Xlib.h>

namespace canvas {
namespace x11 {
namespace vdpau {

Render::Render( Window *window )
	: canvas::x11::Render( window )
{
	_target = VDP_INVALID_HANDLE;
	_queue = VDP_INVALID_HANDLE;
	_layerSurface = VDP_INVALID_HANDLE;
	_screenSurface = VDP_INVALID_HANDLE;	
	_videoSurface = VDP_INVALID_HANDLE;
	_mixer = VDP_INVALID_HANDLE;
	_surface = NULL;
	_layer = NULL;
	_stride = 0;
}

Render::~Render()
{
}

//	Aux initialization
bool Render::init() {
	LDEBUG( "vdpau", "Initialize" );
	
	if (!_wrapper.open( display(), DefaultScreen(display()) )) {
		LERROR( "vdpau", "cannot open VDPAU device" );		
		return false;
	}

	if (!_wrapper.pqtCreate( win(), &_target )) {
		LERROR( "vdpau", "cannot create target" );
		return false;
	}

	if (!_wrapper.pqCreate( _target, &_queue )) {
		LERROR( "vdpau", "cannot create queue" );
		return false;
	}

	return true;
}

void Render::fin() {
	LDEBUG( "vdpau", "Finalize" );
	
	_wrapper.pqDestroy( _queue );
	_wrapper.pqtDestroy( _target );
	_wrapper.close();
}

//	Aux render initialization
bool Render::initRender() {
	LDEBUG( "vdpau", "Initialize render" );
	
	const Size &s = size();

	if (!_wrapper.osCreate( VDP_RGBA_FORMAT_B8G8R8A8, s.w, s.h, &_screenSurface )) {
		LERROR( "vdpau", "cannot create screen surface" );
		return false;
	}		

	VdpChromaType chroma=VDP_CHROMA_TYPE_422;
	{	//	Create video surface
		uint32_t w, h;
		if (!_wrapper.vsQueryCapabilities( chroma, &w, &h )) {
			LERROR( "vdpau", "Cannot get chroma type for video surface" );
			return false;
		}
			
		if (!_wrapper.vsCreate( chroma, s.w, s.h, &_videoSurface )) {
			LERROR( "vdpau", "Cannot create video surface" );
			return false;
		}
	}		

	{	//	Create video mixer
		VdpVideoMixerParameter params[] = {
			VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_WIDTH,
			VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_HEIGHT,
			VDP_VIDEO_MIXER_PARAMETER_CHROMA_TYPE,
			VDP_VIDEO_MIXER_PARAMETER_LAYERS
		};
		int numLayers = 1;
		void const *paramValues[] = {
			&s.w,
			&s.h,
			&chroma,
			&numLayers
		};

		if (!_wrapper.vmCreate( 0, NULL, 4, params, paramValues, &_mixer )) {
			LERROR( "vdpau", "cannot create video mixer" );
			return false;
		}
	}

	return x11::Render::initRender();
}

void Render::finRender() {
	LDEBUG( "vdpau", "Finalize render" );
	
	_wrapper.vmDestroy( _mixer );
	_wrapper.vsDestroy( _videoSurface );
	_wrapper.osDestroy( _screenSurface );

	x11::Render::finRender();
}

//	Aux layer
bool Render::initLayer( Canvas *canvas ) {
	LDEBUG( "vdpau", "Initialize layer" );
	
	_canvasSize = canvas->size();

	if (!_wrapper.osCreate( VDP_RGBA_FORMAT_B8G8R8A8, _canvasSize.w, _canvasSize.h, &_layerSurface )) {
		LERROR( "vdpau", "cannot create layer surface" );
		return false;
	}

	//	Setup data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = canvas->size();
	img.length = _canvasSize.w*_canvasSize.h*4;
	img.data = _layer = (unsigned char *)malloc( img.length );
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = _stride = img.size.w*4;
	img.dataOffset = 0;

	_surface = canvas->createSurface( &img );
	if (!_surface) {
		LERROR("vdpau", "Cannot create a canvas surface");
		return false;
	}
	
	return true;
}

void Render::finLayer( Canvas * /*canvas*/ ) {
	LDEBUG( "vdpau", "Finalize layer" );
	
	_wrapper.osDestroy( _layerSurface );
	DEL(_surface);
	free( _layer ); _layer = NULL;
}

Surface *Render::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Render::blit( Surface * /*surface*/, const std::vector<Rect> & /*dirtyRegions*/, Pixmap /*dest*/ ) {
	uint32_t w = _canvasSize.w;
	uint32_t h = _canvasSize.h;
	VdpRect dstRect = { 0, 0, w, h };
	uint32_t pitches = _stride;
	if (!_wrapper.osPutBitsNative( _layerSurface, (void **)&_layer, &pitches, &dstRect )) {
		LWARN( "vdpau", "cannot put bits native" );
		return;
	}

	queue();
}

//	Video methods
int Render::getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines ) {
	strcpy( chroma, "YUYV" );
	const Size &s = size();
	*width = s.w;
	*height = s.h;
	pitches[0] = 8*((s.w + 3) / 4);
	pitches[1] = pitches[2] = 0;
	lines[0] = s.h;
	lines[1] = lines[2] = 0;
	return 1;
}

void *Render::allocFrame( void **pixels ) {
	util::BYTE *frame = NULL;
	const Size &s = size();	
	frame = (util::BYTE *)malloc( s.w*s.h*4 );
	*pixels = frame;
	return frame;
}

void Render::freeFrame( void *frame ) {
	free(frame);
}

void Render::renderFrame( void *frame ) {
	//	Put data into video surface
	const Size &s = size();
	uint32_t calc = 8*((s.w + 3) / 4);
	uint32_t pitches[] = { calc };
	void* data[] = { frame };
	bool res=_wrapper.vsPutBitsYCbCr(
		_videoSurface,
		VDP_YCBCR_FORMAT_YUYV,
		( const void * const *)&data,
		pitches
	);
	if (!res) {
		LERROR( "vdpau", "Cannot put bits into video surface" );
		return;
	}

	queue();
}

void Render::queue() {
	VdpLayer layer = {
		VDP_LAYER_VERSION,
		_layerSurface,
		NULL,
		NULL
	};

	int layerCount = (_layerSurface == VDP_INVALID_HANDLE) ? 0 : 1;
	
	VdpRect out;
	{        //        Copy video output
		const Rect &tmp = videoBounds();
		out.x0 = tmp.x;
		out.y0 = tmp.y;
		out.x1 = tmp.x+tmp.w;
		out.y1 = tmp.y+tmp.h;
	}
	//	Merge video surface with layer surface
	bool res = _wrapper.vmRender(
		_mixer,
		VDP_INVALID_HANDLE,
		NULL,
		VDP_VIDEO_MIXER_PICTURE_STRUCTURE_TOP_FIELD,
		0,
		NULL,
		_videoSurface,
		0,
		NULL,
		NULL,
		_screenSurface,
		NULL,
		&out,
		layerCount,
		&layer
		 );
	if (!res) {
		LERROR( "vdpau", "Cannot render video surface into canvas layer" );
		return;
	}
	
	if (!_wrapper.pqDisplay(
		_queue,
		_screenSurface,
		0,
		0,
		0 ))
	{
		LWARN( "vdpau", "cannot display layer" );
	}
}

}
}
}

