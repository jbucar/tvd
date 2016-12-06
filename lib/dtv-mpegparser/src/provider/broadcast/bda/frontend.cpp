/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "frontend.h"
#include "bdagraph.h"
#include "provider.h"
#include <util/log.h>

namespace tuner {
namespace bda {

Frontend::Frontend( demuxer::ts::DemuxerImpl *demux )
	: _demux( demux )
{
	_bda = new BDAGraph( _demux );
}

Frontend::~Frontend()
{
	delete _bda;
	_bda = NULL;
}

bool Frontend::initialize() {
	return _bda->initialize();
}

void Frontend::finalize() {
}

//	ISDBT
bool Frontend::start( tuner::broadcast::IsdbtConfig *ch ) {
	unsigned int khBandwidth = 6000;
	LDEBUG( "Frontend", "Tune started begin: freq=%d, bandwidth=%d", ch->frequency(), khBandwidth );
	return _bda->setFrequency(ch->frequency(), khBandwidth);
}

//	Status/signal
bool Frontend::getStatus( status::Type &st ) const {
	memset(&st,0,sizeof(st));
	st.isLocked =  _bda->getSignalStrength() > 0.0;

//	LDEBUG( "Frontend", "Status: freq=%f, isLocked=%s", f,  st.isLocked?"true":"false");
	return true;
}

}
}
