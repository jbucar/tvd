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

#include "../../frontend.h"
#include <boost/thread.hpp>

namespace tuner {

namespace demuxer {
namespace ts {
	class DemuxerImpl;
}
}

namespace ts {

class Frontend : public tuner::Frontend {
public:
	Frontend( demuxer::ts::DemuxerImpl *demux );
	virtual ~Frontend( void );

	virtual bool start( size_t nIndex );
	virtual void stop();

	virtual bool getStatus( status::Type &st ) const;

protected:
	demuxer::ts::DemuxerImpl *demux() const { return _demux; }

	virtual std::string startImpl( size_t nIndex )=0;
	virtual void stopImpl()=0;

	virtual void reader( const std::string net )=0;

private:
	bool _locked;
	demuxer::ts::DemuxerImpl *_demux;
	boost::thread _thread;
};
	
}
}
