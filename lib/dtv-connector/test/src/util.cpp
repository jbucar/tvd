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
#include "util.h"
#include <util/types.h>
#include <util/log.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

namespace test {

namespace fs = boost::filesystem;

void counter( int *count ) {
	(*count)++;
}

boost::condition_variable _cWakeup;
boost::mutex _mutex;
std::map<util::DWORD,int> _flags;

void clean()  {
	_mutex.lock();
	_flags.clear();
	_mutex.unlock();
}

void notify( flags::type flag ) {
	LTRACE("ConnectorTest", "notify flag=%d", flag );

	
	_mutex.lock();
	_flags[flag]++;
	_mutex.unlock();
	_cWakeup.notify_all();
}

void wait( flags::type flag ) {
	LTRACE("ConnectorTest", "wait flag=%d", flag );

	boost::unique_lock<boost::mutex> lock( _mutex );
	while (!_flags[flag]) {
		_cWakeup.wait( lock );
	}
	_flags[flag]--;
}

}
