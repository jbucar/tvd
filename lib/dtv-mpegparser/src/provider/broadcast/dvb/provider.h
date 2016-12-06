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

#include "../../provider.h"
#include <util/id/ident.h>
#include <boost/thread.hpp>
#include <map>

namespace util {
namespace io {
	class Dispatcher;
}
}

namespace tuner {

namespace stream {
	class Pipe;
}

namespace dvb {

class Provider : public tuner::Provider {
public:
	Provider();
	virtual ~Provider();

	//	Getters
	const std::string &device();

	//	Aux methods
	void startReadSections( int fd, ID pid );
	void startStream( int fd, stream::Pipe *pipe );
	void rmWatcher( int fd );

protected:
	//	Tuner methods
	virtual Frontend *createFrontend() const;

	//	Filter methods
	virtual Filter *createFilter( PSIDemuxer *demux );
	virtual Filter *createFilter( pes::FilterParams *params );
	virtual bool startEngineFilter();
	virtual void stopEngineFilter();
	virtual int maxFilters() const;
	virtual bool checkCRC() const;

	//	Aux
	typedef std::map<int,util::id::Ident> Handles;
	typedef boost::function<void (util::id::Ident &)> IOCallback;
	void readSection( int fd, ID pid );
	void readStream( int fd, stream::Pipe *pipe );
	void startIO( int fd, const IOCallback &callback );
	void stopIO( int fd );
	void ioThread();

private:
	boost::thread _thread;
	util::io::Dispatcher *_io;
	int _maxFilters;
	std::string _frontend;
	std::string _demux;
	Handles _handles;
};

}
}

