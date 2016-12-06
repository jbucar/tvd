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

#include "types.h"
#include <string>
#include <set>

namespace util {

void removeDirectory( const std::string &dir );
bool safeRemoveDirectory( const std::string &dir );

void cleanDirectory( const std::string &dir );
bool safeCleanDirectory( const std::string &dir );

bool getMountedFilesystems( std::set<std::string> &paths );

namespace fs {

//	The user home the directory
const std::string home();

//	The root of the directory tree for read-only architecture-independent
const std::string installRootDir();

//	The root of the directory tree for read-only architecture-independent data files.
const std::string installDataDir();

//	The directory for installing read-only data files that pertain to a single machine–that is to say, files for configuring a host.
const std::string sysConfDir();

//	The directory for data files which the programs modify while they run
const std::string stateDir();

//	Log directory
const std::string logDir();

//	Temp directory
const std::string tmpDir();

//	Make name from parts
std::string make( const std::string &p1, const std::string &p2 );
std::string make( const std::string &p1, const std::string &p2, const std::string &p3 );

}	//	namespace fs

}

