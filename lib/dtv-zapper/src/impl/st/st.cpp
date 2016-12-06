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
#include <util/log.h>

extern "C" {
#include "stapp_main.h"
}

#define STGFB_INSERT_FULLHD "insmod /root/modules/stgfb_core.ko mod_init=NO g_devnum=1 layer_name=GFX_LAYER0 g_tvOutMode=1920x1080-60 g_mem=16 g_format=ARGB8888 g_ratio=16TO9 mem_clear=ALL g_width=720 g_height=576 g_outwidth=720 g_outheight=576"

namespace st {

namespace impl {
	static bool isMuted = false;
}


namespace clonner {
	bool initialize();
	void finalize();
}

static ST_ErrorCode_t MODULES_Init(void) {
	system(STGFB_INSERT_FULLHD);
	return (ST_NO_ERROR);
}

static bool STSDK_Init( U32 SDK_Mode ) {
	ST_ErrorCode_t ErrCode=ST_NO_ERROR;
	U32            ErrDriverLayer[2]={0,0},ErrMiddleLayer[2]={0,0},ErrModuleLayer[2]={0,0},ErrAppliLayer[2]={0,0};

	/* Initialize driver layer */
	if ( SYS_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 0); /* Kernel + memory + cache + system clocks */
	if ( PIO_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 1); /* Configure the Pio pins                  */
	if ( UART_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 2); /* Configure the Uart ports                */
	if ( TBX_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 3); /* Configure the STAPI Trace module        */
	if ( TRACE_Init     (TRACE_UART_ID)!= ST_NO_ERROR ) ErrDriverLayer[0] |= (1<< 4); /* Configure the Trace driver              */
	if ( EVT_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 5); /* Configure the Event handler             */
	if ( I2C_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 6); /* Configure the I2C ports                 */
	if ( SPI_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 7); /* Configure the SPI interface             */
	if ( PWM_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 8); /* Configure the PWM                       */
	if ( FLASH_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 9); /* Configure the Flash drivers             */
	if ( PCPD_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<10); /* Configure the PCPD driver               */
	if ( SCART_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<11); /* Configure the SCARTs                    */
	if ( TUNER_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<12); /* Configure the Tuners                    */
	if ( DMA_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<13); /* Configure the DMA engines               */
	if ( DEMUX_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<14); /* Configure the Demux (Merger+Pti)        */
	if ( INJECT_Init    ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<15); /* Configure the Inject interface          */
	if ( CLKRV_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<16); /* Configure the Clock recovery            */
	if ( DENC_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<17); /* Configure the DENC output               */
	if ( VTG_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<18); /* Configure the VTG time generators       */
	if ( VOUT_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<19); /* Configure the VOS output                */
	if ( LAYER_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<20); /* Configure the Video layers              */
	if ( VBI_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<21); /* Configure the VBI interface             */
	if ( BLIT_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<22); /* Configure the Bitter                    */
	if ( GRAFIX_Init    ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<23); /* Configure the Graphic layers            */
	if ( VMIX_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<24); /* Configure the Mixers                    */
	if ( VID_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<25); /* Configure the Video drivers             */
	if ( AUD_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<26); /* Configure the Audio drivers             */
	//if ( BLAST_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<27); /* Configure the Blast driver              */
	if ( USB_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<28); /* Configure the USB interface             */
	if ( GAMLOAD_Init   ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<29); /* Configure the GAM loader                */
	if ( CRYPT_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 0); /* Configure the CRYPT interface           */
	if ( ATAPI_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 1); /* Configure the ATAPI interface           */
	if ( SMART_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 2); /* Configure the Smartcard interface       */
	if ( HDMI_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 3); /* Configure the HDMI interface            */
	if ( KEYSCN_Init    ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 4); /* Configure the KEYSCAN interface         */
	if ( VIN_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 5); /* Configure the VIN interface             */
	if ( TTX_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 6); /* Configure the Teletext driver           */
	if ( CC_Init        ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 7); /* Configure the Close caption driver      */
	if ( POD_Init       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 8); /* Configure the POD interface             */
	if ( MOCA_Init      ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 9); /* Configure the MoCA interface            */
	if ( PCCRD_Init     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<<10); /* Configure the PCCRD interface           */
	//if ( GRAFIX_DFBInit ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<<11); /* Configure the GRAFIX-DirectFB interface */

	/* Initialize middle layer */
	if (SDK_Mode&SDK_INIT_MIDDLE_LAYER)	{
		if ( TCPIP_Init   ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 0); /* Initialize TCP/IP stack                 */
		if ( VFS_Init     ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 1); /* Initialize the file system              */
		if ( PLAYREC_Init ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 2); /* Initialize the play and record manager  */
		if ( SUBT_Init    ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 3); /* Initialize the Subtitle driver          */
	}

	/* Initialize module layer */
	if (SDK_Mode&SDK_INIT_MODULE_LAYER)	{
		if ( MODULES_Init ( )!= ST_NO_ERROR )             ErrModuleLayer[0] |= (1<< 0); /* Initialize the modules                  */
	}

	/* Initialize application layer */
	if (SDK_Mode&SDK_INIT_APPLICATION_LAYER) {
		if ( GUI_Init     ( )!= ST_NO_ERROR )             ErrAppliLayer[0] |= (1<< 0);  /* Initialize the GUI interface            */
	}

	/* Check initialization */
	if ((ErrDriverLayer[0]!=0)||(ErrDriverLayer[1]!=0)) {
		ErrCode|=SDK_INIT_DRIVER_LAYER_FAILED;
		LWARN("st", "Errors encountered during initialization of the driver layer, code = 0x%08x%08x failure summary:",ErrDriverLayer[1], ErrDriverLayer[0]);
		if (ErrDriverLayer[0] & (1<< 0)) LWARN("st", "\t- SYS_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 1)) LWARN("st", "\t- PIO_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 2)) LWARN("st", "\t- UART_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 3)) LWARN("st", "\t- TBX_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 4)) LWARN("st", "\t- TRACE_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 5)) LWARN("st", "\t- EVT_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 6)) LWARN("st", "\t- I2C_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 7)) LWARN("st", "\t- SPI_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 8)) LWARN("st", "\t- PWM_Init() Failure");
		if (ErrDriverLayer[0] & (1<< 9)) LWARN("st", "\t- FLASH_Init() Failure");
		if (ErrDriverLayer[0] & (1<<10)) LWARN("st", "\t- PCPD_Init() Failure");
		if (ErrDriverLayer[0] & (1<<11)) LWARN("st", "\t- MBX_Init() Failure");
		if (ErrDriverLayer[0] & (1<<12)) LWARN("st", "\t- SCART_Init() Failure");
		if (ErrDriverLayer[0] & (1<<13)) LWARN("st", "\t- TUNER_Init() Failure");
		if (ErrDriverLayer[0] & (1<<14)) LWARN("st", "\t- DMA_Init() Failure");
		if (ErrDriverLayer[0] & (1<<15)) LWARN("st", "\t- DEMUX_Init() Failure");
		if (ErrDriverLayer[0] & (1<<16)) LWARN("st", "\t- INJECT_Init() Failure");
		if (ErrDriverLayer[0] & (1<<17)) LWARN("st", "\t- CLKRV_Init() Failure");
		if (ErrDriverLayer[0] & (1<<18)) LWARN("st", "\t- DENC_Init() Failure");
		if (ErrDriverLayer[0] & (1<<19)) LWARN("st", "\t- VTG_Init() Failure");
		if (ErrDriverLayer[0] & (1<<20)) LWARN("st", "\t- VOUT_Init() Failure");
		if (ErrDriverLayer[0] & (1<<21)) LWARN("st", "\t- LAYER_Init() Failure");
		if (ErrDriverLayer[0] & (1<<22)) LWARN("st", "\t- VBI_Init() Failure");
		if (ErrDriverLayer[0] & (1<<23)) LWARN("st", "\t- BLIT_Init() Failure");
		if (ErrDriverLayer[0] & (1<<24)) LWARN("st", "\t- GRAFIX_Init() Failure");
		if (ErrDriverLayer[0] & (1<<25)) LWARN("st", "\t- VMIX_Init() Failure");
		if (ErrDriverLayer[0] & (1<<26)) LWARN("st", "\t- VID_Init() Failure");
		if (ErrDriverLayer[0] & (1<<27)) LWARN("st", "\t- AUD_Init() Failure");
		if (ErrDriverLayer[0] & (1<<28)) LWARN("st", "\t- BLAST_Init() Failure");
		if (ErrDriverLayer[0] & (1<<29)) LWARN("st", "\t- USB_Init() Failure");
		if (ErrDriverLayer[0] & (1<<30)) LWARN("st", "\t- GAMLOAD_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 0)) LWARN("st", "\t- CRYPT_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 1)) LWARN("st", "\t- ATAPI_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 2)) LWARN("st", "\t- SMART_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 3)) LWARN("st", "\t- HDMI_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 4)) LWARN("st", "\t- KEYSCN_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 5)) LWARN("st", "\t- VIN_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 6)) LWARN("st", "\t- TTX_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 7)) LWARN("st", "\t- CC_Init() Failure");
		if (ErrDriverLayer[1] & (1<< 8)) LWARN("st", "\t- POD_Init() Failure");
	}

	if ((ErrMiddleLayer[0]!=0)||(ErrMiddleLayer[1]!=0)) {
		ErrCode|=SDK_INIT_MIDDLE_LAYER_FAILED;
		LWARN("st", "Errors encountered during initialization of the middle layer, code = 0x%08x%08x failure summary:",ErrMiddleLayer[1],ErrMiddleLayer[0]);
		if (ErrMiddleLayer[0] & (1<< 0)) LWARN("st", "\t- TCPIP_Init() Failure");
		if (ErrMiddleLayer[0] & (1<< 1)) LWARN("st", "\t- VFS_Init() Failure");
		if (ErrMiddleLayer[0] & (1<< 2)) LWARN("st", "\t- PLAYREC_Init() Failure");
		if (ErrMiddleLayer[0] & (1<< 3)) LWARN("st", "\t- SUBT_Init() Failure");
	}
	
	if ((ErrModuleLayer[0]!=0)||(ErrModuleLayer[1]!=0)) {
		ErrCode|=SDK_INIT_MODULE_LAYER_FAILED;
		LWARN("st", "Errors encountered during initialization of the module layer, code = 0x%08x%08x failure summary:",ErrModuleLayer[1],ErrModuleLayer[0]);
		if (ErrModuleLayer[0] & (1<< 0)) LWARN("st", "\t- MODULES_Init() Failure");
	}
	
	if ((ErrAppliLayer[0]!=0)||(ErrAppliLayer[1]!=0)) {
		ErrCode|=SDK_INIT_APPLICATION_LAYER_FAILED;
		LWARN("st", "Errors encountered during initialization of the appli  layer, code = 0x%08x%08x failure summary:",ErrAppliLayer[1],ErrAppliLayer[0]);
		if (ErrAppliLayer[0] & (1<< 0)) LWARN("st", "\t- GUI_Init() Failure");
	}

	//	HACK: Setup both VTG always to 720x576
	if (!setResolution( STVTG_TIMING_MODE_576P50000_27000, STVTG_TIMING_MODE_576I50000_13500 )) {
		return false;
	}

	if (!clonner::initialize()) {
		return false;
	}
	
	//	return ErrCode == ST_NO_ERROR;
	return true;
}

static ST_ErrorCode_t EVT_Term1(void)	//	HACK: Force shutdown!!!
{
	/* If there is no event support, do nothing */
	/* ======================================== */
#if !defined(EVT_MAX_NUMBER)
#else

	/* Do the event term */
	/* ================= */
	ST_ErrorCode_t ErrCode=ST_NO_ERROR;
	U32            i;

	/* Terminate all the event handlers */
	/* ================================ */
	for (i=(EVT_MAX_NUMBER-1);i!=0xFFFFFFFF;i--)
	{
		STEVT_TermParams_t EVT_TermParams;

		/* Terminate STEVT */
		/* --------------- */
		memset(&EVT_TermParams,0,sizeof(STEVT_TermParams_t));
		EVT_TermParams.ForceTerminate=TRUE;
		ErrCode=STEVT_Term(EVT_DeviceName[i],&EVT_TermParams);
		if ((ErrCode!=ST_NO_ERROR)&&(ErrCode!=ST_ERROR_DEVICE_BUSY))
		{
			LWARN("st", "Unable to terminate event handler" );
			return(ErrCode);
		}
	}
#endif

	/* Return no errors */
	/* ================ */
	return(ST_NO_ERROR);
}

void STSDK_Term(U32 SDK_Mode) {
	ST_ErrorCode_t ErrCode=ST_NO_ERROR;
	U32            ErrDriverLayer[2]={0,0},ErrMiddleLayer[2]={0,0},ErrModuleLayer[2]={0,0},ErrAppliLayer[2]={0,0};

	clonner::finalize();

	//	Terminate application layer
	if (SDK_Mode&SDK_TERM_APPLICATION_LAYER) {
		if ( GUI_Term     ( )!= ST_NO_ERROR )             ErrAppliLayer[0] |= (1<< 0);  /* Terminate the GUI interface            */
	}

	// /* Terminate module layer */
	// if (SDK_Mode&SDK_TERM_MODULE_LAYER)	{
	// 	if ( MODULES_Term ( )!= ST_NO_ERROR )             ErrModuleLayer[0] |= (1<< 0); /* Terminate the modules                  */
	// }

	/* Terminate middle layer */
	if (SDK_Mode&SDK_TERM_MIDDLE_LAYER)	{
		if ( SUBT_Term    ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 3); /* Terminate the Subtitle driver          */
		if ( PLAYREC_Term ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 2); /* Terminate the play and record manager  */
		if ( VFS_Term     ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 1); /* Terminate the file system              */
		if ( TCPIP_Term   ( )!= ST_NO_ERROR )             ErrMiddleLayer[0] |= (1<< 0); /* Terminate TCP/IP stack                 */
	}

	/* Terminate driver layer */
	if (SDK_Mode&SDK_TERM_DRIVER_LAYER)	{
		//if ( GRAFIX_DFBTerm ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 11);/* Terminate the GRAFIX-DirectFB interface */
		if ( PCCRD_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 10);/* Terminate the PCCRD interface           */
		if ( MOCA_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 9); /* Terminate the MOCA interface            */
		if ( POD_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 8); /* Terminate the POD interface             */
		if ( CC_Term        ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 7); /* Terminate the Close caption driver      */
		if ( TTX_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 6); /* Terminate the Teletext driver           */
		if ( VIN_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 5); /* Terminate the VIN interface             */
		if ( KEYSCN_Term    ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 4); /* Terminate the KEYSCAN interface         */
		if ( HDMI_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 3); /* Terminate the HDMI interface            */
		if ( SMART_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 2); /* Terminate the Smartcard interface       */
		if ( ATAPI_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 1); /* Terminate the ATAPI interface           */
		if ( CRYPT_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[1] |= (1<< 0); /* Terminate the CRYPT interface           */
		if ( GAMLOAD_Term   ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<29); /* Terminate the GAM loader                */
		if ( USB_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<28); /* Terminate the USB interface             */
		//if ( BLAST_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<27); /* Terminate the Blast driver              */
		if ( AUD_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<26); /* Terminate the Audio drivers             */
		if ( VID_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<25); /* Terminate the Video drivers             */
		if ( VMIX_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<24); /* Terminate the Mixers                    */
		if ( GRAFIX_Term    ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<23); /* Terminate the Graphic layers            */
		if ( BLIT_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<22); /* Terminate the Bitter                    */
		if ( VBI_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<21); /* Terminate the VBI interface             */
		if ( LAYER_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<20); /* Terminate the Video layers              */
		if ( VOUT_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<19); /* Terminate the VOS output                */
		if ( VTG_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<18); /* Terminate the VTG time generators       */
		if ( DENC_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<17); /* Terminate the DENC output               */
		if ( CLKRV_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<16); /* Terminate the Clock recovery            */
		if ( INJECT_Term    ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<15); /* Terminate the Inject interface          */
		if ( DEMUX_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<14); /* Terminate the Demux (Merger+Pti)        */
		if ( DMA_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<13); /* Terminate the DMA engines               */
		if ( TUNER_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<12); /* Terminate the Tuners                    */
		if ( SCART_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<11); /* Terminate the SCARTs                    */
		if ( PCPD_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<<10); /* Terminate the PCPD driver               */
		if ( FLASH_Term     ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 9); /* Terminate the Flash drivers             */
		if ( PWM_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 8); /* Terminate the PWM                       */
		if ( SPI_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 7); /* Terminate the SPI interface             */
		if ( I2C_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 6); /* Terminate the I2C ports                 */
		if ( EVT_Term1      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 5); /* Terminate the Event handler             */
		if ( TRACE_Term     (TRACE_UART_ID)!= ST_NO_ERROR ) ErrDriverLayer[0] |= (1<< 4); /* Terminate the Trace driver              */
		if ( TBX_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 3); /* Terminate the STAPI Trace module        */
		if ( UART_Term      ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 2); /* Terminate the Uart ports                */
		if ( PIO_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 1); /* Terminate the Pio pins                  */
		if ( SYS_Term       ( )!= ST_NO_ERROR )             ErrDriverLayer[0] |= (1<< 0); /* Kernel + memory + cache + system clocks */
	}

	/* Check terminate */
	if ((ErrAppliLayer[0]!=0)||(ErrAppliLayer[1]!=0)) {
		ErrCode|=SDK_TERM_APPLICATION_LAYER_FAILED;
		LWARN("st", "Errors encountered during terminate of the appli  layer, code = 0x%08x%08x failure summary:",ErrAppliLayer[1],ErrAppliLayer[0]);
		if (ErrAppliLayer[0] & (1<< 0)) LWARN("st", "\t- GUI_Term() Failure");
	}
	
	if ((ErrModuleLayer[0]!=0)||(ErrModuleLayer[1]!=0)) {
		ErrCode|=SDK_TERM_MODULE_LAYER_FAILED;
		LWARN("st", "Errors encountered during terminate of the module layer, code = 0x%08x%08x failure summary:",ErrModuleLayer[1],ErrModuleLayer[0]);
		if (ErrModuleLayer[0] & (1<< 0)) LWARN("st", "\t- MODULES_Term() Failure");
	}
	
	if ((ErrMiddleLayer[0]!=0)||(ErrMiddleLayer[1]!=0)) {
		ErrCode|=SDK_TERM_MIDDLE_LAYER_FAILED;
		LWARN("st", "Errors encountered during terminate of the middle layer, code = 0x%08x%08x failure summary:",ErrMiddleLayer[1],ErrMiddleLayer[0]);
		if (ErrMiddleLayer[0] & (1<< 0)) LWARN("st", "\t- TCPIP_Term() Failure");
		if (ErrMiddleLayer[0] & (1<< 1)) LWARN("st", "\t- VFS_Term() Failure");
		if (ErrMiddleLayer[0] & (1<< 2)) LWARN("st", "\t- PLAYREC_Term() Failure");
		if (ErrMiddleLayer[0] & (1<< 3)) LWARN("st", "\t- SUBT_Term() Failure");
	}
	
	if ((ErrDriverLayer[0]!=0)||(ErrDriverLayer[1]!=0)) {
		ErrCode|=SDK_TERM_DRIVER_LAYER_FAILED;
		LWARN("st", "Errors encountered during terminate of the driver layer, code = 0x%08x%08x failure summary:", ErrDriverLayer[1], ErrDriverLayer[0]);
		if (ErrDriverLayer[1] & (1<<11)) LWARN("st", "\t- GRAFIX_DFBTerm() Failure");
		if (ErrDriverLayer[1] & (1<<10)) LWARN("st", "\t- PCCRD_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 9)) LWARN("st", "\t- MOCA_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 8)) LWARN("st", "\t- POD_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 7)) LWARN("st", "\t- CC_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 6)) LWARN("st", "\t- TTX_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 5)) LWARN("st", "\t- VIN_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 4)) LWARN("st", "\t- KEYSCN_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 3)) LWARN("st", "\t- HDMI_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 2)) LWARN("st", "\t- SMART_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 1)) LWARN("st", "\t- ATAPI_Term() Failure");
		if (ErrDriverLayer[1] & (1<< 0)) LWARN("st", "\t- CRYPT_Term() Failure");
		if (ErrDriverLayer[0] & (1<<29)) LWARN("st", "\t- GAMLOAD_Term() Failure");
		if (ErrDriverLayer[0] & (1<<28)) LWARN("st", "\t- USB_Term() Failure");
		if (ErrDriverLayer[0] & (1<<27)) LWARN("st", "\t- BLAST_Term() Failure");
		if (ErrDriverLayer[0] & (1<<26)) LWARN("st", "\t- AUD_Term() Failure");
		if (ErrDriverLayer[0] & (1<<25)) LWARN("st", "\t- VID_Term() Failure");
		if (ErrDriverLayer[0] & (1<<24)) LWARN("st", "\t- VMIX_Term() Failure");
		if (ErrDriverLayer[0] & (1<<23)) LWARN("st", "\t- GRAFIX_Term() Failure");
		if (ErrDriverLayer[0] & (1<<22)) LWARN("st", "\t- BLIT_Term() Failure");
		if (ErrDriverLayer[0] & (1<<21)) LWARN("st", "\t- VBI_Term() Failure");
		if (ErrDriverLayer[0] & (1<<20)) LWARN("st", "\t- LAYER_Term() Failure");
		if (ErrDriverLayer[0] & (1<<19)) LWARN("st", "\t- VOUT_Term() Failure");
		if (ErrDriverLayer[0] & (1<<18)) LWARN("st", "\t- VTG_Term() Failure");
		if (ErrDriverLayer[0] & (1<<17)) LWARN("st", "\t- DENC_Term() Failure");
		if (ErrDriverLayer[0] & (1<<16)) LWARN("st", "\t- CLKRV_Term() Failure");
		if (ErrDriverLayer[0] & (1<<15)) LWARN("st", "\t- INJECT_Term() Failure");
		if (ErrDriverLayer[0] & (1<<14)) LWARN("st", "\t- DEMUX_Term() Failure");
		if (ErrDriverLayer[0] & (1<<13)) LWARN("st", "\t- DMA_Term() Failure");
		if (ErrDriverLayer[0] & (1<<12)) LWARN("st", "\t- TUNER_Term() Failure");
		if (ErrDriverLayer[0] & (1<<11)) LWARN("st", "\t- SCART_Term() Failure");
		if (ErrDriverLayer[0] & (1<<10)) LWARN("st", "\t- PCPD_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 9)) LWARN("st", "\t- FLASH_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 8)) LWARN("st", "\t- PWM_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 7)) LWARN("st", "\t- SPI_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 6)) LWARN("st", "\t- I2C_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 5)) LWARN("st", "\t- EVT_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 4)) LWARN("st", "\t- TRACE_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 3)) LWARN("st", "\t- TBX_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 2)) LWARN("st", "\t- UART_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 1)) LWARN("st", "\t- PIO_Term() Failure");
		if (ErrDriverLayer[0] & (1<< 0)) LWARN("st", "\t- SYS_Term() Failure");
	}
}

bool STSDK_Init() {
	return STSDK_Init(
		SDK_INIT_DRIVER_LAYER |
		SDK_INIT_MIDDLE_LAYER |
		SDK_INIT_MODULE_LAYER |
		SDK_INIT_APPLICATION_LAYER
	);
}

void STSDK_Term() {
#ifndef ST_7109
	STSDK_Term(
		SDK_TERM_DRIVER_LAYER |
		SDK_TERM_MIDDLE_LAYER |
		SDK_TERM_MODULE_LAYER |
		SDK_TERM_APPLICATION_LAYER
	);
#endif
}

bool setResolution( int modeHD, int modeSD ) {
	if (VTG_SetMode( VTG_HD_OUTPUT, (STVTG_TimingMode_t)modeHD ) != ST_NO_ERROR) {
		LERROR( "st", "Cannot set resolution on HD VTG" );
		return false;
	}

	if (VTG_SetMode( VTG_SD_OUTPUT, (STVTG_TimingMode_t)modeSD ) != ST_NO_ERROR) {
		LERROR( "st", "Cannot set resolution on SD VTG" );
		return false;
	}
	
	return true;
}

//	HACK: Mute state
void setMuteState( bool state ) {
	impl::isMuted = state;
}

bool isMuted() {
	return impl::isMuted;
}

bool isResetButtonPressed() {

#ifdef MT_742PV
	//	Check reset button status
	int reset_button_status;
	if(PIO_BIT_Get(6, 5, &reset_button_status) != ST_NO_ERROR) {
		LERROR("st", "Cannot read PIO status");
		return false;
	}
	return reset_button_status ? true : false;
#else
	return false;
#endif
}

}
