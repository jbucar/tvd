/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "pool.h"
#include "../buffer.h"
#include "../log.h"
#include "../assert.h"
#include <boost/circular_buffer.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>

namespace util {
namespace pool {

namespace impl {

inline Buffer *getBuffer( Buffer *b ) { return b; }

template<typename T>
struct RemoveAll {
	bool operator()( const T & ) const {
		return true;
	}
};

}

template<typename T>
class CircularPool : public Pool {
public:
	CircularPool( const std::string &id, size_t max, size_t blockSize ) : Pool(id), _nodes(max), _blockSize(blockSize), _allocated(0), _mask(0), _enable(true)
	{
		BOOST_ASSERT( _nodes.capacity() > 5 );
		LDEBUG("CircularPool", "Create pool: id=%s, max=%d, blocksize=%d",
			_id.c_str(), max, _blockSize);
	}
		
	virtual ~CircularPool()
	{
		removeAll();
		if (_allocated != _freeBuffers.size()) {
			LWARN( "CircularPool", "Free buffers differ from allocated: id=%s, allocated=%d, free=%d",
				_id.c_str(), _allocated, _freeBuffers.size() );
		}
		else {
			LDEBUG("CircularPool", "Destroy pool: id=%s, allocated=%d, free=%d",
				_id.c_str(), _allocated, _freeBuffers.size() );
		}

		while (_freeBuffers.size() > 0) {
			util::Buffer *buf = _freeBuffers.front();
			delete buf;
			buf = NULL;
			_freeBuffers.pop();
		}
	}

	void enable( bool run, bool clean=true ) {
		_mutex.lock();
		_enable = run;
		if (!run && clean) {
			remove_if_impl( impl::RemoveAll<T>() );
		}
		_mutex.unlock();
	}

	virtual util::Buffer *alloc() {
		util::Buffer *buf;
		
		_mutex.lock();
		if (!_freeBuffers.empty()) {
			buf=_freeBuffers.front();
			_freeBuffers.pop();
		}
		else if (_allocated < _nodes.capacity()) {
			_allocated++;
			buf = new Buffer( _blockSize );
		}
		else {
			buf = NULL;
			while (!buf && !_nodes.empty()) {
				T &item=_nodes.front();
				buf = impl::getBuffer(item);
				_nodes.pop_front();
			}
		}
		_mutex.unlock();

		BOOST_ASSERT(buf);
		buf->resize(0);
		return buf;
	}
	
	virtual void free( util::Buffer *buf ) {
		BOOST_ASSERT(buf);
		_mutex.lock();
		_freeBuffers.push( buf );
		_mutex.unlock();
	}
	
	bool get( T &item, DWORD &mask, int ms=-1 ) {
		bool result=false;
		
		{	//	Wait for buffer or some mask changed?
			boost::unique_lock<boost::mutex> lock( _mutex );
			bool timerExpired = false;
			while (!_mask && _nodes.empty() && !timerExpired) {
				if (ms < 0) {
					_cWakeup.wait( lock );
				}
				else {
					timerExpired=_cWakeup.timed_wait( lock, boost::get_system_time()+boost::posix_time::milliseconds(ms) ) ? false : true;
				}
			}

			if (!_nodes.empty()) {
				item=_nodes.front();
				_nodes.pop_front();
				result=true;
			}

			//	Copy mask to user and reset flags
			mask = _mask;
			_mask = 0;
		}

		return result;
	}
	
	void put( T &item ) {
		bool notify=false;

		_mutex.lock();
		if (!_enable) {
			safeFree( item );
		}
		else {
			notify = true;
			_nodes.push_back( item );
		}
		_mutex.unlock();

		if (notify) {
			_cWakeup.notify_all();
		}
	}

	void removeAll() {
		remove_if( impl::RemoveAll<T>() );
	}

	template<typename P> void remove_if( P pred ) {
		_mutex.lock();
		remove_if_impl<P>( pred );
		_mutex.unlock();
	}

	void notify_all( DWORD mask ) {
		_mutex.lock();
		_mask |= mask;
		_mutex.unlock();		
		_cWakeup.notify_all();
	}

protected:
	void safeFree( T &item ) {
		Buffer *buf = impl::getBuffer(item);
		if (buf) {
			_freeBuffers.push( buf );			
		}
	}

	template<typename P> void remove_if_impl( P pred ) {
		typename boost::circular_buffer<T>::iterator it=_nodes.begin();
		while (it != _nodes.end()) {
			T &item = (*it);
			if (pred( item )) {
				safeFree( item );
				it = _nodes.erase(it);
			}
			else {
				it++;
			}
		}
	}
	

private:
	boost::condition_variable _cWakeup;
	boost::mutex _mutex;
	std::queue<util::Buffer *> _freeBuffers;
	boost::circular_buffer<T> _nodes;
	size_t _blockSize;
	size_t _allocated;
	DWORD _mask;
	bool _enable;
};

}
}
