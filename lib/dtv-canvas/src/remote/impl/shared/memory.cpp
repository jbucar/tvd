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

#include "memory.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace canvas {
namespace remote {
namespace shared {

Memory::Memory( const std::string &name )
	: remote::Memory(name)
{
	_shmObj = NULL;
	_mmapReg = NULL;
	_nMutex = NULL;
	_nSem = NULL;
}

Memory::~Memory()
{
	DTV_ASSERT(!_shmObj);
	DTV_ASSERT(!_mmapReg);
	DTV_ASSERT(!_nMutex);
	DTV_ASSERT(!_nSem);
}

bool Memory::initialize() {
	bool result = false;

	//	Try create shared
	try {
		cleanIPC();
		
		//	Create process mutex and semaphore
		_nMutex = createMutex();
		_nSem = createSem();

		//	Create shared memory in this process
		_shmObj = createShmObj();

		_mmapReg = new ipc::mapped_region( *_shmObj, ipc::read_write );

		result = true;
	} catch( ipc::interprocess_exception &ex ) {
		LERROR("shared::ServerMemory", "error while creating shared memory: what=%s", ex.what());
		finalize();
	}

	return result;
}

void Memory::finalize() {
	cleanIPC();

	DEL(_mmapReg);
	DEL(_shmObj);
	DEL(_nMutex);
	DEL(_nSem);
}

size_t Memory::size() const {
	return _mmapReg->get_size();
}

void *Memory::lock() {
	_nMutex->lock();
	return _mmapReg->get_address();
}

void Memory::unlock() {
	_nMutex->unlock();
}

void Memory::wait() {
	_nSem->wait();
}

void Memory::post() {
	_nSem->post();
}

void Memory::cleanIPC() {
}

}
}
}
