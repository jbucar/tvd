/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include <stddef.h>

#ifdef _WIN32
typedef void *HANDLE;
#else
#include <unistd.h>
#endif

//  Generic macros
#define GET_BYTE(ptr)           (util::BYTE)((ptr)[0])
#define GET_WORD(ptr)           (util::WORD)((GET_BYTE(ptr) << 8)  | GET_BYTE((ptr)+1))
#define GET_BE_WORD(ptr)        (util::WORD)((GET_BYTE((ptr)+1) << 8)  | GET_BYTE(ptr))
#define GET_DWORD(ptr)          (util::DWORD)((GET_WORD(ptr) << 16) | GET_WORD((ptr)+2))
#define GET_BE_DWORD(ptr)       (util::DWORD)((GET_BE_WORD((ptr)+2) << 16) | GET_BE_WORD(ptr))
#define GET_QWORD(ptr)          (util::QWORD)((util::QWORD(GET_DWORD(ptr)) << 32) | GET_DWORD((ptr)+4))

#define PUT_BYTE(value,ptr)           { (ptr)[0]=((util::BYTE) ((value) & 0xff)); }
#define PUT_WORD(value,ptr)           { PUT_BYTE((value >> 8), (ptr)+1); PUT_BYTE(value, (ptr)); }
#define PUT_BE_WORD(value,ptr)        { PUT_BYTE((value >> 8), ptr);     PUT_BYTE(value, (ptr)+1); }
#define PUT_DWORD(value,ptr)          { PUT_WORD((value >> 16),(ptr)+2); PUT_WORD(value, ptr); }
#define PUT_BE_DWORD(value,ptr)       { PUT_BE_WORD((value >> 16),ptr); PUT_BE_WORD(value, (ptr)+2); }
#define PUT_QWORD(value,ptr)          { PUT_DWORD((value >> 32),(ptr)+4); PUT_DWORD(value, ptr); }
#define PUT_BE_QWORD(value,ptr)       { PUT_BE_DWORD((value >> 32),ptr); PUT_BE_DWORD(value, (ptr)+4); }

#define RB(ptr,off)             GET_BYTE((ptr)+(off)); (off)++;
#define RW(ptr,off)             GET_WORD((ptr)+(off)); (off) += 2;
#define RDW(ptr,off)            GET_DWORD((ptr)+(off)); (off) += 4;
#define RQW(ptr,off)            GET_QWORD((ptr)+(off)); (off) += 8;

namespace util {

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef unsigned long long QWORD;

#ifdef _WIN32
typedef HANDLE DESCRIPTOR;
typedef unsigned int SOCKET;
typedef signed int SSIZE_T;
#else
typedef ssize_t SSIZE_T;
typedef	int SOCKET;
typedef int DESCRIPTOR;
#endif

}
