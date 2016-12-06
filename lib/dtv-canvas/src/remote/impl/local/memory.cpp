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
#include <util/log.h>
#include <util/assert.h>

namespace canvas {
namespace remote {
namespace local {

MemMap::MemMap()
{
	LDEBUG("remote::local::MemMap", "created");
}

MemMap::~MemMap()
{
	LDEBUG("remote::local::MemMap", "destroyed");
	DTV_ASSERT(_map.size() == 0);
}

void MemMap::put( const std::string &name, MemValue *value ) {
	_map[name] = value;
}

MemValue *MemMap::get( const std::string &name ) const {
	std::map<std::string, MemValue*>::const_iterator it = _map.find(name);
	return it->second;
}

void MemMap::remove( const std::string &name ) {
	_map.erase(name);
}

Memory::Memory( const std::string &name )
	: remote::Memory(name)
{
}

Memory::~Memory()
{
}

bool Memory::initialize() {
	LDEBUG("remote::local::Memmory", "initialize(name=%s)", name().c_str() );
	bool result = init();
	return result;
}

void Memory::finalize() {
	fin();
}

bool Memory::init() {
	return true;
}

void Memory::fin() {
}

size_t Memory::size() const {
	MemValue *val = memMap()->get( name() );
	return val->size;
}

void *Memory::lock() {
	MemValue *val = memMap()->get( name() );
	val->memMtx.lock();
	return val->ptr;
}

void Memory::unlock() {
	MemValue *val = memMap()->get( name() );
	val->memMtx.unlock();
}

void Memory::wait() {
	MemValue *val = memMap()->get( name() );
	boost::unique_lock<boost::mutex> lock(val->condMtx);
	val->cond.wait( lock );
}

void Memory::post() {
	MemValue *val = memMap()->get( name() );
	val->cond.notify_one();
}

MemMap *Memory::memMap() const {
	return static_cast<MemMap*>(&_memMap.instance());
}

}
}
}
