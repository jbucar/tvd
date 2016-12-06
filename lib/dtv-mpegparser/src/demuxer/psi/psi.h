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

#include "../ts.h"

//  PSI generic macros
#define PSI_TABLE(ptr)      GET_BYTE(ptr)
#define PSI_SYNTAX(ptr)     ((ptr)[1] & 0x80)
#define PSI_EXTENSION(ptr)  GET_WORD((ptr)+3)
#define PSI_LENGTH(ptr)     (GET_WORD((ptr)+1) & 0x0FFF)
#define PSI_VERSION(ptr)    util::BYTE(((ptr)[5] & 0x3E) >> 1)
#define PSI_NEXT(ptr)       ((ptr)[5] & 0x01)
#define PSI_NUMBER(ptr)     ((ptr)[6])
#define PSI_LAST(ptr)       ((ptr)[7])

#define GET_CRC(ptr)        GET_DWORD(ptr)

//  Aux psi header
#define PSI_GENERIC_HEADER         3
#define PSI_CRC_SIZE               4
#define PSI_STUFFING_BYTE          0xFF
#define PSI_SECTION_LENGTH(ptr)    (PSI_LENGTH(ptr)+PSI_GENERIC_HEADER)
#define PSI_PAYLOAD_OFFSET         8

//  Max sections bytes
#define TSS_ISO_MAX_BYTES       1024
#define TSS_PRI_MAX_BYTES       4096

//  Table ID assignements (ISO/IEC 13818-1, ETSI EN 300 468)
#define PSI_TID_FORBIDDEN       0xFF    //  Forbidden
#define PSI_TID_PAT             0x00    //  Program Association section
#define PSI_TID_CAT             0x01    //  Conditional Access section
#define PSI_TID_PMT             0x02    //  Program Map section
#define PSI_TID_NIT_ACTUAL      0x40    //	NIT actual network
#define PSI_TID_NIT_OTHER       0x41    //	NIT other networks
#define PSI_TID_SDT_ACTUAL      0x42    //	SDT actual network
#define PSI_TID_SDT_OTHER       0x46    //	SDT other network
#define PSI_TID_EIT_PF_ACTUAL   0x4E    //	EIT (Present and following program of actual stream)
#define PSI_TID_EIT_PF_OTHER    0x4F    //	EIT (Present and following program of others stream)
#define PSI_TID_EIT_SBF_ACTUAL  0x50    //	EIT (First event schedule basic information of actual stream)
#define PSI_TID_EIT_SBL_ACTUAL  0x57    //	EIT (Last event schedule basic information of actual stream)
#define PSI_TID_EIT_SEF_ACTUAL  0x58    //	EIT (First event schedule information of actual stream)
#define PSI_TID_EIT_SEL_ACTUAL  0x5F    //	EIT (Last event schedule information of actual stream)
#define PSI_TID_EIT_SBF_OTHER   0x60    //	EIT (First event schedule information of others stream)
#define PSI_TID_EIT_SBL_OTHER   0x67    //	EIT (Last event schedule information of others stream)
#define PSI_TID_EIT_SEF_OTHER   0x68    //	EIT (First event schedule information of others stream)
#define PSI_TID_EIT_SEL_OTHER   0x6F    //	EIT (Last event schedule information of others stream)
#define PSI_TID_TOT             0x73    //	TOT
#define PSI_TID_AIT             0x74    //	AIT
#define PSI_TID_SDTT            0xC3    //	SDTT
#define PSI_TID_CDT             0xC8    //	CDT
#define PSI_INVALID_VERSION     0xFF    //  Invalid version

//  Table ID assignements (ISO/IEC 13818-6, DSM-CC)
#define DSMCC_TID_MED           0x3A    //  Multi-protocol encapsulated data
#define DSMCC_TID_UNM           0x3B    //  U-N Messages, except DDM
#define DSMCC_TID_DDM           0x3C    //  Download Data Messages
#define DSMCC_TID_SD            0x3D    //  Stream Descriptors
#define DSMCC_TID_PD            0x3E    //  Private Data

//  Table ID assignements (ABNT NBR 15603-2)
#define ABNT_TID_FORBIDDEN      0xC8     // Adjusted: CDT Table Id is 0xC8

//	Network Identification coding (ETR 162)
#define NIT_ID_RESERVED         0x0000	//	Reserved

//  PMT Stream Type assignements
/*************************************************************************************************
0x00 "ITU-T | ISO/IEC Reserved"
0x01 "ISO/IEC 11172 Video
0x02 "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
0x03 "ISO/IEC 11172 Audio
0x04 "ISO/IEC 13818-3 Audio
0x05 "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
0x06 "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
0x07 "ISO/IEC 13522 MHEG
0x08 "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC
0x09 "ITU-T Rec. H.222.1
0x0A "ISO/IEC 13818-6 type A
0x0B "ISO/IEC 13818-6 type B
0x0C "ISO/IEC 13818-6 type C
0x0D "ISO/IEC 13818-6 type D
0x0E "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
0x0F "ISO/IEC 13818-7 Audio with ADTS transport syntax
0x10 "ISO/IEC 14496-2 Visual
0x11 "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1
0x12 "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
0x13 "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.
0x14 "ISO/IEC 13818-6 Synchronized Download Protocol
0x15-0x7F "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
0x80-0xFF "User Private
*************************************************************************************************/

//  PMT Stream Type assignements (ISO/IEC 13818-6)
#define PSI_ST_TYPE_AIT 0x05  //  AIT
#define PSI_ST_TYPE_A   0x0A  //  Multi-protocol Encapsulation
#define PSI_ST_TYPE_B   0x0B  //  DSM-CC U-N Messages
#define PSI_ST_TYPE_C   0x0C  //  DSM-CC Stream Descriptors
#define PSI_ST_TYPE_D   0x0D  //  DSM-CC Sections (any type, including private data)


/******************************************************************************

                                  PAT      CAT    PMT    Priv(1)   SDT      DSMCC
0    tableID                        8        8      8       8        8          8
1    section syntax indicator       1(=1)    1(=1)  1(=1)   1        1(=1)      1
1    '0'                            1        1      1       1        1          1
1    reserved1                      2        2      2       2        2          2
1,2  len                           12       12     12      12       12         12
3,4  table_id_extension            16       16     16      16       16         16
5    reserved2                      2        2      2       2        2          2
5    version                        5        5      5       5        5          5
5    current next indicator         1        1      1       1        1          1
6    section number                 8        8      8       8        8          8
7    last section number            8        8      8       8        8          8

(1) -> Private with section syntax indicator = 1

section_syntax_indicator – This is a 1-bit indicator. When set to '1', it indicates that the private section follows the
generic section syntax beyond the private_section_length field. When set to '0', it indicates that the private_data_bytes
immediately follow the private_section_length field.

Support for versions: Incremented by 1 modulo 32
******************************************************************************/

