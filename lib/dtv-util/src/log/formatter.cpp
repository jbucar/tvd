/*******************************************************************************

  logog is Copyright (c) 2011, Gigantic Software.

********************************************************************************

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

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

#include "formatter.h"
#include "message.h"

namespace util {
namespace log {

Formatter::Formatter() {
	_buffer.reserve(1024 * 8);
}

std::string &Formatter::format( const Message &message, bool useColors/*=false*/ ) {
	_buffer.clear();
	if (useColors) {
		_buffer.append(colorForLevel(message.level()));
	}
	if (!message.group().empty()) {
		_buffer.append( "[" );
		_buffer.append( message.group() );
		if (!message.category().empty()) {
			_buffer.append( "::" );
			_buffer.append( message.category() );
		}
		_buffer.append( "] " );
	}
	_buffer.append( errorDescription( message.level()) );
	if (useColors) {
		_buffer.append("\x1b[0m");
	}
	_buffer.append( message.message() );
	_buffer.append( "\n" );
	return _buffer;
}

const char *Formatter::colorForLevel( const LOG_LEVEL_TYPE level ) {
	if ( level == LOG_LEVEL_ERROR ) {
		return "\x1b[31m";
	} else if ( level == LOG_LEVEL_WARN ) {
		return "\x1b[33m";
	} else if ( level == LOG_LEVEL_DEBUG ) {
		return "\x1b[36m";
	} else if ( level == LOG_LEVEL_TRACE ) {
		return "\x1b[37m";
	} else {
		return "\x1b[32m";
	}
}

const char *Formatter::errorDescription( const LOG_LEVEL_TYPE level ) {
	if ( level == LOG_LEVEL_ERROR ) {
		return "ERROR: ";
	} else if ( level == LOG_LEVEL_WARN ) {
		return "WARNING: ";
	} else if ( level == LOG_LEVEL_DEBUG ) {
		return "DEBUG: ";
	} else if ( level == LOG_LEVEL_TRACE ) {
		return "TRACE: ";
	} else {
		return "";
	}
}

}
}
