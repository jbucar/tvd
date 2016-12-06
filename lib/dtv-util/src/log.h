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

#pragma once

#include <string>

/**
 * @file dtv-util/src/log.h
 * @namespace util::log
 * @brief Funciones y macros de logging.
 * 
 * Este archivo tiene como función agrupar a las macros utilizadas para hacer registro de eventos. 
 * Yendo desde LTRACE a LERROR cada macro va aumentando su importancia al momento de registrarse.
 */

#define LOG_LEVEL_TYPE	unsigned int

#define LOG_LEVEL_UNKNOWN 0
#define LOG_LEVEL_NONE    1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_WARN    3
#define LOG_LEVEL_INFO    4
#define LOG_LEVEL_DEBUG   5
#define LOG_LEVEL_TRACE   6
#define LOG_LEVEL_ALL     7

#define LOG_PUBLISH( l, g, c, f, ... ) \
	if (::util::log::canLog( LOG_LEVEL_##l, g, c )) { \
		::util::log::log( LOG_LEVEL_##l, g, c, f, ##__VA_ARGS__ ); \
	}

#define TO_STR( x ) (#x)
#define TO_STR_IND( x ) TO_STR(x)

#define LTRACE( cat, msg, ... ) LOG_PUBLISH( TRACE, TO_STR_IND(PROJECT_NAME), cat, msg, ##__VA_ARGS__ )
#define LDEBUG( cat, msg, ... ) LOG_PUBLISH( DEBUG, TO_STR_IND(PROJECT_NAME), cat, msg, ##__VA_ARGS__ )
#define LINFO( cat, msg, ... )  LOG_PUBLISH( INFO,  TO_STR_IND(PROJECT_NAME), cat, msg, ##__VA_ARGS__ )
#define LWARN( cat, msg, ... )  LOG_PUBLISH( WARN,  TO_STR_IND(PROJECT_NAME), cat, msg, ##__VA_ARGS__ )
#define LERROR( cat, msg, ... ) LOG_PUBLISH( ERROR, TO_STR_IND(PROJECT_NAME), cat, msg, ##__VA_ARGS__ )

namespace util {
namespace log {

void init( const std::string &config="" );
void flush();
void fin();
void setLevel( const char *group, const char *category, const char *level );
bool canLog( LOG_LEVEL_TYPE level, const char *group, const char *category );
void log( LOG_LEVEL_TYPE level, const char *group, const char *category, const char *format, ... );
LOG_LEVEL_TYPE getLevelValue( const std::string &levelstr );

}
}

