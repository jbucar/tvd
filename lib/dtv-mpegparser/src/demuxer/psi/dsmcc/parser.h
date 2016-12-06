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

#include "../psi.h"

//  DSMCC generic macros for dsmccDownloadDataHeader and dsmccMessageHeader
#define DSMCC_DISCRIMINATOR(ptr)    GET_BYTE(ptr)
#define DSMCC_TYPE(ptr)             GET_BYTE((ptr)+1)
#define DSMCC_ID(ptr)               GET_WORD((ptr)+2)
#define DSMCC_RESERVED(ptr)         GET_BYTE((ptr)+8)
#define DSMCC_ADAPTATION_LEN(ptr)   GET_BYTE((ptr)+9)
#define DSMCC_LEN(ptr)              GET_WORD((ptr)+10)

//	DSMCC for dsmccDownloadDataHeader
#define DSMCC_DOWNLOAD_ID(ptr)      GET_DWORD((ptr)+4)

//	DSMCC for dsmccMessageHeader
#define DSMCC_TRANSACTION_ID(ptr)   GET_DWORD((ptr)+4)


//  DSMCC Aux header
#define DSMCC_GENERIC_HEADER         12

//  DSM-CC dsmccType values
#define DSMCC_TYPE_RESERVED          0x00   //  ISO/IEC 13818-6 Reserved
#define DSMCC_TYPE_UNCM              0x01   //  User-to-Network Configuration Message.
#define DSMCC_TYPE_UNSM              0x02   //  User-to-Network Session Message.
#define DSMCC_TYPE_DOWNLOAD          0x03   //  Download message.
#define DSMCC_TYPE_CCP               0x04   //  SDB Channel Change Protocol message.
#define DSMCC_TYPE_UNPASSTHRU        0x05   //  User-to- Network pass-thru message.
#define DSMCC_TYPE_PRIVATE           0x80   //  User Defined message type.

//  DSM-CC Download messageID assignements
#define DSMCC_MSGID_DIR              0x1001 //  Download Info Request
#define DSMCC_MSGID_DII              0x1002 //  Download Info Indication
#define DSMCC_MSGID_DDB              0x1003 //  Download Data Block
#define DSMCC_MSGID_DDR              0x1004 //  Download Data Request
#define DSMCC_MSGID_DC               0x1005 //  Download Cancel
#define DSMCC_MSGID_DSI              0x1006 //  Download Server Initiate

//  DSM-CC Stream Event Descriptors
#define DSMCC_SE_NPT_REFERENCE       0x01   //  NPT Reference descriptor
#define DSMCC_SE_NPT_ENDPOINT        0x02   //  NPT Endpoint descriptor
#define DSMCC_SE_STREAM_MODE         0x03   //  Stream Mode descriptor
#define DSMCC_SE_STREAM_EVENT        0x04   //  Stream Event descriptor
#define DSMCC_SE_GINGA_EDITCMD       0x1a   //  Ginga Editing Command descriptor

//	DSM-CC Transaction ID macros (DVB BlueBook A068 Rev.3 Annex B, Table B.34)
#define DSMCC_TID_UPDATE             0x00000001
#define DSMCC_CM_UPDATED(tID)        ((tID) & DSMCC_TID_UPDATE)
#define DSMCC_CM_IDENT(tID)          ((tID) & 0x0000FFFE)
#define DSMCC_CM_VERSION(tID)        ((tID) & 0x3FFF0000)
#define DSMCC_CM_ORIGINATOR(tID)     ((tID) & 0xC0000000)

//  DSM-CC Stream Event macros
#define GET_SE_TAG(section)          GET_BYTE(section);
#define GET_SE_LEN(section)          GET_BYTE((section)+1);
