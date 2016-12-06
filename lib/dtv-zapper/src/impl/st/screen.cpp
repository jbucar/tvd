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

#include "screen.h"
#include "st.h"
#include <util/log.h>
#include <util/assert.h>
#include <vector>

extern "C" {
#include "stapp_main.h"
}

namespace st {

static void showInfo() {
	LINFO( "st", "Video out: count=%d", VOUT_MAX_NUMBER ); //	Configure physical output
	LINFO( "st", "Video mix: count=%d", VMIX_MAX_NUMBER ); //	Configure video mixer (video/layer/etc)
	LINFO( "st", "VTG: count=%d", VTG_MAX_NUMBER );        //	Video Timing (for output/mpeg decoder)

	//	Get modes on All output
	for (int i=0; i<VTG_MAX_NUMBER; i++) {
		STVTG_TimingMode_t VTG_TimingMode;
		STVTG_ModeParams_t VTG_ModeParams;
		if (STVTG_GetMode(VTG_Handle[i], &VTG_TimingMode, &VTG_ModeParams) != ST_NO_ERROR) {
			LWARN("st", "Cannot get current aspect ratio from VTG: handle=%d", i);
			continue;
		}
		LINFO( "st", "VTG\t\tdevice=%d, resolution=(%d,%d)",
			i, VTG_ModeParams.ActiveAreaWidth, VTG_ModeParams.ActiveAreaHeight );
	}
}

Screen::Screen()
{
	showInfo();
}

Screen::~Screen()
{
}

bool Screen::turnOnImpl() {
	LINFO("st", "turnOn()");
	enableOutputs();
	return true;
}

bool Screen::turnOffImpl() {
	LINFO("st", "turnOff()");
	disableOutputs();
	return true;
}

void Screen::enableOutputs() {
	for (int i=0; i<VOUT_MAX_NUMBER; i++) {
		if (VOUT_Handle[i] && STVOUT_Enable( VOUT_Handle[i]) != ST_NO_ERROR ) {
			LWARN( "st", "Cannot enable video output: handle=%d", i );
			continue;
		}
	}
}

void Screen::disableOutputs() {
	for (int i=0; i<VOUT_MAX_NUMBER; i++) {
		if (VOUT_Handle[i] && STVOUT_Disable( VOUT_Handle[i]) != ST_NO_ERROR ) {
			LWARN( "st", "Cannot disable video output: handle=%d", i );
			continue;
		}
	}
}

// Connetor methods
canvas::connector::type Screen::defaultConnector() const {
	return canvas::connector::hdmi;
}

std::vector<canvas::connector::type> Screen::supportedConnectors() const {
	std::vector<canvas::connector::type> tmp;
	tmp.push_back( canvas::connector::composite );
	tmp.push_back( canvas::connector::component );
	tmp.push_back( canvas::connector::hdmi );
	return tmp;
}

// Mode methods
std::vector<canvas::mode::type> Screen::supportedModes( canvas::connector::type conn ) const {
	std::vector<canvas::mode::type> tmp;

	switch (conn) {
		case canvas::connector::composite:
		case canvas::connector::svideo: {
			tmp.push_back( canvas::mode::m576i_50 );
			tmp.push_back( canvas::mode::mNTSC );
			break;
		}
		case canvas::connector::component: {
			tmp.push_back( canvas::mode::mNTSC );
 			tmp.push_back( canvas::mode::m576i_50 );
 			tmp.push_back( canvas::mode::m720p_50 );
 			tmp.push_back( canvas::mode::m720p_60 );
			tmp.push_back( canvas::mode::m1080i_60 );
			tmp.push_back( canvas::mode::m1080i_50 );
			break;
		}
		case canvas::connector::hdmi: {
			tmp.push_back( canvas::mode::mNTSC );
 			tmp.push_back( canvas::mode::m576i_50 );
 			tmp.push_back( canvas::mode::m720p_50 );
 			tmp.push_back( canvas::mode::m720p_60 );
			tmp.push_back( canvas::mode::m1080i_60 );
			tmp.push_back( canvas::mode::m1080i_50 );
			tmp.push_back( canvas::mode::m1080p_60 );
			tmp.push_back( canvas::mode::m1080p_50 );
			break;
		}
		default: {
			LWARN("st", "invalid type of connector: conn=%02x", conn);
		}
	};
	return tmp;
}

bool Screen::modeImpl( canvas::connector::type /*conn*/, canvas::mode::type vMode ) {
	//	Ignore setup if connector is not active
	STVTG_TimingMode_t modeHD, modeSD;

	switch (vMode) {
		case canvas::mode::mNTSC: {	//	NTSC
			modeSD = STVTG_TIMING_MODE_480I60000_13514;
			modeHD = STVTG_TIMING_MODE_480P60000_27027;
			break;
		}
		case canvas::mode::m576i_50:	{    // 720x576-50i
			modeHD = STVTG_TIMING_MODE_576P50000_27000;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m720p_50:	{    // 1280x720-50
			modeHD = STVTG_TIMING_MODE_720P50000_74250;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m720p_60: {    // 1280x720-60
			modeHD = STVTG_TIMING_MODE_720P60000_74250;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m1080i_60: {    // 1920x1080-60i
			modeHD = STVTG_TIMING_MODE_1080I60000_74250;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m1080i_50: {    // 1920x1080-50i
			modeHD = STVTG_TIMING_MODE_1080I50000_74250_1;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m1080p_60: {    // 1920x1080-60
			modeHD = STVTG_TIMING_MODE_1080P60000_148500;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		case canvas::mode::m1080p_50: {    // 1920x1080-50
			modeHD = STVTG_TIMING_MODE_1080P50000_148500;
			modeSD = STVTG_TIMING_MODE_576I50000_13500;
			break;
		}
		default: {
			modeSD = modeHD = STVTG_TIMING_MODE_576I50000_13500;
		}
	};

	LDEBUG("st", "Set tvout display mode: HD=%d, SD=%d", modeHD, modeSD);

	//	Disable ALL outputs
	disableOutputs();

	//	Set resolution
	if (!setResolution( (int)modeHD, (int)modeSD )) {
		return false;
	}

	//	Enable ALL outputs
	enableOutputs();

	return true;
}

// Aspect methods
canvas::aspect::type Screen::defaultAspect() const {
	return canvas::aspect::automatic;
}

std::vector<canvas::aspect::type> Screen::supportedAspects() const {
	//	The function STVMIX_SetScreenParams does not work any more.
	//	The solution to change aspect ratio is only available for HDMI. It is not valid.
	std::vector<canvas::aspect::type> tmp;
	tmp.push_back( canvas::aspect::automatic );
	return tmp;
}

bool Screen::aspectImpl( canvas::aspect::type /*vAspect*/ ) {
	//STGXOBJ_AspectRatio_t ar;

	// switch (vAspect) {
	// 	case canvas::aspect::a4_3:
	// 		ar = STGXOBJ_ASPECT_RATIO_4TO3;
	// 		break;
	// 	case canvas::aspect::a16_9:
	// 		ar = STGXOBJ_ASPECT_RATIO_16TO9;
	// 		break;
	// 	default:
	// 		LWARN("st", "Aspect ratio not supported");
	// 		return false;
	// }

	// for (int i=0; i<VMIX_MAX_NUMBER; i++) {
	// 	aspect( i, ar );
	// }

	return true;
}

bool Screen::aspect( int vmixOut, int aspect ) {
	LDEBUG("st", "Set aspect ratio: vmixOut=%d, aspect=%d", vmixOut, aspect );

	//	Get current mode
	STVTG_TimingMode_t VTG_TimingMode;
	STVTG_ModeParams_t VTG_ModeParams;
	if (STVTG_GetMode(VTG_Handle[vmixOut], &VTG_TimingMode, &VTG_ModeParams) != ST_NO_ERROR) {
		LWARN("st", "Cannot get current aspect ratio from VTG: vmixOut=%d", vmixOut);
		return false;
	}

	//	Set aspect ratio
	STVMIX_ScreenParams_t ScreenParams;
	ScreenParams.AspectRatio = (STGXOBJ_AspectRatio_t)aspect;
	ScreenParams.ScanType    = VTG_ModeParams.ScanType;
	ScreenParams.FrameRate   = VTG_ModeParams.FrameRate;
	ScreenParams.Width       = VTG_ModeParams.ActiveAreaWidth;
	ScreenParams.Height      = VTG_ModeParams.ActiveAreaHeight;
	ScreenParams.XStart      = VTG_ModeParams.ActiveAreaXStart;
	ScreenParams.YStart      = VTG_ModeParams.ActiveAreaYStart;
	if (STVMIX_SetScreenParams(VMIX_Handle[vmixOut], &ScreenParams) != ST_NO_ERROR) {
		LWARN("st", "Cannot set aspect ratio: vmixOut=%d", vmixOut);
		return false;
	}

	return true;
}

}
