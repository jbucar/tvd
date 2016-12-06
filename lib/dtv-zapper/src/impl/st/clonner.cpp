/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "st.h"
#include <util/cfg/cfg.h>
#include <util/log.h>

extern "C" {
#include "stapp_main.h"
#include "stgfb.h"
}

#define BLITTER_ID 0
#define GFX_HD 0
#define GFX_SD 1

#if defined(BLIT_MAX_NUMBER)&&defined(GFX_LAYER_MAX_NUMBER)
extern Semaphore_t GFX_BLIT_Sem_EventOccured[BLIT_MAX_NUMBER];
#endif

namespace st {
namespace clonner {

static STGXOBJ_Bitmap_t _bmpAux;
static bool _running=false;

static bool initLayer( int layer ) {
	if (GFX_LAYER_Handle[layer]==0) {
		LERROR( "st", "Graphic layer is undefined on this hardware: layer=%d", layer );
		return false;
	}

	if (GFX_LAYER_ViewPortHandle[layer] == 0) {
		LERROR( "st", "View port handle is undefined on this hardware: layer=%d", layer );
		return false;
	}

	//	Clear plane
	if (GRAFIX_Clear( layer, BLITTER_ID ) != ST_NO_ERROR) {
		LERROR( "st", "Cannot enable GFX layer: layer=%d", layer );
		return false;
	}

	//	Enable SD plane
	if (GRAFIX_Enable( layer ) != ST_NO_ERROR) {
		LERROR( "st", "Cannot enable GFX layer: layer=%d", layer );
		//return false;
	}

	return true;
}

static bool openEvent( STEVT_Handle_t &evtHandle, STEVT_SubscriberID_t &EVT_SubscriberID ) {
	ST_ErrorCode_t ErrCode;
	
	//	Create semaphore
	Semaphore_Init_Fifo( GFX_BLIT_Sem_EventOccured[ BLITTER_ID ], 0 );
  
	//	Initialise STBLIT Event
	STEVT_OpenParams_t EVT_OpenParams;
	EVT_OpenParams.dummy = 0;
	ErrCode=STEVT_Open( EVT_DeviceName[0], &EVT_OpenParams, &evtHandle );
	if (ErrCode!=ST_NO_ERROR) {
		Semaphore_Delete( GFX_BLIT_Sem_EventOccured[BLITTER_ID] );
		LERROR( "st", "Unable to create an event handle");
		return false;
	}
  
	//	Register for the BLIT Events
	STEVT_DeviceSubscribeParams_t EVT_SubcribeParams;
	memset(&EVT_SubcribeParams,0,sizeof(STEVT_DeviceSubscribeParams_t));
	EVT_SubcribeParams.NotifyCallback = GFXi_BLIT_Callback;
	EVT_SubcribeParams.SubscriberData_p = (void *)(0x00FAA100+BLITTER_ID);
	ErrCode=STEVT_SubscribeDeviceEvent(
		evtHandle,
		BLIT_DeviceName[BLITTER_ID],
		STBLIT_BLIT_COMPLETED_EVT,
		&EVT_SubcribeParams);
	if (ErrCode!=ST_NO_ERROR) {
		STEVT_Close(evtHandle);
		Semaphore_Delete(GFX_BLIT_Sem_EventOccured[BLITTER_ID]);
		LERROR( "st", "Unable to subscribe to blitter events" );
		return false;
	}

	//	Get subscriberID
	ErrCode=STEVT_GetSubscriberID( evtHandle, &EVT_SubscriberID );
	if (ErrCode!=ST_NO_ERROR) {
		STEVT_UnsubscribeDeviceEvent( evtHandle, BLIT_DeviceName[BLITTER_ID], STBLIT_BLIT_COMPLETED_EVT);
		STEVT_Close(evtHandle);
		Semaphore_Delete(GFX_BLIT_Sem_EventOccured[BLITTER_ID]);
		LERROR( "st", "Unable to get the subscriber id" );
		return false;
	}

	return true;
}

static bool closeEvent( STEVT_Handle_t evtHandle ) {
	bool result=true;
	ST_ErrorCode_t ErrCode;

	//	Unsubscribe
	ErrCode=STEVT_UnsubscribeDeviceEvent(
		evtHandle,
		BLIT_DeviceName[ BLITTER_ID ],
		STBLIT_BLIT_COMPLETED_EVT
	);
	if (ErrCode!=ST_NO_ERROR) {
		result &= false;
		LERROR( "st", "Unable to unsubscribe from blitter events" );
	}

	//	Close event
	ErrCode=STEVT_Close( evtHandle );
	if (ErrCode!=ST_NO_ERROR) {
		result &= false;
		LERROR( "st", "Unable to close blitter event handle" );
	}
  
	//	Delete the semaphore
	Semaphore_Delete( GFX_BLIT_Sem_EventOccured[ BLITTER_ID ] );

	return result;
}

static void showBitmap( STGXOBJ_Bitmap_t *bmp ) {
	printf("\tBitmap\n" );
	printf("\t\tColorType  : %d\n",bmp->ColorType);
	printf("\t\tBitmapType : %d\n",bmp->BitmapType);
	printf("\t\tPremult    : %d\n",bmp->PreMultipliedColor);
	printf("\t\tColorSpace : %d\n",bmp->ColorSpaceConversion);
	printf("\t\tAspect     : %d\n",bmp->AspectRatio);
    printf("\t\tWidth      : %d\n",bmp->Width);
    printf("\t\tHeight     : %d\n",bmp->Height);

    printf("\t\tPitch     : %d\n",bmp->Pitch);
    printf("\t\tOffset    : %d\n",bmp->Offset);
	printf("\t\tData1_p   : %p\n",bmp->Data1_p);
	printf("\t\tData1_Cp  : %p\n",bmp->Data1_Cp);
	printf("\t\tData1_NCp : %p\n",bmp->Data1_NCp);
	printf("\t\tSize1     : %d\n",bmp->Size1);

    printf("\t\tPitch2    : %d\n",bmp->Pitch2);
    printf("\t\tOffset2   : %d\n",bmp->Offset2);
	printf("\t\tData2_p   : %p\n",bmp->Data2_p);
	printf("\t\tData2_Cp  : %p\n",bmp->Data2_Cp);
	printf("\t\tData2_NCp : %p\n",bmp->Data2_NCp);
	printf("\t\tSize2     : %d\n",bmp->Size2);

	printf("\t\tSubtype   : %d\n",bmp->SubByteFormat);
	printf("\t\tBit       : %d\n",bmp->BigNotLittle);
	printf("\t\tYUVScal   : (%d,%d)\n", bmp->YUVScaling.ScalingFactorY, bmp->YUVScaling.ScalingFactorUV);	
}

static bool allocBitmap() {
	ST_ErrorCode_t ErrCode;
	
	//	Destination
	STGXOBJ_Bitmap_t              LAYER_GXOBJ_Bitmap_Dst;
	STGXOBJ_Palette_t             LAYER_GXOBJ_Palette_Dst;
	STLAYER_ViewPortParams_t      LAYER_ViewPortParams_Dst;
	STLAYER_ViewPortSource_t      LAYER_ViewPortSource_Dst;
	memset(&LAYER_ViewPortParams_Dst,0,sizeof(STLAYER_ViewPortParams_t));
	memset(&LAYER_ViewPortSource_Dst,0,sizeof(STLAYER_ViewPortSource_t));
	memset(&LAYER_GXOBJ_Bitmap_Dst  ,0,sizeof(STGXOBJ_Bitmap_t));
	memset(&LAYER_GXOBJ_Palette_Dst ,0,sizeof(STGXOBJ_Palette_t));
	LAYER_ViewPortSource_Dst.Data.BitMap_p = &LAYER_GXOBJ_Bitmap_Dst;
	LAYER_ViewPortSource_Dst.Palette_p     = &LAYER_GXOBJ_Palette_Dst;
	LAYER_ViewPortParams_Dst.Source_p      = &LAYER_ViewPortSource_Dst;
	ErrCode=STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[ GFX_SD ],&LAYER_ViewPortParams_Dst);
	if (ErrCode!=ST_NO_ERROR) {
		LERROR( "st", "Unable to get GFX layer parameters" );
		return false;
	}

	//	Disable color key on GFX SD layer
	if (STLAYER_DisableColorKey( GFX_LAYER_ViewPortHandle[ GFX_SD ] ) != ST_NO_ERROR) {
		LERROR( "st", "Unable to disable color key on GFX layer" );
		return false;
	}
	
	//	Copy from view port bitmap
	memcpy( &_bmpAux, LAYER_ViewPortSource_Dst.Data.BitMap_p, sizeof(STGXOBJ_Bitmap_t) );
	_bmpAux.ColorType = STGXOBJ_COLOR_TYPE_ARGB8888_255;	//	HACK!!!! Con ARGB8888 hace cosas locas con el alpha
 	_bmpAux.Data2_p              = NULL;
	_bmpAux.PreMultipliedColor   = FALSE;
	_bmpAux.Size2                = 0;

	//	Get parameters
	STGXOBJ_BitmapAllocParams_t GXOBJ_BitmapAllocParams1;
	STGXOBJ_BitmapAllocParams_t	GXOBJ_BitmapAllocParams2;
	ErrCode = STLAYER_GetBitmapAllocParams(GFX_LAYER_Handle[GFX_SD], &_bmpAux, &GXOBJ_BitmapAllocParams1, &GXOBJ_BitmapAllocParams2);
	if (ErrCode != ST_NO_ERROR) {
		LERROR( "st", "Unable to get bitmap allocation parameters" );
		return false;
	}
	_bmpAux.Offset = GXOBJ_BitmapAllocParams1.Offset;
	_bmpAux.Pitch  = GXOBJ_BitmapAllocParams1.Pitch;
	_bmpAux.Size1  = GXOBJ_BitmapAllocParams1.AllocBlockParams.Size;
	
	//	Alloc memory
	_bmpAux.Data1_NCp = (void *)SYS_Memory_Allocate( 
		SYS_MEMORY_AVMEM|SYS_MEMORY_CONTIGUOUS|SYS_MEMORY_SECURED|0,
		GXOBJ_BitmapAllocParams1.AllocBlockParams.Size,
		GXOBJ_BitmapAllocParams1.AllocBlockParams.Alignment
	);
	if (!_bmpAux.Data1_NCp) {
		LERROR( "st", "Unable to alloc bitmap memory" );
		return false;
	}

	//	Convert to device memory
	_bmpAux.Data1_p = (void *)SYS_Memory_UserToDevice(
		SYS_MEMORY_AVMEM|SYS_MEMORY_CONTIGUOUS|SYS_MEMORY_SECURED|0,
		(U32)_bmpAux.Data1_NCp
	);
	_bmpAux.Data1_Cp = (void *)_bmpAux.Data1_NCp;

	//	Show!
	showBitmap( &_bmpAux );

	return true;
}

static void freeBitmap() {
	if (_bmpAux.Data1_NCp) {
		SYS_Memory_Deallocate(
			SYS_MEMORY_AVMEM|SYS_MEMORY_CONTIGUOUS|SYS_MEMORY_SECURED|0, 
			(U32)_bmpAux.Data1_NCp
		);
	}
}

static bool blitBitmap() {
	ST_ErrorCode_t                ErrCode=ST_NO_ERROR;
	STEVT_Handle_t                BLIT_EVT_Handle;
	STBLIT_Source_t               Src;
	STBLIT_Destination_t          Dst;
	STBLIT_BlitContext_t          BLIT_Context;

	//	Source
	STGXOBJ_Bitmap_t              LAYER_GXOBJ_Bitmap_Src;
	STGXOBJ_Palette_t             LAYER_GXOBJ_Palette_Src;
	STLAYER_ViewPortParams_t      LAYER_ViewPortParams_Src;
	STLAYER_ViewPortSource_t      LAYER_ViewPortSource_Src;
	memset(&LAYER_ViewPortParams_Src,0,sizeof(STLAYER_ViewPortParams_t));
	memset(&LAYER_ViewPortSource_Src,0,sizeof(STLAYER_ViewPortSource_t));
	memset(&LAYER_GXOBJ_Bitmap_Src  ,0,sizeof(STGXOBJ_Bitmap_t));
	memset(&LAYER_GXOBJ_Palette_Src ,0,sizeof(STGXOBJ_Palette_t));
	LAYER_ViewPortSource_Src.Data.BitMap_p = &LAYER_GXOBJ_Bitmap_Src;
	LAYER_ViewPortSource_Src.Palette_p     = &LAYER_GXOBJ_Palette_Src;
	LAYER_ViewPortParams_Src.Source_p      = &LAYER_ViewPortSource_Src;
	ErrCode=STLAYER_GetViewPortParams( GFX_LAYER_ViewPortHandle[ GFX_HD ], &LAYER_ViewPortParams_Src );
	if (ErrCode!=ST_NO_ERROR) {
		LERROR( "st", "Unable to get the source layer parameters" );
		return false;
	}
	
	//	Destination
	STGXOBJ_Bitmap_t              LAYER_GXOBJ_Bitmap_Dst;
	STGXOBJ_Palette_t             LAYER_GXOBJ_Palette_Dst;
	STLAYER_ViewPortParams_t      LAYER_ViewPortParams_Dst;
	STLAYER_ViewPortSource_t      LAYER_ViewPortSource_Dst;
	memset(&LAYER_ViewPortParams_Dst,0,sizeof(STLAYER_ViewPortParams_t));
	memset(&LAYER_ViewPortSource_Dst,0,sizeof(STLAYER_ViewPortSource_t));
	memset(&LAYER_GXOBJ_Bitmap_Dst  ,0,sizeof(STGXOBJ_Bitmap_t));
	memset(&LAYER_GXOBJ_Palette_Dst ,0,sizeof(STGXOBJ_Palette_t));
	LAYER_ViewPortSource_Dst.Data.BitMap_p = &LAYER_GXOBJ_Bitmap_Dst;
	LAYER_ViewPortSource_Dst.Palette_p     = &LAYER_GXOBJ_Palette_Dst;
	LAYER_ViewPortParams_Dst.Source_p      = &LAYER_ViewPortSource_Dst;
	ErrCode=STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[ GFX_SD ],&LAYER_ViewPortParams_Dst);
	if (ErrCode!=ST_NO_ERROR) {
		LERROR( "st", "Unable to get the destination layer parameters" );
		return false;
	}


	STEVT_SubscriberID_t EVT_SubscriberID;
	if (!openEvent( BLIT_EVT_Handle, EVT_SubscriberID )) {
		return false;
	}

	/* Setup the blitter context */
	memset(&BLIT_Context,0,sizeof(STBLIT_BlitContext_t));
	BLIT_Context.ColorKeyCopyMode         = STBLIT_COLOR_KEY_MODE_NONE;
	BLIT_Context.AluMode                  = STBLIT_ALU_COPY;
	BLIT_Context.EnableMaskWord           = FALSE;
	BLIT_Context.MaskWord                 = 0;
	BLIT_Context.EnableMaskBitmap         = FALSE;
	BLIT_Context.MaskBitmap_p             = NULL;
	BLIT_Context.MaskRectangle.PositionX  = 0;
	BLIT_Context.MaskRectangle.PositionY  = 0;
	BLIT_Context.MaskRectangle.Width      = 0;
	BLIT_Context.MaskRectangle.Height     = 0;
	BLIT_Context.WorkBuffer_p             = NULL;
	BLIT_Context.EnableColorCorrection    = FALSE;
	BLIT_Context.ColorCorrectionTable_p   = NULL;
	BLIT_Context.Trigger.EnableTrigger    = FALSE;
	BLIT_Context.Trigger.ScanLine         = 0;
	BLIT_Context.Trigger.Type             = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
	BLIT_Context.Trigger.VTGId            = 0;
	BLIT_Context.GlobalAlpha              = 0;
	BLIT_Context.EnableClipRectangle      = FALSE;
	BLIT_Context.ClipRectangle.PositionX  = 0;
	BLIT_Context.ClipRectangle.PositionY  = 0;
	BLIT_Context.ClipRectangle.Width      = _bmpAux.Width;
	BLIT_Context.ClipRectangle.Height     = _bmpAux.Height;
	BLIT_Context.WriteInsideClipRectangle = TRUE;
	BLIT_Context.EnableFlickerFilter      = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
	BLIT_Context.EnableResizeFilter       = FALSE;
#endif
	BLIT_Context.JobHandle                = STBLIT_NO_JOB_HANDLE;
	BLIT_Context.UserTag_p                = NULL;
	BLIT_Context.NotifyBlitCompletion     = TRUE;
	BLIT_Context.EventSubscriberID        = EVT_SubscriberID;
	
	//	Set source
	Src.Type                = STBLIT_SOURCE_TYPE_BITMAP;
	Src.Data.Bitmap_p       = &_bmpAux;
	Src.Rectangle.PositionX = 0;
	Src.Rectangle.PositionY = 0;
	Src.Rectangle.Width     = _bmpAux.Width;
	Src.Rectangle.Height    = _bmpAux.Height;
	Src.Palette_p           = NULL;
	
	//	Set Destination
	Dst.Bitmap_p            = LAYER_ViewPortSource_Dst.Data.BitMap_p;
	Dst.Rectangle.PositionX = 0;
	Dst.Rectangle.PositionY = 0;
	Dst.Rectangle.Width     = LAYER_ViewPortSource_Dst.Data.BitMap_p->Width;
	Dst.Rectangle.Height    = LAYER_ViewPortSource_Dst.Data.BitMap_p->Height;
	Dst.Palette_p           = NULL;

	//	Do the blit now
	ErrCode=STBLIT_Blit(BLIT_Handle[BLITTER_ID],&Src,NULL,&Dst,&BLIT_Context);
	if (ErrCode!=ST_NO_ERROR) {
		closeEvent( BLIT_EVT_Handle );
		LERROR( "st", "Unable to perform a blit copy" );
		return false;
	}

	//	Wait end of operation
	Semaphore_Wait(GFX_BLIT_Sem_EventOccured[BLITTER_ID]);
	
	closeEvent( BLIT_EVT_Handle );

	return true;
}

void clone( unsigned char *buffer, int size ) {
	LTRACE( "st", "Clone output: size=%d", size );

	if (!_running || size > (int)_bmpAux.Size1) {
		LERROR( "st", "Cannot clone output", size, _bmpAux.Size1 );
	}
	else {
		//	Copy to bmp aux
		memcpy( _bmpAux.Data1_Cp, buffer, size );

		//	Blit bmp aux to SD
		blitBitmap();
	}
}

bool initialize() {
	if (!_running) {
		LINFO( "st", "Initialize ST clonner (HD->SD)" );
		
		if (BLITTER_ID >= BLIT_MAX_NUMBER ) {
			LERROR( "st", "Invalid blitter identifier: id=%d", BLITTER_ID );
			return false;
		}

		if (!initLayer( GFX_HD )) {
			return false;
		}

		if (!initLayer( GFX_SD )) {
			return false;
		}

		if (!allocBitmap()) {
			return false;
		}

		{	//	Setup drawBuffer method in directFB
			unsigned long value = (unsigned long)&st::clonner::clone;
			util::cfg::setValue( "gui.directfb.drawBuffer", value );
		}

		_running = true;
	}

	return true;
}

void finalize() {
	if (_running) {
		LINFO( "st", "Finalize ST clonner (HD->SD)" );
		_running = false;

		GRAFIX_Disable( GFX_SD );
		freeBitmap();
	}
}

}
}
