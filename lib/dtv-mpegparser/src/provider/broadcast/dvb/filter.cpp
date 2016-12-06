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

#include "filter.h"
#include "provider.h"
#include <util/assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/dvb/dmx.h>

namespace tuner {
namespace dvb {

Filter::Filter( ID pid, util::DWORD timeout, Provider *dvb )
	: tuner::Filter( pid, timeout ), _dvb(dvb), _fd(-1), _io(false)
{
}

Filter::~Filter( void )
{
	DTV_ASSERT(_fd == -1);
}

bool Filter::initialized() const {
	return _fd >= 0;
}

bool Filter::initialize( void ) {
	//	Open a new demuxer device
	_io = false;
	_fd = open( _dvb->device().c_str(), O_RDWR | O_NONBLOCK );
	return _fd >= 0;
}

void Filter::deinitialize( void ) {
	close( _fd );
	_fd = -1;
	_io = false;
}

void Filter::stop( void ) {
	DTV_ASSERT( _fd >= 0 );
	ioctl( _fd, DMX_STOP );
	if (_io) {
		dvb()->rmWatcher( _fd );
	}
}

bool Filter::startSection() {
	dvb()->startReadSections( _fd, pid() );
	_io = true;
	return true;
}

bool Filter::startStream( stream::Pipe *pipe ) {
	DTV_ASSERT(pipe);
	dvb()->startStream( _fd, pipe );
	_io = true;
	return true;
}

}
}

