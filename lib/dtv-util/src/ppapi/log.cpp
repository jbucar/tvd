/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../assert.h"
#include "../log.h"
#include "generated/config.h"

namespace util {
namespace log {

#if (LOG_USE_PPAPI == 1)
//	TODO: Check if we need log to javascript console .....

namespace impl {

static PP_LogLevel converter[] = {
	/* LOG_LEVEL_UNKNOWN */ PP_LOGLEVEL_TIP,
	/* LOG_LEVEL_NONE	 */ PP_LOGLEVEL_TIP,
	/* LOG_LEVEL_ERROR	 */ PP_LOGLEVEL_ERROR,
	/* LOG_LEVEL_WARN	 */ PP_LOGLEVEL_WARNING,
	/* LOG_LEVEL_INFO	 */ PP_LOGLEVEL_LOG,
	/* LOG_LEVEL_DEBUG	 */ PP_LOGLEVEL_LOG,
	/* LOG_LEVEL_TRACE	 */ PP_LOGLEVEL_LOG,
	/* LOG_LEVEL_ALL	 */ PP_LOGLEVEL_LOG
};
static LOG_LEVEL_TYPE logLevel = -1;

}	//	namespace impl

void init( const std::string & /*config*/ ) {}
void flush() {}
void fin() {}

void setLevel( const char *group, const char * /*category*/, const char *level ) {
	const std::string &grp = group;
	if (grp == "all") {
		impl::logLevel = getLevelValue(level);
	}
}

bool canLog( LOG_LEVEL_TYPE level, const char * /*group*/, const char * /*category*/ ) {
	return level <= impl::logLevel;
}

void log( LOG_LEVEL_TYPE level, const char *group, const char *category, const char *format, ... ) {
	const PPB_Console* console = reinterpret_cast<const PPB_Console*>( pp::Module::Get()->GetBrowserInterface(PPB_CONSOLE_INTERFACE) );
	if (console && level <= LOG_LEVEL_ALL) {
		PP_LogLevel prio = impl::converter[level];
		va_list args;
		va_start(args, format);
		std::string formatedMsg = format_va( format, args );
		va_end( args );
		char buf[1024];
		snprintf( buf, 1024, "[%s::%s] %s", group, category, formatedMsg.c_str() );
		pp::Var b(buf);
		pp::Var src("");
		console->Log( 0, prio, b.pp_var() );
		console->LogWithSource( 0, prio, src.pp_var(), b.pp_var() );
	}
}

#endif	//	#if LOG_USE_PPAPI == 1

}	//	namespace log
}	//	namespace util

