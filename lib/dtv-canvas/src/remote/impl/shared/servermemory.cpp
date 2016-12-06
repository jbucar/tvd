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

#include "servermemory.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace canvas {
namespace remote {
namespace shared {

ServerMemory::ServerMemory( const std::string &name, size_t size )
	: Memory( name ), _size(size)
{
}

ServerMemory::~ServerMemory()
{
}

ipc::named_mutex *ServerMemory::createMutex() {
	return new ipc::named_mutex( ipc::create_only, (name()+"_MUTEX").c_str() );
}

ipc::named_semaphore *ServerMemory::createSem() {
	return new ipc::named_semaphore( ipc::create_only, (name()+"_SEM").c_str(), 0, ipc::permissions() );
}

ipc::shared_memory_object *ServerMemory::createShmObj() {
	ipc::shared_memory_object *obj = new ipc::shared_memory_object( ipc::create_only, name().c_str(), ipc::read_write );
	obj->truncate( _size );
	return obj;
}

void ServerMemory::cleanIPC() {
	ipc::shared_memory_object::remove( name().c_str() );
	ipc::named_semaphore::remove( (name()+"_SEM").c_str() );	
	ipc::named_mutex::remove( (name()+"_MUTEX").c_str() );
}

}
}
}
