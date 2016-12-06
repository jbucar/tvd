/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "manager.h"
#include "request.h"
#include "../../log.h"
#include "../../assert.h"

namespace util {
namespace netlib {
namespace curl {

Manager::Manager()
{
	_activeHandles = 0;
	_exit=false;
}

Manager::~Manager()
{
	DTV_ASSERT( _activeHandles == 0 );
}

bool Manager::initialize() {
	if (netlib::Manager::initialize()) {
		_exit = false;
		_thread = boost::thread(boost::bind(&Manager::loop,this) );
		return true;
	}
	return false;
}

void Manager::finalize() {
	stopAll();

	//	Wakeup thread
	_mutex.lock();
	_exit=true;
	_cWakeup.notify_all();
	_mutex.unlock();
	_thread.join();

	netlib::Manager::finalize();

	DTV_ASSERT(_activeHandles == 0);
}

netlib::Request *Manager::createRequest( const id::Ident &id, RequestInfo *info, Reply *reply ) {
	return new Request(this,id,info,reply);
}

void Manager::addRequest( Request *req ) {
	LTRACE( "curl", "Add request: req=%p", req );
	_mutex.lock();
	_pendingList.push_back( req );
	_cWakeup.notify_all();
	_mutex.unlock();
}

void Manager::removeRequest( Request *req ) {
	LTRACE( "curl", "Remove request: req=%p", req );
	_mutex.lock();
	_cancelList.push_back( req );
	_cWakeup.notify_all();
	_mutex.unlock();
}

bool Manager::updateHandles( CURLM *_handle ) {
	bool result=false;

	{	//	Lock!
		boost::unique_lock<boost::mutex> lock( _mutex );
		while (_pendingList.empty() && _cancelList.empty() && !_activeHandles && !_exit) {
			_cWakeup.wait( lock );
		}

		{	//	Add pending curl easy handles to multi list
			size_t size = _pendingList.size();
			for (size_t i = 0; i<size; i++) {
				Request *req = _pendingList[i];
				if (req->init()) {
					curl_multi_add_handle( _handle, req->handle() );
					_activeHandles++;
				}
			}
			_pendingList.clear();
		}

		{	//	Remove curl easy handles from multi list
			size_t size = _cancelList.size();
			for (size_t i=0; i<size; i++) {
				Request *req = _cancelList[i];
				CURLMcode ret = curl_multi_remove_handle( _handle, req->handle() );
				if (ret != CURLM_OK) {
					LWARN( "curl", "Error removing request: handle=%p, err=%d", req, ret );
				}
				else {
					req->fin();
					_activeHandles--;
				}
			}
			_cancelList.clear();
		}

		result = _exit;
	}	//	Unlock!

	return result;
}

void Manager::loop() {
	LINFO( "curl", "Begin curl manager thread" );

	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		LERROR( "curl", "Cannot initialize curl" );
		return;
	}

	CURLM *_handle = curl_multi_init();
	if (!_handle) {
		curl_global_cleanup();
		LERROR( "curl", "Cannot create multi curl" );
		return;
	}

	bool exit=false;
	while (!exit) {
		if (updateHandles(_handle)) {
			break;
		}

		// Retry 'select' if it was interrupted by a process signal.
		int rc = 0;
		do {
			fd_set fdread;
			fd_set fdwrite;
			fd_set fdexcep;

			int maxfd = 0;

			const int selectTimeoutMS = 5;

			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = selectTimeoutMS * 1000; // select waits microseconds

			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);
			curl_multi_fdset( _handle, &fdread, &fdwrite, &fdexcep, &maxfd );
			// When the 3 file descriptors are empty, winsock will return -1
			// and bail out, stopping the file download. So make sure we
			// have valid file descriptors before calling select.
			if (maxfd >= 0) {
				rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
			}
		} while (rc == -1 && errno == EINTR);

		{	//	Perform operations!
			int activeRequestCount = 0;
			while (curl_multi_perform( _handle, &activeRequestCount ) == CURLM_CALL_MULTI_PERFORM) {
				//	Run
			}
		}

		{	//	Check for completed operations
			int messagesInQueue = 1;
			while (messagesInQueue > 0) {
				CURLMsg* msg = curl_multi_info_read( _handle, &messagesInQueue );
				if (!msg)
					break;

				if (msg->msg == CURLMSG_DONE) {
					Request *req = NULL;
					CURLcode err = curl_easy_getinfo( msg->easy_handle, CURLINFO_PRIVATE, &req );
					if (err == CURLE_OK && req) {
						if (msg->data.result == CURLE_OK) {
							req->status(request::success);
						} else {
							req->status(request::failure);
							LWARN("curl", curl_easy_strerror(msg->data.result));
						}
					} else {
						LWARN("curl", curl_easy_strerror(err));
					}
				}
			}
		}
	}

	//LINFO( "curl", "End curl manager thread: pending=%d, completed=%d, active=%d", _pendingHandleList.size(), _completedHandleList.size(), _activeHandles );
	DTV_ASSERT(_activeHandles == 0);

	curl_multi_cleanup(_handle);
	curl_global_cleanup();

	LINFO( "curl", "End curl manager thread" );
}

}
}
}

