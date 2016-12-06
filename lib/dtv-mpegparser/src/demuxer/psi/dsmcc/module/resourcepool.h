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

#include <util/types.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <queue>

namespace util {
	class Buffer;
}

namespace tuner {
namespace dsmcc {

class ResourcePool {
public:
	ResourcePool( const std::string &id, util::DWORD max, util::DWORD blockSize );
	virtual ~ResourcePool();

	const std::string &id() const;
	util::DWORD blocks( util::DWORD size ) const;
	util::DWORD blockSize() const;
	bool canAlloc( util::DWORD moduleSize ) const;
	bool lockBlocks( util::DWORD moduleSize );
	void unlockBlocks( util::DWORD moduleSize );	
	util::Buffer *alloc( util::DWORD msTimeout=0 );
	void free( std::vector<util::Buffer *> &buffers, bool locked=false );
	void free( util::Buffer *buf, bool locked=false );

protected:
	void clear();
	util::DWORD availables() const;

private:
	std::string _id;
	util::DWORD _max;
	util::DWORD _blockSize;
	util::DWORD _allocated;
	util::DWORD _locked;
	boost::mutex _mFreeBuffers;
	boost::condition_variable _cWakeup;
	std::queue<util::Buffer *> _freeBuffers;
};

}
}

