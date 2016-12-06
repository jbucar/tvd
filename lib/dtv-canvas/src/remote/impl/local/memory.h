/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../../memory.h"
#include <string>
#include <util/singleton.h>
#include <boost/thread.hpp>

namespace canvas {
namespace remote {
namespace local {

struct MemValue {
	void *ptr;
	size_t size;
	boost::mutex memMtx;
	boost::mutex condMtx;
	boost::condition_variable cond;
};

class MemMap {
public:
	MemMap();
	virtual ~MemMap();

	void put( const std::string &name, MemValue *value );
	MemValue *get( const std::string &name ) const;
	void remove( const std::string &name );

private:
	std::map<std::string, MemValue*> _map;
};

class Memory : public remote::Memory {
public:
	explicit Memory( const std::string &name );
	virtual ~Memory();

	virtual bool initialize();
	virtual void finalize();

	size_t size() const;

	// Access memory
	virtual void *lock();
	virtual void unlock();

	// Sinchronization
	virtual void wait();
	virtual void post();

protected:
	virtual bool init();
	virtual void fin();

	MemMap *memMap() const;

private:
	util::Singleton<MemMap> _memMap;
};

}
}
}
