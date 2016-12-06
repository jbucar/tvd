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

#include "statics.h"
#include "message.h"
#include "target.h"
#include "formatter.h"
#include <stdexcept>
#include <boost/foreach.hpp>

namespace util {
namespace log {

LOG_LEVEL_TYPE Statics::_defaultLevel;
Groups Statics::_groups;
std::vector<Target*> Statics::_outputs;
Formatter *Statics::_formatter;

bool Statics::_initialized = false;
boost::mutex Statics::_initializedMutex;

int Statics::_nTransmitting;
boost::mutex Statics::_nTransmittingMutex;
boost::condition_variable Statics::_nTransmittingCond;

void Statics::initStatics( LOG_LEVEL_TYPE level ) {
	Statics::_initializedMutex.lock();
	if (Statics::_initialized) {
		throw std::logic_error("Log library already initialized!");
	}
	Statics::_defaultLevel = level;
	Statics::_nTransmitting = 0;
	Statics::_groups["all"]["all"] = _defaultLevel;
	Statics::_formatter = new Formatter();
	Statics::_initialized = true;
	Statics::_initializedMutex.unlock();
}

void Statics::finStatics() {
	Statics::_initializedMutex.lock();
	{
		boost::unique_lock<boost::mutex> lock(Statics::_nTransmittingMutex);
		while (Statics::_nTransmitting) {
			Statics::_nTransmittingCond.wait(lock);
		}
	}
	if (!Statics::_initialized) {
		throw std::logic_error("Log library already finalized!");
	}
	Statics::_initialized = false;
	BOOST_FOREACH( Target *t, Statics::_outputs) {
		t->fin();
		delete t;
	}
	Statics::_outputs.clear();
	Statics::_groups.clear();
	delete Statics::_formatter;
	Statics::_formatter = NULL;
	Statics::_initializedMutex.unlock();
}

bool Statics::canLog( LOG_LEVEL_TYPE level, const std::string &group, const std::string &category ){
	LOG_LEVEL_TYPE curLev = Statics::_groups["all"]["all"];
	Groups::const_iterator group_it = Statics::_groups.find( group );
	if (group_it != Statics::_groups.end()) {
		Categories::const_iterator cat_it = group_it->second.find( category );
		if (cat_it != group_it->second.end()) {
			curLev = cat_it->second;
		} else {
			curLev = _groups[group]["all"];
		}
	} else {
		Categories::const_iterator cat_it = Statics::_groups["all"].find( category );
		if (cat_it != Statics::_groups["all"].end()) {
			curLev = cat_it->second;
		}
	}
	return (level<=curLev);
}

void Statics::transmitMessage( const Message &m ) {
	Statics::_initializedMutex.lock();
	if (!Statics::_initialized) {
		Statics::_initializedMutex.unlock();
		return;
	} else {
		Statics::_nTransmittingMutex.lock();
		Statics::_nTransmitting++;
		Statics::_nTransmittingMutex.unlock();
		Statics::_initializedMutex.unlock();
	}

	BOOST_FOREACH( Target *t, Statics::_outputs) {
		t->receive(m);
	}

	Statics::_nTransmittingMutex.lock();
	Statics::_nTransmitting--;
	Statics::_nTransmittingCond.notify_one();
	Statics::_nTransmittingMutex.unlock();
}

Formatter *Statics::getFormatter() {
	return Statics::_formatter;
}

void Statics::addTarget( Target *t ) {
	Statics::_outputs.push_back(t);
}

void Statics::flushMessages() {
	BOOST_FOREACH( Target *t, Statics::_outputs ) {
		t->flush();
	}
}

void Statics::setLevel( const std::string &group, const std::string &category, const LOG_LEVEL_TYPE level ) {
	Statics::_groups[group][category] = level;
	if (_groups[group].count("all") == 0) {
		Statics::_groups[group]["all"] = _defaultLevel;
	}
}

}
}
