/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "log.h"
#include "generated/config.h"
#include <stdexcept>

namespace util {
namespace log {

LOG_LEVEL_TYPE getLevelValue( const std::string &levelstr ) {
	LOG_LEVEL_TYPE level;
	if (levelstr == "none") {
		level = LOG_LEVEL_NONE;
	} else if (levelstr == "error") {
		level = LOG_LEVEL_ERROR;
	} else if (levelstr == "warn") {
		level = LOG_LEVEL_WARN;
	} else if (levelstr == "info") {
		level = LOG_LEVEL_INFO;
	} else if (levelstr == "debug") {
		level = LOG_LEVEL_DEBUG;
	} else if (levelstr == "trace") {
		level = LOG_LEVEL_TRACE;
	} else if (levelstr == "all") {
		level = LOG_LEVEL_ALL;
	} else {
		throw std::runtime_error( "bad log severity name");
	}
	return level;
}

#if (LOG_USE_NONE == 1)

void init( const std::string & /*config*/ ) {}
void flush() {}
void fin() {}
void setLevel( const char * /*group*/, const char * /*category*/, const char * /*level*/ ) {}
bool canLog( LOG_LEVEL_TYPE /*level*/, const char * /*group*/, const char * /*category*/ ) { return false; }
void log( LOG_LEVEL_TYPE /*level*/, const char * /*group*/, const char * /*category*/, const char * /*format*/, ... ) {}

#endif

}
}
