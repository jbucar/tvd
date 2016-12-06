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

#pragma once

#include <canvas/rect.h>

#ifndef DEFINED_U32
#define DEFINED_U32
typedef unsigned int   U32;
#endif

#ifndef DEFINED_S32
#define DEFINED_S32
typedef signed int   S32;
#endif

typedef U32 STPTI_Handle_t;
typedef STPTI_Handle_t STPTI_Buffer_t;
typedef STPTI_Handle_t STPTI_Signal_t;
typedef STPTI_Handle_t STPTI_Slot_t;
struct STPTI_SlotData_s;
typedef struct STPTI_SlotData_s STPTI_SlotData_t;
typedef STPTI_Handle_t STPTI_Filter_t;

#define AUDIO_ID 0
#define AUDIO_INPUT_ID 0

#define VTG_HD_OUTPUT 0
#define VTG_SD_OUTPUT 1

namespace st {

bool STSDK_Init();
void STSDK_Term();

//	Common methods
bool setResolution( int modeHD, int modeSD );

//	HACK: Mute state
void setMuteState( bool st );
bool isMuted();

bool isResetButtonPressed();

}

