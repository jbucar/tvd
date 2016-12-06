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

#include <vdpau/vdpau.h>
#include <vdpau/vdpau_x11.h>
#include <string>

namespace canvas {
namespace x11 {
namespace vdpau {

typedef struct {
	uint32_t max_level;
	uint32_t max_macroblocks;
	uint32_t max_width;
	uint32_t max_height;
} DecoderCapabilities;

class Wrapper {
public:
	Wrapper();
	~Wrapper();

	bool isOpen() const;
	bool open( Display *display, int screen );
	void close();

	bool getApiVersion( unsigned int &version );
	bool getInformation( std::string &information );

	//	Video mixer
	bool vmQueryFeature( VdpVideoMixerFeature feature );
	bool vmCreate(
		uint32_t featureCount,
		VdpVideoMixerFeature const *features,
		uint32_t parameterCount,
		VdpVideoMixerParameter const *parameters,
		void const * const *parameterValues,
		VdpVideoMixer *mixer
	);
	bool vmDestroy( VdpVideoMixer mixer );
	bool vmRender(
		VdpVideoMixer mixer,
		VdpOutputSurface backgroundSurface,
		VdpRect const *backgroundSrcRect,
		VdpVideoMixerPictureStructure currentPictureStructure,
		uint32_t videoSurfacePastCount,
		VdpVideoSurface const *videoSurfacePast,
		VdpVideoSurface videoSurfaceCurrent,
		uint32_t videoSurfaceFutureCount,
		VdpVideoSurface const *videoSurfaceFuture,
		VdpRect const *videoSourceRect,
		VdpOutputSurface dstSurface,
		VdpRect const *dstRect,
		VdpRect const *dstVideoRect,
		uint32_t layerCount,
		VdpLayer const *layers
	);

	//	Decoder
	bool dQueryCapabilities( VdpDecoderProfile p, DecoderCapabilities &capabilities );

	//	Presentation
	bool pqtCreate( Drawable drawable, VdpPresentationQueueTarget *target );
	bool pqtDestroy( VdpPresentationQueueTarget target );	
	bool pqCreate( VdpPresentationQueueTarget target, VdpPresentationQueue *queue );
	bool pqDestroy( VdpPresentationQueue queue );	
	bool pqDisplay(
		VdpPresentationQueue queue,
		VdpOutputSurface     surface,
		uint32_t             clip_width,
		uint32_t             clip_height,
		VdpTime              earliest_presentation_time );

	//	Video surface
	bool vsQueryCapabilities(
		VdpChromaType format,
		uint32_t *max_width,
		uint32_t *max_height
	);
	bool vsCreate(
		VdpChromaType type,
		uint32_t width,
		uint32_t height,
		VdpVideoSurface *surface
	);
	bool vsDestroy( VdpVideoSurface surface );

	bool vsPutBitsYCbCr(
		VdpVideoSurface surface,
		VdpYCbCrFormat format,
		void const * const * data,
		uint32_t const * pitches
	);

	//	Output surface
	bool osQueryCapabilities(
		VdpRGBAFormat format,
		uint32_t *max_width,
		uint32_t *max_height );

	bool osCreate(
		VdpRGBAFormat format,
		uint32_t width,
		uint32_t height,
		VdpOutputSurface *surface );

	bool osDestroy( VdpOutputSurface surface );

	bool osPutBitsNative(
		VdpOutputSurface surface,
		void const *const *source_data,
		uint32_t const *source_pitches,
		VdpRect const *destination_rect );

	bool osRenderBitmapSurface(
		VdpOutputSurface dst,
		VdpRect const *dstRect,
		VdpBitmapSurface src,
		VdpRect const *srcRect,
		VdpColor const *colors,
		VdpOutputSurfaceRenderBlendState const *blendState,
		uint32_t flags );

	//	Bitmap
	bool bsQueryCapabilities(
		VdpRGBAFormat format,
		uint32_t *maxWidth,
		uint32_t *maxHeight );
	
	bool bsCreate(
		VdpRGBAFormat format,
		uint32_t width,
		uint32_t height,
		VdpBool frequently_accessed,
		VdpBitmapSurface *surface );

	bool bsDestroy( VdpBitmapSurface surface );
	bool bsPutBitsNative(
		VdpBitmapSurface surface,
		void const *const *data,
		uint32_t const *pitches,
		VdpRect const *dstRect );

protected:
	bool loadMethod( VdpFuncId id, void **ptr );
	void showInfo();	

private:
	VdpDevice _device;
	VdpGetProcAddress *_getProcAddress;
	VdpDeviceDestroy *_procDEVICE_DESTROY;
	VdpGetErrorString *_procGET_ERROR_STRING;
	VdpGetApiVersion *_procGET_API_VERSION;
	VdpGetInformationString *_procGET_INFORMATION_STRING;

	//	Bitmap
	VdpBitmapSurfaceQueryCapabilities *_procBITMAP_SURFACE_QUERY_CAPABILITIES;
	VdpBitmapSurfaceCreate *_procBITMAP_SURFACE_CREATE;
	VdpBitmapSurfaceDestroy *_procBITMAP_SURFACE_DESTROY;
	VdpBitmapSurfacePutBitsNative *_procBITMAP_SURFACE_PUT_BITS_NATIVE;

	//	Presentation
	// VdpPresentationQueueTarget _vdp_flip_target;
	// VdpPresentationQueue _vdp_flip_queue;
	// VdpPresentationQueueBlockUntilSurfaceIdle *_presentation_queue_block_until_surface_idle;
	// VdpPresentationQueueQuerySurfaceStatus *_presentation_queue_query_surface_status;
	// VdpPresentationQueueGetTime *_presentation_queue_get_time;
	VdpPresentationQueueTargetCreateX11 *_procPRESENTATION_QUEUE_TARGET_CREATE_X11;
	VdpPresentationQueueTargetDestroy *_procPRESENTATION_QUEUE_TARGET_DESTROY;
	VdpPresentationQueueCreate *_procPRESENTATION_QUEUE_CREATE;
	VdpPresentationQueueDestroy *_procPRESENTATION_QUEUE_DESTROY;
	VdpPresentationQueueDisplay *_procPRESENTATION_QUEUE_DISPLAY;

	//	Video surface
	VdpVideoSurfaceCreate *_procVIDEO_SURFACE_CREATE;
	VdpVideoSurfaceDestroy *_procVIDEO_SURFACE_DESTROY;
	VdpVideoSurfacePutBitsYCbCr *_procVIDEO_SURFACE_PUT_BITS_Y_CB_CR;
	VdpVideoSurfaceQueryCapabilities *_procVIDEO_SURFACE_QUERY_CAPABILITIES;
	// VdpVideoSurfaceCreate *_video_surface_create;
	// VdpVideoSurfaceDestroy *_video_surface_destroy;
	// VdpVideoSurfacePutBitsYCbCr *_video_surface_put_bits_y_cb_cr;
	// VdpVideoSurfaceGetBitsYCbCr *_video_surface_get_bits_y_cb_cr;

	//	Output surface
	VdpOutputSurfaceQueryCapabilities *_procOUTPUT_SURFACE_QUERY_CAPABILITIES;
	VdpOutputSurfaceCreate *_procOUTPUT_SURFACE_CREATE;
	VdpOutputSurfaceDestroy *_procOUTPUT_SURFACE_DESTROY;
	VdpOutputSurfacePutBitsNative *_procOUTPUT_SURFACE_PUT_BITS_NATIVE;
	
	VdpOutputSurfaceRenderOutputSurface *_procOUTPUT_SURFACE_RENDER_OUTPUT_SURFACE;
	VdpOutputSurfaceRenderBitmapSurface *_procOUTPUT_SURFACE_RENDER_BITMAP_SURFACE;

	// VdpOutputSurfacePutBitsYCbCr *_output_surface_put_bits_y_cb_cr;
	// VdpOutputSurfaceGetBitsNative *_output_surface_get_bits_native;
	// VdpOutputSurfacePutBitsIndexed *_output_surface_put_bits_indexed;

	//	Video mixer
	VdpVideoMixerCreate *_procVIDEO_MIXER_CREATE;
	VdpVideoMixerDestroy *_procVIDEO_MIXER_DESTROY;
	VdpVideoMixerQueryFeatureSupport *_procVIDEO_MIXER_QUERY_FEATURE_SUPPORT;
	VdpVideoMixerRender *_procVIDEO_MIXER_RENDER;
	
	// VdpVideoMixerSetFeatureEnables *_video_mixer_set_feature_enables;
	// VdpVideoMixerQueryParameterSupport *_video_mixer_query_parameter_support;
	// VdpVideoMixerSetAttributeValues *_video_mixer_set_attribute_values;
	// VdpGenerateCSCMatrix *_generate_csc_matrix;

	//	Decoder
	// VdpDecoderCreate *_decoder_create;
	// VdpDecoderDestroy *_decoder_destroy;
	// VdpDecoderRender *_decoder_render;
	VdpDecoderQueryCapabilities *_procDECODER_QUERY_CAPABILITIES;
};

}
}
}
