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

#include "dispatcher.h"
#include <util/android/android.h>
#include <util/id/pool.h>
#include <util/task/dispatcherimpl.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <android/looper.h>

namespace canvas {
namespace android {

namespace impl {

class IOEvent {
public:
	IOEvent( util::DESCRIPTOR fd, const util::id::Ident &id, const util::io::Callback &callback ) : _fd(fd), _id(id), _callback(callback) {}
	~IOEvent() {}

	util::DESCRIPTOR _fd;
	util::id::Ident _id;
	util::io::Callback _callback;
};

static int on_fd_callback( int fd, int events, void* data ) {
	DTV_ASSERT( data );
	Dispatcher *disp = (Dispatcher *)data;
	return disp->onFD( fd, events );
}

}

Dispatcher::Dispatcher()
{
	_tasks = new util::task::DispatcherImpl();
	_tasks->onPost( boost::bind(&Dispatcher::onPost,this) );	
}

Dispatcher::~Dispatcher()
{
	delete _tasks;
}

//	Run/exit io event notification loop
void Dispatcher::run() {
	bool exit=false;
	int ms;
	int ident;
	int events;
	void *output;

	LINFO( "android", "run begin" );
	
	//	loop waiting for stuff to do.
	while (!exit) {
		//	Check timers
		ms = _timers.consume();

		//	Block waiting for events.
		ident=ALooper_pollAll( ms, NULL, &events, (void**)&output );

		if (ident > 2) {	//	Discard system events
			LDEBUG( "android", "poll: ms=%d, iden=%d", ms, ident );
		}

		//	Process android glue data
		util::android::glue()->process( ident, output );

		//	Handle exit
		if (util::android::glue()->needExit() || !util::android::glue()->haveGUI()) {
			exit = true;
		}
		else {
			//	Run all tasks
			_tasks->runAll();
		}
	}

	//	Clear all events
	clearEvents();
	
	LINFO( "android", "run end" );	
}

void Dispatcher::exit() {
	LINFO( "android", "exit" );
	
	util::android::glue()->exit();

	//	Wakeup loop
	ALooper_wake( util::android::glue()->looper() );
}

//	Dispatcher
void Dispatcher::onPost() {
	//	Wakeup loop
	ALooper_wake( util::android::glue()->looper() );
}

util::task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}
 
//	Timer events
util::id::Ident Dispatcher::addTimer( int ms, const util::io::Callback &callback ) {
	return _timers.add( ms, callback );
}

void Dispatcher::stopTimer( util::id::Ident &id ) {
	_timers.rm( id );
}

//	IO events
util::id::Ident Dispatcher::addIO( util::DESCRIPTOR fd, const util::io::Callback &callback ) {
	int res=ALooper_addFd( util::android::glue()->looper(), fd, -1, ALOOPER_EVENT_INPUT, impl::on_fd_callback, this );
	if (res == 1) {
		util::id::Ident id = allocID();
		impl::IOEvent *ev = new impl::IOEvent( fd, id, callback );
		_events.push_back( ev );
		return id;
	}
	return util::id::Ident();
}

void Dispatcher::stopIO( util::id::Ident &id ) {
	Events::iterator it=std::find_if(
		_events.begin(),
		_events.end(),
		boost::bind(&impl::IOEvent::_id,_1) == id );
	if (it != _events.end()) {
		stopEvent( (*it) );
		_events.erase( it );
	}	
}

int Dispatcher::onFD( int fd, int events ) {
	int ret = 0;	//	No more
	Events::const_iterator it=std::find_if(
		_events.begin(),
		_events.end(),
		boost::bind(&impl::IOEvent::_fd,_1) == fd
	);
	if (it != _events.end()) {
		impl::IOEvent *ev = (*it);
		
		//	IO pending?
		if (events & ALOOPER_EVENT_INPUT) {
			ev->_callback(ev->_id);
		}

		//	If any error, remove event from list
		if (events & ALOOPER_EVENT_ERROR || events & ALOOPER_EVENT_HANGUP) {
			stopIO( ev->_id );
		}
		else {
			ret = 1;
		}
	}

	return ret;
}

void Dispatcher::stopEvent( impl::IOEvent *ev ) {
	ALooper_removeFd( util::android::glue()->looper(), ev->_fd );
	delete ev;
}

void Dispatcher::clearEvents() {
	//	Stop and clear all io events
	BOOST_FOREACH( impl::IOEvent *ev, _events ) {
		stopEvent( ev );
	}
	_events.clear();

	//	Stop all timers
	_timers.clear();
}

//	Socket events
util::id::Ident Dispatcher::addSocket( util::SOCKET sock, const util::io::Callback &callback ) {
	return addIO( sock, callback );
}

void Dispatcher::stopSocket( util::id::Ident &id ) {
	stopIO( id );
}

}
}
	
