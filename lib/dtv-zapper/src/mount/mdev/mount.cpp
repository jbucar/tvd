/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "mount.h"
#include "generated/config.h"
#include <canvas/system.h>
#include <util/fs.h>
#include <util/io/dispatcher.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

namespace zapper {
namespace mount {
namespace mdev {

Mount::Mount( canvas::System *sys )
{
	_sys = sys;
	_usb = -1;
}

Mount::~Mount()
{
}

bool Mount::init() {
	bool result=false;

	//	Reset mount status
	_currentPaths.clear();

	//	Configure usb notification
	LINFO("mdev::Mount", "Setup fifo for usb");
	int ret = mkfifo( MDEV_FIFO, 0666 );
	if (ret == -1 && errno != EEXIST) {
		LWARN("mdev::Mount", "cannot create the named pipe");
		return false;
	}

	//	Try open pipe
	if (openUsb()) {
		//	Check status
		checkMounts();
		result=true;
	}
	return result;
}

void Mount::fin() {
	closeUsb();
}

bool Mount::openUsb() {
	//	Open the pipe for reading
	_usb = open( MDEV_FIFO, O_RDONLY | O_NONBLOCK );
	if (_usb < 0) {
		LWARN("mdev::Mount", "cannot open usb fifo: fifo=%s, errno=%s", MDEV_FIFO, strerror(errno) );
		return false;
	}
	fcntl( _usb, F_SETFL, O_NONBLOCK);

	//	Register fd to get notifications
	_evtID = _sys->io()->addIO( _usb, boost::bind(&Mount::onChanged,this) );
	if (!util::id::isValid(_evtID)) {
		LERROR("mdev::Mount", "cannot start fd io event");
		return false;
	}

	return true;
}

void Mount::closeUsb() {
	//	Unregister handler
	_sys->io()->stopIO( _evtID );

	//	Close descriptor
	close( _usb );
}

void Mount::checkPath( const std::vector<std::string> &v1, const std::vector<std::string> &v2, bool oper ) {
	BOOST_FOREACH( const std::string &p, v1 ) {
		if (std::find( v2.begin(), v2.end(), p) == v2.end()) {
			addPath( p, oper );
		}
	}
}

void Mount::checkMounts() {
	std::set<std::string> paths;
	if (!util::getMountedFilesystems( paths ) ) {
		LERROR( "mdev::Mount", "Cannot get mounted filesystems" );
		return;
	}

	//	Get all mounted filesystem
	std::vector<std::string> currents;
	BOOST_FOREACH( const std::string &p, paths ) {
		if (p.compare( 0, strlen(MDEV_PATH), MDEV_PATH ) == 0) {
			currents.push_back( p );
		}
	}

	//	Add new paths
	checkPath( currents, _currentPaths, true );
	//	Remove unused paths
	checkPath( _currentPaths, currents, false );

	_currentPaths.swap(currents);
}

void Mount::onChanged() {
    char data[32];

    LINFO("mdev::Mount", "Mount changed, re-scan usb directory");

    //	Reset pipe
    int bytes = read( _usb, data, sizeof(data) );
    while (bytes == sizeof(data)) {
	    bytes = read( _usb, data, sizeof(data));
    }

    if (!bytes) {
		LDEBUG("mdev::Mount", "Pipe handup, re-open");
		closeUsb();
		openUsb();
    }

    checkMounts();
}

}
}
}

