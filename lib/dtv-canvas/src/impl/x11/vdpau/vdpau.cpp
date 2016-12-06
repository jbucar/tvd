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

#include "vdpau.h"
#include "vdpau/vdpau_x11.h"
#include <util/log.h>
#include <util/assert.h>
#include <string.h>
#include <stdio.h>

#define VDPAU_VERBOSE

#define LOAD_METHOD(id)	\
	if (!loadMethod( VDP_FUNC_ID_##id, (void **)&_proc##id )) { return false; }

#define RUN_METHOD1(id,p1) \
	if (_proc##id( p1 ) != VDP_STATUS_OK) { return false; }

#define RUN_METHOD4(id,p1,p2,p3,p4)	{\
		VdpStatus st; \
		st = _proc##id( p1, p2, p3, p4 ); \
		if (st!= VDP_STATUS_OK) { LERROR( "vdpau", "Cannot execute method %s: status=%x", #id, st ); return false; } }

#define RUN_METHOD5(id,p1,p2,p3,p4,p5)	  \
	if (_proc##id( p1, p2, p3, p4, p5 ) != VDP_STATUS_OK) { return false; }

#define RUN_METHOD7(id,p1,p2,p3,p4,p5,p6,p7)	  \
	if (_proc##id( p1, p2, p3, p4, p5, p6, p7 ) != VDP_STATUS_OK) { return false; }

#define RUN_METHOD15(id,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) \
	if (_proc##id( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15 ) != VDP_STATUS_OK) { return false; }

#define RUN_DEVICE_METHOD2(id,p1,p2) \
	if (_proc##id( _device, p1, p2 ) != VDP_STATUS_OK) { return false; }

#define RUN_DEVICE_METHOD3(id,p1,p2,p3)	  \
	if (_proc##id( _device, p1, p2, p3 ) != VDP_STATUS_OK) { return false; }

#define RUN_DEVICE_METHOD4(id,p1,p2,p3,p4)	  \
	if (_proc##id( _device, p1, p2, p3, p4 ) != VDP_STATUS_OK) { return false; }

#define RUN_DEVICE_METHOD5(id,p1,p2,p3,p4,p5)	  \
	if (_proc##id( _device, p1, p2, p3, p4, p5 ) != VDP_STATUS_OK) { return false; }

#define RUN_DEVICE_METHOD6(id,p1,p2,p3,p4,p5,p6)	{\
			VdpStatus st; \
			st=_proc##id( _device, p1, p2, p3, p4, p5, p6 ); \
			if (st != VDP_STATUS_OK) {	  \
				printf( "vdpau" "Cannot execute method %s: status=%x", #id, st ); \
				return false; \
			} }


namespace canvas {
namespace x11 {
namespace vdpau {

typedef struct {
	const char *name;
	VdpDecoderProfile profile;
} DecoderProfiles;

static DecoderProfiles dProfiles[] = {
	{ "MPEG1", VDP_DECODER_PROFILE_MPEG1 },
	{ "MPEG2 SIMPLE", VDP_DECODER_PROFILE_MPEG2_SIMPLE },
	{ "MPEG2 MAIN", VDP_DECODER_PROFILE_MPEG2_MAIN },
	{ "H264 BASELINE", VDP_DECODER_PROFILE_H264_BASELINE },
	{ "H264 MAIN", VDP_DECODER_PROFILE_H264_MAIN },
	{ "MPEG2 MAIN", VDP_DECODER_PROFILE_H264_HIGH },
	{ "VC1 SIMPLE", VDP_DECODER_PROFILE_VC1_SIMPLE },
	{ "VC1 MAIN", VDP_DECODER_PROFILE_VC1_MAIN },
	{ "VC1 ADVANCED", VDP_DECODER_PROFILE_VC1_ADVANCED },
	{ "MPEG4 PART2_SP", VDP_DECODER_PROFILE_MPEG4_PART2_SP },
	{ "MPEG4 MAIN", VDP_DECODER_PROFILE_MPEG4_PART2_ASP },
	{ "DIVX4 QMOBILE", VDP_DECODER_PROFILE_DIVX4_QMOBILE },
	{ "DIVX4 MOBILE", VDP_DECODER_PROFILE_DIVX4_MOBILE },
	{ "DIVX4 THEATER", VDP_DECODER_PROFILE_DIVX4_HOME_THEATER },
	{ "DIVX4 HD 1080p", VDP_DECODER_PROFILE_DIVX4_HD_1080P },
	{ "DIVX5 QMOBILIE", VDP_DECODER_PROFILE_DIVX5_QMOBILE },
	{ "DIVX5 MOBILE", VDP_DECODER_PROFILE_DIVX5_MOBILE },
	{ "DIVX5 HOME THEATER", VDP_DECODER_PROFILE_DIVX5_HOME_THEATER },
	{ "DIVX5 HD 1080p", VDP_DECODER_PROFILE_DIVX5_HD_1080P },
	{ NULL, 0 }
};

typedef struct {
	const char *name;
	VdpVideoMixerFeature feature;
} MixerFeatures;

static MixerFeatures vmFeatures[] = {
	{ "Temporal", VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL },
	{ "Temporal Spatial", VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL_SPATIAL },
	{ "Inverse telecine", VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE },
	{ "Noise reduction", VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION },
	{ "Sharpness", VDP_VIDEO_MIXER_FEATURE_SHARPNESS },
	{ "Luma key", VDP_VIDEO_MIXER_FEATURE_LUMA_KEY },
	{ "High quality scaling L1", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L1 },
	{ "High quality scaling L2", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L2 },
	{ "High quality scaling L3", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L3 },
	{ "High quality scaling L4", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L4 },
	{ "High quality scaling L5", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L5 },
	{ "High quality scaling L6", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L6 },
	{ "High quality scaling L7", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L7 },
	{ "High quality scaling L8", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L8 },
	{ "High quality scaling L9", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L9 },
	{ NULL, 0 }
};

typedef struct {
	const char *name;
	VdpRGBAFormat format;
} VDPFormats;

static VDPFormats vdpFormats[] = {
	{ "B8G8R8A8", VDP_RGBA_FORMAT_B8G8R8A8 },
	{ "R8G8B8A8", VDP_RGBA_FORMAT_R8G8B8A8 },
	{ "R10G10B10A2", VDP_RGBA_FORMAT_R10G10B10A2 },
	{ "B10G10R10A2", VDP_RGBA_FORMAT_B10G10R10A2 },
	{ "A8", VDP_RGBA_FORMAT_A8 }
};

typedef struct {
	const char *name;
	VdpChromaType format;
} VDPChromaFormats;

static VDPChromaFormats vdpChromaFormats[] = {
	{ "420", VDP_CHROMA_TYPE_420 },
	{ "422", VDP_CHROMA_TYPE_422 },
	{ "444", VDP_CHROMA_TYPE_444 }
};

Wrapper::Wrapper()
{
	_device = VDP_INVALID_HANDLE;
	_getProcAddress = NULL;
	_procDEVICE_DESTROY = NULL;
	_procGET_ERROR_STRING = NULL;
	_procGET_API_VERSION = NULL;
	_procGET_INFORMATION_STRING = NULL;

	//	Bitmap
	_procBITMAP_SURFACE_QUERY_CAPABILITIES = NULL;
	_procBITMAP_SURFACE_CREATE = NULL;
	_procBITMAP_SURFACE_DESTROY = NULL;
	_procBITMAP_SURFACE_PUT_BITS_NATIVE = NULL;

	//	Decoder
	_procDECODER_QUERY_CAPABILITIES = NULL;

	//	Video mixer
	_procVIDEO_MIXER_CREATE = NULL;
	_procVIDEO_MIXER_DESTROY = NULL;
	_procVIDEO_MIXER_QUERY_FEATURE_SUPPORT = NULL;
	_procVIDEO_MIXER_RENDER = NULL;

	//	Presentation
	_procPRESENTATION_QUEUE_TARGET_CREATE_X11 = NULL;
	_procPRESENTATION_QUEUE_TARGET_DESTROY = NULL;	
	_procPRESENTATION_QUEUE_CREATE = NULL;
	_procPRESENTATION_QUEUE_DESTROY = NULL;
	_procPRESENTATION_QUEUE_DISPLAY = NULL;

	//	Video surface
	_procVIDEO_SURFACE_QUERY_CAPABILITIES = NULL;
	_procVIDEO_SURFACE_CREATE = NULL;
	_procVIDEO_SURFACE_DESTROY = NULL;
	_procVIDEO_SURFACE_PUT_BITS_Y_CB_CR = NULL;

	//	Output surface
	_procOUTPUT_SURFACE_QUERY_CAPABILITIES = NULL;
	_procOUTPUT_SURFACE_CREATE = NULL;
	_procOUTPUT_SURFACE_DESTROY = NULL;
	_procOUTPUT_SURFACE_PUT_BITS_NATIVE = NULL;

	_procOUTPUT_SURFACE_RENDER_BITMAP_SURFACE = NULL;
	_procOUTPUT_SURFACE_RENDER_OUTPUT_SURFACE = NULL;
}

Wrapper::~Wrapper()
{
	DTV_ASSERT( !isOpen() );
}

bool Wrapper::isOpen() const {
	return _device != VDP_INVALID_HANDLE;
}

bool Wrapper::open( Display *display, int screen ) {
	VdpStatus status;

	DTV_ASSERT(!isOpen());

	//	Create device
	status = vdp_device_create_x11(
		display,
		screen,
		&_device,
		&_getProcAddress );	
	if (status != VDP_STATUS_OK) {
		LERROR( "vdpau", "Cannot create vdp device" );
		return false;
	}

	//	Load generic methods
	LOAD_METHOD( DEVICE_DESTROY )
	LOAD_METHOD( GET_API_VERSION );
	LOAD_METHOD( GET_ERROR_STRING );
	LOAD_METHOD( GET_INFORMATION_STRING );

	//	Bitmap
	LOAD_METHOD( BITMAP_SURFACE_QUERY_CAPABILITIES );
	LOAD_METHOD( BITMAP_SURFACE_CREATE );
	LOAD_METHOD( BITMAP_SURFACE_DESTROY );
	LOAD_METHOD( BITMAP_SURFACE_PUT_BITS_NATIVE );

	//	Decoder
	LOAD_METHOD( DECODER_QUERY_CAPABILITIES );

	//	Video mixer
	LOAD_METHOD( VIDEO_MIXER_CREATE );
	LOAD_METHOD( VIDEO_MIXER_DESTROY );
	LOAD_METHOD( VIDEO_MIXER_QUERY_FEATURE_SUPPORT );
	LOAD_METHOD( VIDEO_MIXER_RENDER );

	//	Video surface
	LOAD_METHOD( VIDEO_SURFACE_QUERY_CAPABILITIES );
	LOAD_METHOD( VIDEO_SURFACE_CREATE );
	LOAD_METHOD( VIDEO_SURFACE_DESTROY );	
	LOAD_METHOD( VIDEO_SURFACE_PUT_BITS_Y_CB_CR );
	
	//	Output surface
	LOAD_METHOD( OUTPUT_SURFACE_QUERY_CAPABILITIES );
	LOAD_METHOD( OUTPUT_SURFACE_CREATE );
	LOAD_METHOD( OUTPUT_SURFACE_DESTROY );
	LOAD_METHOD( OUTPUT_SURFACE_PUT_BITS_NATIVE );

	LOAD_METHOD( OUTPUT_SURFACE_RENDER_BITMAP_SURFACE );
	LOAD_METHOD( OUTPUT_SURFACE_RENDER_OUTPUT_SURFACE );
	
	//	Presentation
	LOAD_METHOD( PRESENTATION_QUEUE_TARGET_CREATE_X11 );
	LOAD_METHOD( PRESENTATION_QUEUE_TARGET_DESTROY );		
	LOAD_METHOD( PRESENTATION_QUEUE_CREATE );
	LOAD_METHOD( PRESENTATION_QUEUE_DESTROY );
	LOAD_METHOD( PRESENTATION_QUEUE_DISPLAY );

#ifdef VDPAU_VERBOSE
	showInfo();
#endif
	
	return true;
}

void Wrapper::close() {
	if (isOpen()) {
		if (_procDEVICE_DESTROY( _device ) != VDP_STATUS_OK) {
			LWARN( "vdpau", "Destroy vdp device failed\n" );
		}
		_device = VDP_INVALID_HANDLE;
	}
}

bool Wrapper::loadMethod( VdpFuncId id, void **ptr ) {
	VdpStatus st = _getProcAddress( _device, id, ptr );
	return st == VDP_STATUS_OK;
}

//	Video mixer
bool Wrapper::vmCreate(
	uint32_t featureCount,
	VdpVideoMixerFeature const *features,
	uint32_t parameterCount,
	VdpVideoMixerParameter const *parameters,
	void const * const *parameterValues,
	VdpVideoMixer *mixer )
{
	RUN_DEVICE_METHOD6(
		VIDEO_MIXER_CREATE,
		featureCount,
		features,
		parameterCount,
		parameters,
		parameterValues,
		mixer );
	return true;
}

bool Wrapper::vmDestroy( VdpVideoMixer mixer ) {
	if (mixer != VDP_INVALID_HANDLE) {
		RUN_METHOD1( VIDEO_MIXER_DESTROY, mixer );
		return true;
	}
	return false;
}

bool Wrapper::vmRender(
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
)
{
	RUN_METHOD15(
		VIDEO_MIXER_RENDER,
		mixer,
		backgroundSurface,
		backgroundSrcRect,
		currentPictureStructure,
		videoSurfacePastCount,
		videoSurfacePast,
		videoSurfaceCurrent,
		videoSurfaceFutureCount,
		videoSurfaceFuture,
		videoSourceRect,
		dstSurface,
		dstRect,
		dstVideoRect,
		layerCount,
		layers
	);
	return true;
}

bool Wrapper::vmQueryFeature( VdpVideoMixerFeature feature ) {
	VdpBool supported;
	RUN_DEVICE_METHOD2(VIDEO_MIXER_QUERY_FEATURE_SUPPORT, feature, &supported);
	return supported ? true : false;
}

bool Wrapper::getApiVersion( unsigned int &version ) {
	RUN_METHOD1(GET_API_VERSION, &version);
	return true;
}

bool Wrapper::getInformation( std::string &information ) {
	const char *tmp;
	RUN_METHOD1(GET_INFORMATION_STRING, &tmp);
	information=tmp;
	return true;
}

bool Wrapper::dQueryCapabilities( VdpDecoderProfile profile, DecoderCapabilities &capabilities ) {
	VdpBool ok=false;	
	RUN_DEVICE_METHOD6(
		DECODER_QUERY_CAPABILITIES,
		profile,
		&ok,
		&capabilities.max_level,
		&capabilities.max_macroblocks,
		&capabilities.max_width,
		&capabilities.max_height );
	return ok ? true : false;
}

//	Presentation queue
bool Wrapper::pqtCreate( Drawable drawable, VdpPresentationQueueTarget *target ) {
	RUN_DEVICE_METHOD2(
		PRESENTATION_QUEUE_TARGET_CREATE_X11,
		drawable,
		target );
	return true;
}

bool Wrapper::pqtDestroy( VdpPresentationQueueTarget target ) {
	if (target != VDP_INVALID_HANDLE) {
		RUN_METHOD1( PRESENTATION_QUEUE_TARGET_DESTROY, target );
		return true;
	}
	return false;
}

bool Wrapper::pqCreate( VdpPresentationQueueTarget target, VdpPresentationQueue *queue ) {
	RUN_DEVICE_METHOD2(
		PRESENTATION_QUEUE_CREATE,
		target,
		queue );
	return true;
}
	
bool Wrapper::pqDestroy( VdpPresentationQueue queue ) {
	if (queue != VDP_INVALID_HANDLE) {
		RUN_METHOD1( PRESENTATION_QUEUE_DESTROY, queue );
		return true;
	}
	return false;
}

bool Wrapper::pqDisplay(
	VdpPresentationQueue queue,
	VdpOutputSurface     surface,
	uint32_t             clip_width,
	uint32_t             clip_height,
	VdpTime              earliest_presentation_time )
{
	RUN_METHOD5(
		PRESENTATION_QUEUE_DISPLAY,
		queue,
		surface,
		clip_width,
		clip_height,
		earliest_presentation_time );
	return true;
}

//	Video surface
bool Wrapper::vsQueryCapabilities(
	VdpChromaType format,
	uint32_t *maxWidth,
	uint32_t *maxHeight )
{
	VdpBool ok=false;
	RUN_DEVICE_METHOD4(
		VIDEO_SURFACE_QUERY_CAPABILITIES,
		format,
		&ok,
		maxWidth,
		maxHeight );
	return ok ? true : false;
}

bool Wrapper::vsCreate(
	VdpChromaType type,
	uint32_t width,
	uint32_t height,
	VdpVideoSurface *surface )
{
	RUN_DEVICE_METHOD4(
		VIDEO_SURFACE_CREATE,
		type,
		width,
		height,
		surface
	);	
	return true;
}

bool Wrapper::vsDestroy( VdpVideoSurface surface ) {
	if (surface != VDP_INVALID_HANDLE) {
		RUN_METHOD1( VIDEO_SURFACE_DESTROY, surface );
		return true;
	}
	return false;
}

bool Wrapper::vsPutBitsYCbCr(
	VdpVideoSurface surface,
	VdpYCbCrFormat format,
	void const * const * source_data,
	uint32_t const * source_pitches
)
{
	RUN_METHOD4(
		VIDEO_SURFACE_PUT_BITS_Y_CB_CR,
		surface,
		format,
		source_data,
		source_pitches
	);
	return true;
}

//	Output surface
bool Wrapper::osQueryCapabilities(
	VdpRGBAFormat format,
	uint32_t *maxWidth,
	uint32_t *maxHeight )
{
	VdpBool ok=false;
	RUN_DEVICE_METHOD4(
		OUTPUT_SURFACE_QUERY_CAPABILITIES,
		format,
		&ok,
		maxWidth,
		maxHeight );
	return ok ? true : false;
}

bool Wrapper::osCreate(
	VdpRGBAFormat format,
	uint32_t width,
	uint32_t height,
	VdpOutputSurface *surface )
{
	RUN_DEVICE_METHOD4(
		OUTPUT_SURFACE_CREATE,
		format,
		width,
		height,
		surface
	);	
	return true;
}

bool Wrapper::osDestroy( VdpOutputSurface surface ) {
	if (surface != VDP_INVALID_HANDLE) {
		RUN_METHOD1( OUTPUT_SURFACE_DESTROY, surface );
		return true;
	}
	return false;
}

bool Wrapper::osPutBitsNative(
	VdpOutputSurface surface,
	void const *const *source_data,
	uint32_t const *source_pitches,
	VdpRect const *destination_rect )
{
	RUN_METHOD4(
		OUTPUT_SURFACE_PUT_BITS_NATIVE,
		surface,
		source_data,
		source_pitches,
		destination_rect );
	return true;
}

bool Wrapper::osRenderBitmapSurface(
	VdpOutputSurface dst,
	VdpRect const *dstRect,
	VdpBitmapSurface src,
	VdpRect const *srcRect,
	VdpColor const *colors,
	VdpOutputSurfaceRenderBlendState const *blendState,
	uint32_t flags )
{
	RUN_METHOD7(
		OUTPUT_SURFACE_RENDER_BITMAP_SURFACE,
		dst,
		dstRect,
		src,
		srcRect,
		colors,
		blendState,
		flags );
	return true;
}

//	Bitmap
bool Wrapper::bsQueryCapabilities( VdpRGBAFormat format, uint32_t *maxWidth, uint32_t *maxHeight ) {
	VdpBool ok=false;
	RUN_DEVICE_METHOD4(
		BITMAP_SURFACE_QUERY_CAPABILITIES,
		format,
		&ok,
		maxWidth,
		maxHeight );
	return ok ? true : false;
}

bool Wrapper::bsCreate(
	VdpRGBAFormat format,
	uint32_t width, uint32_t height,
	VdpBool frequently_accessed,
	VdpBitmapSurface *surface )
{
	RUN_DEVICE_METHOD5(
		BITMAP_SURFACE_CREATE,
		format,
		width,
		height,
		frequently_accessed,
		surface );
	return true;
}

bool Wrapper::bsDestroy( VdpBitmapSurface surface ) {
	RUN_METHOD1( BITMAP_SURFACE_DESTROY, surface );
	return true;
}

bool Wrapper::bsPutBitsNative(
	VdpBitmapSurface surface,
	void const *const *data,
	uint32_t const *pitches,
	VdpRect const *dstRect )
{
	RUN_METHOD4(
		BITMAP_SURFACE_PUT_BITS_NATIVE,
		surface,
		data,
		pitches,
		dstRect );
	return true;
}

//	Debugging
void Wrapper::showInfo() {
	unsigned int tmp;
	if (getApiVersion(tmp)) {
		printf( "[VDPAU] API version: %d\n", tmp );
	}

	std::string info;
	if (getInformation(info)) {
		printf( "[VDPAU] Implementation: %s\n", info.c_str() );
	}

	{
		printf( "[VDPAU] Supported decoder profiles:\n" );
		int i=0;
		while (dProfiles[i].name) {
			DecoderCapabilities caps;
			if (dQueryCapabilities( dProfiles[i].profile, caps )) {
				printf( "[VDPAU] %20s: maxLevel=%02d, maxMB=%04d, maxWidth=%04d, maxHeight=%04d\n",
				        dProfiles[i].name, caps.max_level, caps.max_macroblocks, caps.max_width, caps.max_height );
			}
			i++;
		}
	}

	{
		printf( "\n[VDPAU] Video mixer features:\n");
		int i=0;
		while (vmFeatures[i].name) {
			if (vmQueryFeature( vmFeatures[i].feature )) {
				printf( "[VDPAU] %25s\n", vmFeatures[i].name );
			}
			i++;
		}
	}

	{
		printf( "\n[VDPAU] Video surface format features:\n");
		int i=0;
		uint32_t width, heigth;
		while (vdpChromaFormats[i].name) {
			if (vsQueryCapabilities( vdpChromaFormats[i].format, &width, &heigth )) {
				printf( "[VDPAU] %15s: max(%d,%d)\n", vdpChromaFormats[i].name, width, heigth );
			}
			i++;
		}
	}

	{
		printf( "\n[VDPAU] Output surface format features:\n");
		int i=0;
		uint32_t width, heigth;
		while (vdpFormats[i].name) {
			if (osQueryCapabilities( vdpFormats[i].format, &width, &heigth )) {
				printf( "[VDPAU] %15s: max(%d,%d)\n", vdpFormats[i].name, width, heigth );
			}
			i++;
		}
	}
	
	{
		printf( "\n[VDPAU] Bitmap format features:\n");
		int i=0;
		uint32_t width, heigth;
		while (vdpFormats[i].name) {
			if (bsQueryCapabilities( vdpFormats[i].format, &width, &heigth )) {
				printf( "[VDPAU] %15s: max(%d,%d)\n", vdpFormats[i].name, width, heigth );
			}
			i++;
		}
	}
}

}	//	namespace vdpau
}	//	namespace x11
}	//	namespace canvas

