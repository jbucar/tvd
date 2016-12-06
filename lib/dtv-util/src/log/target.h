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

#pragma once

#include <string>
#include <queue>
#include <boost/thread.hpp>

namespace util {
namespace log {

class Formatter;
class Message;

class Target {
public :
	Target();
	virtual ~Target();

	void fin();
	void receive( const Message &message );
	void flush();

protected:
	void loggerThread();
	virtual void output( const std::string &line ) = 0;
	virtual void flushImpl();
	virtual bool useColors();

	Formatter *_formatter;

	std::queue<std::string> _lines;
	boost::mutex _linesMutex;
	boost::condition_variable _linesCond;

	bool _waitingFlush;
	boost::mutex _flushMutex;
	boost::condition_variable _flushCond;

	bool _fin;
	boost::mutex _finMutex;

	boost::thread *_thread;
};

class Cout : public Target {
public:
	explicit Cout( FILE *out );
	virtual void output( const std::string &line );

	virtual bool useColors();

private:
	FILE *_out;
};

class LogFile : public Target {
public:
	LogFile( const std::string &fileName );
	virtual ~LogFile();

	virtual void output( const std::string &line );

protected:
	void open();
	void flushImpl();

	std::string _fileName;
	FILE *_file;

private:
	LogFile();
};

}
}
