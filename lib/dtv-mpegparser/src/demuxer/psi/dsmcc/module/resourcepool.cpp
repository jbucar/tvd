/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV implementation.

    DTV is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV.

    DTV es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "resourcepool.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/buffer.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {

using namespace util;

ResourcePool::ResourcePool( const std::string &id, DWORD max, DWORD blockSize )
	: _id (id)
{
    _max = max;
    _blockSize = blockSize;
    _allocated = _locked = 0;

    LDEBUG("ResourcePool", "Create pool: id=%s, max=%d, blocksize=%d",
           _id.c_str(), _max, _blockSize);
}

ResourcePool::~ResourcePool()
{
	LDEBUG("ResourcePool", "Destroy pool: id=%s, allocated=%d, free=%d, locked=%d",
	       _id.c_str(), _allocated, _freeBuffers.size(), _locked);
	clear();
}

DWORD ResourcePool::blockSize( void ) const {
	return _blockSize;
}

bool ResourcePool::canAlloc( DWORD size ) const {
	return blocks( size ) < _max;
}

DWORD ResourcePool::availables( void ) const {
    DWORD totalFree = (_max - _allocated) + _freeBuffers.size();
    return (_locked > totalFree) ? 0 : totalFree - _locked;
}

bool ResourcePool::lockBlocks( DWORD moduleSize ) {
	bool result=false;
	DWORD need = blocks( moduleSize );

	_mFreeBuffers.lock();
	if (need < availables()) {
		_locked += need;
		result=true;
	}
	// LDEBUG("ResourcePool", "lock: id=%s max=%ld, allocated=%ld, free=%d, locked=%ld, availables=%ld, need=%ld, result=%d",
	//        _id.c_str(), _max, _allocated, _freeBuffers.size(), _locked, availables(), need, result );
	
	_mFreeBuffers.unlock();

	return result;
}

void ResourcePool::unlockBlocks( DWORD moduleSize ) {
	DWORD need = blocks( moduleSize );
	
	// LDEBUG("ResourcePool", "unlock: id=%s max=%ld, allocated=%ld, free=%d, locked=%ld, availables=%ld, need=%ld",
	// 	_id.c_str(), _max, _allocated, _freeBuffers.size(), _locked, availables(), need );

	_mFreeBuffers.lock();
	if (_locked >= need) {
		_locked -= need;
	} else {
		LWARN("ResourcePool", "unlock blocks error: id=%s, need=%d, $locked=%d", _id.c_str(), need, _locked );
		DTV_ASSERT(false);
		_locked = 0;
	}
	_mFreeBuffers.unlock();
	_cWakeup.notify_all();
}

const std::string &ResourcePool::id() const {
	return _id;
}

DWORD ResourcePool::blocks( DWORD moduleSize ) const {
	DWORD blocks = moduleSize/_blockSize;
	if (moduleSize % _blockSize) {
		blocks++;
	}
	return blocks;
}

void ResourcePool::clear() {
	_mFreeBuffers.lock();
	Buffer *buf;
	while (!_freeBuffers.empty()) {
		buf = _freeBuffers.front();
		_freeBuffers.pop();
		delete buf;
	}
	_mFreeBuffers.unlock();
}

Buffer *ResourcePool::alloc( DWORD msTimeout/*=0*/ ) {
	Buffer *buf;

	//  Wait for buffers if no in free queue or not allocated overflow the max
	boost::unique_lock<boost::mutex> lock( _mFreeBuffers );
	while (_freeBuffers.empty() && _allocated >= _max) {
		//LDEBUG("ResourcePool", "waiting to free buffers: id=%s, msTimeout=%ld", _id.c_str(), msTimeout);
		if (!msTimeout) {
			_cWakeup.wait( lock );
		} else {
			_cWakeup.timed_wait( lock, boost::get_system_time()+boost::posix_time::milliseconds(msTimeout) );
			break;
		}
	}

	//  Free buffer?
	if (!_freeBuffers.empty()) {
		buf=_freeBuffers.front();
		_freeBuffers.pop();

		buf->resize( 0 );
	} else if (_allocated < _max) {
		_allocated++;
		buf = new Buffer( _blockSize );
	} else {
		buf = NULL;
	}

	return buf;
}

void ResourcePool::free( std::vector<util::Buffer *> &buffers, bool locked/*=false*/ ) {
	_mFreeBuffers.lock();
	BOOST_FOREACH( Buffer *buf, buffers ) {
		_freeBuffers.push( buf );
	}
	buffers.clear();
	_mFreeBuffers.unlock();

	if (!locked) {
		_cWakeup.notify_all();
	}
}

void ResourcePool::free( util::Buffer *buf, bool locked/*=false*/ ) {
	DTV_ASSERT(buf);
	if (buf) {
		_mFreeBuffers.lock();
		_freeBuffers.push( buf );
		_mFreeBuffers.unlock();

		if (!locked) {
			_cWakeup.notify_one();
		}
		
	}
	else {
		LWARN("ResourcePool", "ResourcePool(%d)::free called with buf in NULL", _id.c_str() );
	}
}

}
}
