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
#include "statics.h"
#include "../assert.h"
#include <stdio.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace util {
namespace log {

Target::Target() {
	_formatter = Statics::getFormatter();
	_fin = false;
	_waitingFlush = false;
	_thread = new boost::thread(boost::bind( &Target::loggerThread, this ));
}

Target::~Target() {
}

bool Target::useColors() {
	return false;
}

void Target::receive( const Message &message ) {
	_linesMutex.lock();
	_lines.push(_formatter->format(message, useColors()));
	_linesMutex.unlock();
}

void Target::flush() {
	boost::unique_lock<boost::mutex> lock(_flushMutex);
	_waitingFlush = true;
	_linesCond.notify_one();
	_flushCond.wait(lock);
	flushImpl();
	_waitingFlush = false;
}

void Target::flushImpl() {
}

void Target::loggerThread() {
	_finMutex.lock();
	bool fin = _fin;
	_finMutex.unlock();

	while (!fin) {
		std::vector<std::string> localLines;
		{
			boost::unique_lock<boost::mutex> lock(_linesMutex);
			if(_lines.size()<1) {
				_linesCond.timed_wait(lock, boost::posix_time::milliseconds(100));
			}
			while (_lines.size()>0) {
				localLines.push_back(_lines.front());
				_lines.pop();
			}
		}
		BOOST_FOREACH(std::string l, localLines) {
			output( l );
		}
		localLines.clear();

		_flushMutex.lock();
		if (_waitingFlush) {
			_flushCond.notify_one();
		}
		_flushMutex.unlock();

		_finMutex.lock();
		fin = _fin;
		_finMutex.unlock();
	}
	while (_lines.size()>0) {
		output(_lines.front());
		_lines.pop();
	}
}

void Target::fin() {
	_finMutex.lock();
	_fin = true;
	_finMutex.unlock();
	_linesCond.notify_one();

	_thread->join();
	delete _thread;
	_thread = NULL;
}

Cout::Cout( FILE *out )
	: _out(out)
{
	DTV_ASSERT(out);
}

bool Cout::useColors() {
#ifndef _WIN32
	return true;
#else
	return false;
#endif
}

void Cout::output( const std::string &line ) {
	fprintf( _out, "%s", line.c_str() );
}

LogFile::LogFile( const std::string &fileName ) {
	_file = NULL;
	_fileName = fileName;
}

LogFile::~LogFile() {
	if (_file) {
		char cTimeString[100];
		time_t tRawTime;
		struct tm *tmInfo;

		time( &tRawTime );
		tmInfo = localtime( &tRawTime );
		strftime(cTimeString, 100, "%c", tmInfo);

		std::string msg("Log ended at: ");
		msg += cTimeString;
		msg += "\n";
		fwrite( msg.c_str(), sizeof( char ), msg.size(), _file );
		fclose( _file );
		_file = NULL;
	}
}

void LogFile::open() {
	_file = fopen( _fileName.c_str(), "w" );
	if (_file) {
		size_t result;

		char cTimeString[100];
		time_t tRawTime;
		struct tm *tmInfo;

		time( &tRawTime );
		tmInfo = localtime( &tRawTime );
		strftime(cTimeString, 100, "%c", tmInfo);

		std::string msg("Log started at: ");
		msg += cTimeString;
		msg += "\n";
		result = fwrite( msg.c_str(), sizeof( char ), msg.size(), _file );
		if (!result) {
			fclose( _file );
			_file = NULL;
		}
	}
}

void LogFile::output( const std::string &line ) {
	if (!_file) {
		open();
	}
	if (_file) {
		fwrite( line.c_str(), sizeof( char ), line.size(), _file );
	}
}

void LogFile::flushImpl() {
	DTV_ASSERT(_file);
	fflush(_file);
}

}
}
