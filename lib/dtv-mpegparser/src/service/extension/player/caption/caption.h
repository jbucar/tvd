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

//	Control Codes
#define CTRL_CODE_SP    0x20
#define CTRL_CODE_DEL   0x7f

#define CTRL_CODE_NULL  0x00
#define CTRL_CODE_BEL   0x07
#define CTRL_CODE_APB   0x08
#define CTRL_CODE_APF   0x09
#define CTRL_CODE_APD   0x0a
#define CTRL_CODE_APU   0x0b
#define CTRL_CODE_CS    0x0c
#define CTRL_CODE_APR   0x0d
#define CTRL_CODE_LS1   0x0e
#define CTRL_CODE_LS0   0x0f
#define CTRL_CODE_PAPF  0x16
#define CTRL_CODE_CAN   0x18
#define CTRL_CODE_SS2   0x19
#define CTRL_CODE_ESC   0x1b
#define CTRL_CODE_APS   0x1c
#define CTRL_CODE_SS3   0x1d
#define CTRL_CODE_RS    0x1e
#define CTRL_CODE_US    0x1f
#define CTRL_CODE_BKF   0x80
#define CTRL_CODE_RDF   0x81
#define CTRL_CODE_GRF   0x82
#define CTRL_CODE_YLF   0x83
#define CTRL_CODE_BLF   0x84
#define CTRL_CODE_MGF   0x85
#define CTRL_CODE_CNF   0x86
#define CTRL_CODE_WHF   0x87
#define CTRL_CODE_SSZ   0x88
#define CTRL_CODE_MSZ   0x89
#define CTRL_CODE_NSZ   0x8a
#define CTRL_CODE_SZX   0x8b
#define CTRL_CODE_COL   0x90
#define CTRL_CODE_FLC   0x91
#define CTRL_CODE_CDC   0x92
#define CTRL_CODE_POL   0x93
#define CTRL_CODE_WMM   0x94
#define CTRL_CODE_MACRO 0x95
#define CTRL_CODE_HLC   0x97
#define CTRL_CODE_RPC   0x98
#define CTRL_CODE_SPL   0x99
#define CTRL_CODE_STL   0x9a
#define CTRL_CODE_CSI   0x9b
#define CTRL_CODE_TIME  0x9d

//	Extension Control Code (CSI)
#define CSI_MAX_PARAMS 5
#define CSI_I1   0x3B
#define CSI_I2   0x20

#define CSI_GSM  0x42
#define CSI_SWF  0x53
#define CSI_CCC  0x54
#define CSI_SDF  0x56
#define CSI_SSM  0x57
#define CSI_SHS  0x58
#define CSI_SVS  0x59
#define CSI_PLD  0x5B
#define CSI_PLU  0x5C
#define CSI_GAA  0x5D
#define CSI_SRC  0x5E
#define CSI_SDP  0x5F
#define CSI_ACPS 0x61
#define CSI_TCC  0x62
#define CSI_ORN  0x63
#define CSI_MDF  0x64
#define CSI_CFS  0x65
#define CSI_XCS  0x66
#define CSI_PRA  0x68
#define CSI_ACS  0x69
#define CSI_RCS  0x6E
#define CSI_SCS  0x6F
