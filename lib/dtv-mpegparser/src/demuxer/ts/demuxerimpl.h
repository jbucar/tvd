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
#pragma once

#include "demuxer.h"
#include <boost/thread.hpp>

namespace util {
	namespace pool {
		template<typename T> class CircularPool;
	}
}

namespace tuner {
namespace demuxer {
namespace ts {

/**
 * Esta clase realiza el demultiplexado de un ts de archivo, sacando información del @b pcr,
 * chequeando @b continuity @b counter y pasando las secciones a los filtros.
 */
class DemuxerImpl : public Demuxer {
public:
	explicit DemuxerImpl( int bufCount=100 );
	virtual ~DemuxerImpl();

	//	Initialization
	void start();
	void stop();

	//	Enqueue bytes to process
	void pushData( util::Buffer *buf );

	//	Buffer handling
	util::Buffer *allocBuffer();
	void freeBuffer( util::Buffer *buf );

protected:
	void tsParser();	

private:
	util::pool::CircularPool<util::Buffer *> *_pool;
	boost::thread _thread;
};

}
}
}

