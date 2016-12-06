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

#include "dispatcher.h"
#include "../../task/dispatcherimpl.h"
#include "../../assert.h"
#include "../../log.h"
#include "../../mcr.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <ev.h>

#define INIT(data,callback)	\
	DTV_ASSERT( !callback.empty() ); \
	data.callback = callback; \
	data.id = allocID();	\
	LDEBUG("io::ev::Dispatcher", "Init event: id=%ld", data.id->getID());

#define STOP_EVENT(t, l, id, method)	\
	if (id::isValid(id)) { \
		std::list<t *>::iterator it=std::find_if( l.begin(), l.end(), impl::EventFinder<t>(id->getID()) ); \
		if (it != l.end()) { \
			t *ev=(*it); \
			l.erase(it); \
			ev_##method##_stop( _loop, &ev->ev ); \
			ev->data.id.reset(); \
			free( ev ); \
		} \
		else { LWARN("io::ev::Dispatcher", "invalid " #method ": %ld", id->getID()); } \
	}

namespace util {
namespace io {
namespace ev {

namespace impl {

struct my_data {
	id::Ident id;
	io::Callback callback;
};

struct my_timer_ev {
	struct ev_timer ev;
	struct my_data data;
};

struct my_io_ev {
	struct ev_io ev;
	struct my_data data;
};

static void wakeup_cb( struct ev_loop *loop, struct ev_async */*w*/, int /*revents*/ ) {
	Dispatcher *sys = (Dispatcher *)ev_userdata(loop);
	DTV_ASSERT(sys);
	sys->wakeup();
}

static void timer_cb( struct ev_loop *loop, struct ev_timer *timer, int /*revents*/ ) {
	Dispatcher *sys = (Dispatcher *)ev_userdata(loop);
	DTV_ASSERT(sys);
	struct my_timer_ev *ev = (struct my_timer_ev *)timer;
	id::Ident id(ev->data.id);
	ev->data.callback( id );
	sys->stopTimer( id );
}

static void io_cb( struct ev_loop */*loop*/, struct ev_io *w, int /*revents*/ ) {
	struct my_io_ev *ev = (struct my_io_ev *)w;
	ev->data.callback( ev->data.id );
}

template<class T>
class EventFinder {
public:
	EventFinder( id::ID_TYPE id ) : _id(id) {}

	bool operator()( const T *io ) const {
		return io->data.id->getID() == _id;
	}

private:
	id::ID_TYPE _id;
};

}

Dispatcher::Dispatcher()
{
	_loop = NULL;
	_wakeup = NULL;
	_tasks = new task::DispatcherImpl();
}

Dispatcher::~Dispatcher()
{
	delete _tasks;
}

//	Initialization
bool Dispatcher::initialize() {
	//	Create dispatcher
	_tasks->onPost( boost::bind(&Dispatcher::onPost,this) );

	//	Create loop
	_loop = ev_loop_new(EVFLAG_AUTO);
	if (!_loop) {
		LERROR("io::ev::Dispatcher", "cannot create loop");
		return false;
	}
	ev_set_userdata( _loop, this );

	//	Create and initialize wakeup watcher
	_wakeup = (ev_async *)malloc( sizeof(struct ev_async) );
	if (!_wakeup) {
		free(_loop);
		LWARN("io::ev::Dispatcher", "cannot create async event");
		return false;
	}
	ev_async_init (_wakeup, impl::wakeup_cb);
	ev_async_start (_loop, _wakeup);

	//	Register into dispatcher
	_tasks->registerTarget( this, "io::ev::Dispatcher" );

	return true;
}

void Dispatcher::finalize() {
	//	Run all tasks in queue
	_tasks->runAll();

	//	Remove io from dispatcher
	_tasks->unregisterTarget( this );

	//	Cleanup all events
	while (!_io.empty()) {
		ListOfIO::iterator it = _io.begin();
		LWARN("io::ev::Dispatcher", "stopping lost io event: fd=%d", (*it)->ev.fd);
		stopIO( (*it)->data.id );
	}

	while (!_timers.empty()) {
		ListOfTimer::iterator it = _timers.begin();
		LWARN("io::ev::Dispatcher", "stopping lost timer event");
		stopTimer( (*it)->data.id );
	}

	//	Destroy loop
	ev_loop_destroy( _loop );
	_loop = NULL;
	free(_wakeup);
	_wakeup = NULL;

	{	//	Clean task callback
		util::task::Type empty;
		_tasks->onPost( empty );
	}
}

void Dispatcher::run() {
	LINFO( "io::ev::Dispatcher", "Run begin" );

	//	Wait for events to arrive
	ev_run (_loop, 0);

	LINFO( "io::ev::Dispatcher", "Run end" );
}

void Dispatcher::exit() {
	LINFO( "io::ev::Dispatcher", "Exit" );
	ev_break( _loop );
}

//	Dispatcher
task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}

void Dispatcher::onPost() {
	//	wakeup loop
	ev_async_send( _loop, _wakeup );
}

//	Timer events
id::Ident Dispatcher::addTimer( int ms, const Callback &callback ) {
	LDEBUG("io::ev::Dispatcher", "Add timer: ms=%d", ms);

	//	Create timer
	struct impl::my_timer_ev *timer = (struct impl::my_timer_ev *)calloc( 1, sizeof(struct impl::my_timer_ev) );
	if (!timer) {
		LERROR("io::ev::Dispatcher", "cannot create timer event");
		return id::Ident();
	}
	else {
		//	Setup event
		INIT( timer->data, callback );

		//	Add to list
		_timers.push_back( timer );

		{	//	Add to loop
			struct ev_timer *ev = &timer->ev;
			ev_timer_init( ev, impl::timer_cb, ms/1000., 0. );
			ev_timer_start (_loop, ev);
		}

		return timer->data.id;
	}
}

void Dispatcher::stopTimer( id::Ident &id ) {
	STOP_EVENT( struct impl::my_timer_ev, _timers, id, timer );
	id.reset();
}

//	IO events
id::Ident Dispatcher::addIO( DESCRIPTOR fd, const Callback &callback ) {
	struct impl::my_io_ev *io = (struct impl::my_io_ev *)calloc( 1, sizeof(struct impl::my_io_ev) );
	if (!io) {
		LERROR("io::ev::Dispatcher", "cannot create io event");
		return id::Ident();
	}
	else {
		//	Setup event
		INIT( io->data, callback );

		//	Add to list
		_io.push_back( io );

		{	//	Add to loop
			struct ev_io *ev = &io->ev;
			ev_io_init( ev, impl::io_cb, fd, EV_READ );
			ev_io_start (_loop, ev );
		}

		return io->data.id;
	}
}

void Dispatcher::stopIO( id::Ident &id ) {
	STOP_EVENT( struct impl::my_io_ev, _io, id, io );
	id.reset();
}

//	Socket events
id::Ident Dispatcher::addSocket( SOCKET sock, const Callback &callback ) {
	return addIO( sock, callback );
}

void Dispatcher::stopSocket( id::Ident &id ) {
	stopIO( id );
}

//	Aux
void Dispatcher::wakeup() {
	_tasks->runAll();
}

}
}
}

