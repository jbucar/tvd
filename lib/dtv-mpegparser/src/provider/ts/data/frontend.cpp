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

#include "frontend.h"
#include <util/assert.h>
#include <util/log.h>

namespace tuner {
namespace ts {

Frontend::Frontend( demuxer::ts::DemuxerImpl *demux )
	: _demux( demux )
{
	_locked = false;
}

Frontend::~Frontend()
{
}

bool Frontend::getStatus( status::Type &st ) const {
	memset(&st,0,sizeof(st));
	st.isLocked = _locked;
	return true;
}

bool Frontend::start( size_t nIndex ) {
	std::string net = startImpl( nIndex );
	if (!net.empty()) {
		DTV_ASSERT( !_thread.joinable() );
		LDEBUG( "ts::Frontend", "Start network %s", net.c_str() );
		_locked = true;
		_thread = boost::thread( boost::bind( &Frontend::reader, this, net ) );
	}
	return _locked;
}

void Frontend::stop() {
	LDEBUG( "ts::Frontend", "Stop network" );
	stopImpl();
	_thread.join();
	_locked = false;
}

}
}
