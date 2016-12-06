/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include <util/types.h>

//  TS fields
#define TS_PACKET_SIZE          188
#define TS_HEAD_SIZE            4
#define TS_PAYLOAD_SIZE         (TS_PACKET_SIZE-TS_HEAD_SIZE)
#define TS_SYNC                 0x47
#define TS_INVALID_CONTINUITY   0xFF
#define TS_HAS_ERROR(ptr)       ((ptr)[1] & 0x80)
#define TS_START(ptr)           ((ptr)[1] & 0x40)
#define TS_PRIORITY(ptr)        ((ptr)[1] & 0x20)
#define GET_PID(ptr)            (GET_WORD(ptr) & 0x1FFF)
#define TS_PID(ptr)             GET_PID(ptr+1)
#define TS_SCRAMBLING(ptr)      (((ptr)[3] & 0xC0) >> 6)
#define TS_HAS_ADAPTATION(ptr)  ((ptr)[3] & 0x20)
#define TS_HAS_PAYLOAD(ptr)     ((ptr)[3] & 0x10)
#define TS_CONTINUITY(ptr)      ((ptr)[3] & 0x0F)
#define INVALID_TS_ID           0xFF

//  Adaptation field
#define TSA_LEN(ptr)            ((ptr)[4])
#define TSA_FIELD(ptr)          ((ptr)[5])

//  Payload
#define TS_PAYLOAD_IS_PES(ptr)  (((ptr)[0] == 0) && ((ptr)[1] == 0) && (((ptr)[2]) == 1))
#define TS_PAYLOAD_IS_PSI(ptr)  ((((ptr)[2]) & 0xF0) == 0xB)

//  PID MPEG-2 (ISO 13818-1) tables
#define TS_PID_PAT              0x0000  //  Program Association Table
#define TS_PID_CAT              0x0001  //  Conditional Access Table
#define TS_PID_TSDT             0x0002  //  Transport Stream Description Table
#define TS_PID_SDTT_LOW         0x0023  //  Software Download Trigger Table - Low Protection Layer
#define TS_PID_SDTT_HIGH        0x0028  //  Software Download Trigger Table - High Protection Layer
#define TS_PID_NULL             0x1FFF  //  Null packet (needed for bitrate)

//  PID DVB Mandatory (EN300468) tables
#define TS_PID_NIT              0x0010  //  Network Information Table
#define TS_PID_SDT              0x0011  //  Service Descritor Table
#define TS_PID_EIT              0x0012  //  Actual Transport Stream
#define TS_PID_TOT              0x0014  //  Time Offset Table

//	PID STD-B10 5.1/ABNT NBR 15603-1
#define TS_PID_CDT              0x0029  //  Time Offset Table

