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
#include "../../assert.h"
#include "../../log.h"
#include "../../task/dispatcherimpl.h"
#include <boost/bind.hpp>
#include <event2/event.h>
#include <event2/thread.h>

#if !defined(LIBEVENT_VERSION_NUMBER) || LIBEVENT_VERSION_NUMBER < 0x02000100
#	error "This version of Libevent is not supported; Get 2.0.1 or later."
#endif

#define INIT(ev, callback) \
	DTV_ASSERT( !callback.empty() ); \
	ev->callback = callback; \
	ev->id = allocID(); \
	LDEBUG("io::event::Dispatcher", "Init event: id=%ld", ev->id->getID());

#define STOP_EVENT(l, id)	\
	if (id::isValid(id)) { \
		std::list<impl::my_event*>::iterator it=std::find_if( l.begin(), l.end(), impl::EventFinder(id->getID()) ); \
		if (it != l.end()) { \
			struct impl::my_event *ev=(*it); \
			l.erase(it); \
			event_del( ev->event ); \
			event_free( ev->event ); \
			ev->id.reset(); \
			free( ev ); \
		} \
		else { LWARN("io::event::Dispatcher", "invalid event: %ld", id->getID()); } \
	}

namespace util {
namespace io {
namespace event {

namespace impl {

struct my_event {
	id::Ident id;
	io::Callback callback;
	struct event *event;
};

struct user_data {
	struct my_event *event;
	Dispatcher *dispatcher;
};

static void event_log_cb( int severity, const char *msg ) {
	switch (severity) {
		case EVENT_LOG_DEBUG: LDEBUG("io::event::Dispatcher", msg); break;
		case EVENT_LOG_MSG:   LINFO("io::event::Dispatcher", msg); break;
		case EVENT_LOG_WARN:  LWARN("io::event::Dispatcher", msg); break;
		case EVENT_LOG_ERR:   LERROR("io::event::Dispatcher", msg); break;
	}
}

static void empty_cb( evutil_socket_t /*fd*/, short /*what*/, void * /*arg*/ ) {
}

static void wakeup_cb( evutil_socket_t /*fd*/, short /*what*/, void *arg ) {
	Dispatcher *disp = (Dispatcher*) arg;
	DTV_ASSERT(disp);
	disp->wakeup();
}

static void timeout_cb( evutil_socket_t /*fd*/, short /*what*/, void *arg ) {
	struct user_data *udata = (struct user_data*) arg;
	DTV_ASSERT(udata);
	id::Ident id(udata->event->id);
	udata->event->callback( id );
	udata->dispatcher->stopTimer( id );
	free(udata);
}

static void read_cb( evutil_socket_t /*fd*/, short /*what*/, void *arg ) {
	struct my_event *event = (struct my_event *) arg;
	DTV_ASSERT(event);
	id::Ident id(event->id);
	event->callback( id );
}

class EventFinder {
public:
	EventFinder( id::ID_TYPE id ) : _id(id) {}

	bool operator()( const struct my_event *ev ) const {
		return ev->id->getID() == _id;
	}

private:
	id::ID_TYPE _id;
};

}

Dispatcher::Dispatcher()
{
	_tasks = new task::DispatcherImpl();
	_evBase = NULL;
	_loopEv = NULL;
}

Dispatcher::~Dispatcher()
{
	delete _tasks;
}

//	Initialization
bool Dispatcher::initialize() {
	_tasks->onPost( boost::bind(&Dispatcher::onPost,this) );

	int err=0;
#ifdef _WIN32
    err = evthread_use_windows_threads();
#else
    err = evthread_use_pthreads();
#endif
    if (err) {
		LERROR("io::event::Dispatcher", "Fail to setup threading support");
		return false;
    }

	event_set_log_callback(impl::event_log_cb);
	_evBase = event_base_new();

	// Virtual event to prevent loop for exiting when there aren't pending events.
	// NOTE: use EVLOOP_NO_EXIT_ON_EMPTY on version 2.1 or later
	struct timeval fiveSec = {5,0};
	_loopEv = event_new(_evBase, -1, EV_PERSIST, impl::empty_cb, NULL);
	DTV_ASSERT(_loopEv);
	if (event_add(_loopEv, &fiveSec)!=0) {
		LWARN("io::event::Dispatcher", "Fail to add virtual event to the loop");
	}

	//	Register into dispatcher
	_tasks->registerTarget( this, "io::event::Dispatcher" );

	return true;
}

void Dispatcher::finalize() {
	//	Run all tasks in queue
	_tasks->runAll();

	//	Remove io from dispatcher
	_tasks->unregisterTarget( this );

	//	Cleanup all events
	while (!_io.empty()) {
		ListOfEvent::iterator it = _io.begin();
		LWARN("io::event::Dispatcher", "stopping lost io event: id=%d", (*it)->id->getID());
		stopIO( (*it)->id );
	}

	while (!_timers.empty()) {
		ListOfEvent::iterator it = _timers.begin();
		LWARN("io::event::Dispatcher", "stopping lost timer event: id=%d", (*it)->id->getID());
		stopTimer( (*it)->id );
	}

	while (!_sockets.empty()) {
		ListOfEvent::iterator it = _sockets.begin();
		LWARN("io::event::Dispatcher", "stopping lost socket event: id=%d", (*it)->id->getID());
		stopSocket( (*it)->id );
	}

	event_del(_loopEv);
	event_free(_loopEv);
	_loopEv = NULL;

	event_base_free(_evBase);
	_evBase = NULL;

	{	//	Clean task callback
		util::task::Type empty;
		_tasks->onPost( empty );
	}
}

void Dispatcher::run() {
	LINFO( "io::event::Dispatcher", "Run begin" );

	int err = event_base_loop(_evBase, 0);
	if (err) {
		LWARN("io::event::Dispatcher", "Dispatcher loop end with error: %d", err);
	}

	LINFO( "io::event::Dispatcher", "Run end" );
}

void Dispatcher::exit() {
	int err = event_base_loopbreak(_evBase);
	if (err) {
		LWARN("io::event::Dispatcher", "Dispatcher exit error: %d", err);
	}
}

//	Dispatcher
task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}

void Dispatcher::onPost() {
	//	wakeup loop
	if (event_base_once(_evBase, -1, EV_TIMEOUT, impl::wakeup_cb, this, NULL)!=0) {
		LWARN("io::event::Dispatcher", "Fail to add wakeup event to the loop");
	}
}

//	Timer events
id::Ident Dispatcher::addTimer( int ms, const Callback &callback ) {
	LDEBUG("io::event::Dispatcher", "Add timer: ms=%d", ms);

	//	Create timer
	struct impl::my_event *timer = (struct impl::my_event*) calloc(1, sizeof(struct impl::my_event));
	if (!timer) {
		LERROR("io::event::Dispatcher", "cannot create timer event");
		return id::Ident();
	}
	else {
		//	Setup event
		INIT( timer, callback );

		//	Add to list
		_timers.push_back( timer );

		{	//	Add to loop
			struct impl::user_data *udata = (struct impl::user_data*) calloc(1, sizeof(struct impl::user_data));
			udata->event = timer;
			udata->dispatcher = this;

			timer->event = event_new(_evBase, -1, 0, impl::timeout_cb, udata);
			if (!timer->event) {
				LWARN("io::event::Dispatcher", "Fail to add timer");
			}

			struct timeval timeout = {ms/1000, (ms%1000) * 1000};
			if (event_add(timer->event, &timeout)!=0) {
				LWARN("io::event::Dispatcher", "Fail to add timer");
			}
		}

		return timer->id;
	}
}

void Dispatcher::stopTimer( id::Ident &id ) {
	STOP_EVENT( _timers, id );
	id.reset();
}

//	IO events
id::Ident Dispatcher::addIO( DESCRIPTOR fd, const Callback &callback ) {
	struct impl::my_event *io = (struct impl::my_event*) calloc(1, sizeof(struct impl::my_event));
	if (!io) {
		LERROR("io::event::Dispatcher", "cannot create io event");
		return id::Ident();
	}
	else {
		//	Setup event
		INIT( io, callback );

		//	Add to list
		_io.push_back( io );

		{	//	Add to loop
			io->event = event_new(_evBase, (intptr_t)fd, EV_READ | EV_PERSIST, impl::read_cb, io);
			if (!io->event) {
				LWARN("io::event::Dispatcher", "Fail to add io");
			}

			if (event_add(io->event, NULL)!=0) {
				LWARN("io::event::Dispatcher", "Fail to add io");
			}
		}

		return io->id;
	}
}

void Dispatcher::stopIO( id::Ident &id ) {
	STOP_EVENT(_io, id);
	id.reset();
}

//	Socket events
id::Ident Dispatcher::addSocket( SOCKET sock, const Callback &callback ) {
	struct impl::my_event *ev = (struct impl::my_event*) calloc(1, sizeof(struct impl::my_event));
	if (!ev) {
		LERROR("io::event::Dispatcher", "cannot create socket event");
		return id::Ident();
	}
	else {
		//	Setup event
		INIT( ev, callback );

		//	Add to list
		_sockets.push_back( ev );

		{	//	Add to loop
			ev->event = event_new(_evBase, sock, EV_READ | EV_PERSIST, impl::read_cb, ev);
			if (!ev->event) {
				LWARN("io::event::Dispatcher", "Fail to add socket");
			}

			if (event_add(ev->event, NULL)!=0) {
				LWARN("io::event::Dispatcher", "Fail to add socket");
			}
		}

		return ev->id;
	}
}

void Dispatcher::stopSocket( id::Ident &id ) {
	STOP_EVENT(_sockets, id);
	id.reset();
}

//	Aux
void Dispatcher::wakeup() {
	_tasks->runAll();
}

}
}
}
