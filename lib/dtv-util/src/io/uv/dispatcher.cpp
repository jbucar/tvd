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
#include <uv.h>

namespace util {
namespace io {
namespace uv {

namespace impl {

static void wakeup_cb( uv_async_t *handle ) {
	Dispatcher *sys = (Dispatcher *)handle->data;
	DTV_ASSERT(sys);
	sys->wakeup();
}

static void wakeup_close_cb(uv_handle_t* handle) {
	uv_async_t *real_handle = (uv_async_t *)handle;
	free(real_handle);
}

struct MyDataS {
	Dispatcher *disp;
	id::Ident id;
	io::Callback callback;
};
typedef MyDataS MyData;

MyData *initUser( const id::Ident &id, Dispatcher *disp, const io::Callback &callback ) {
	MyData *d = (MyData *)calloc( 1, sizeof(MyData) );
	if (!d) {
		LERROR("io::uv::Dispatcher", "Cannot create event");
		return d;
	}

	d->disp = disp;
	d->id = id;
	d->callback = callback;
	return d;
}

void freeUser( void *ptr ) {
	MyData *data = (MyData *)ptr;
	data->id.reset();
	free(data);
}

static void timer_cb( uv_timer_t *timer ) {
	MyData *data = (MyData *)timer->data;
	DTV_ASSERT(data);
	id::Ident id(data->id);
	data->callback( id );
	data->disp->stopTimer( id );
}

static void io_cb( uv_poll_t *poll, int /*status*/, int /*events*/ ) {
	MyData *data = (MyData *)poll->data;
	DTV_ASSERT(data);
	id::Ident id(data->id);
	data->callback( id );
	data->disp->stopIO( id );
}

template<class T>
class EventFinder {
public:
	EventFinder( id::ID_TYPE id ) : _id(id) {}

	bool operator()( const T *io ) const {
		return static_cast<MyData *>(io->data)->id->getID() == _id;
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

	if (isEmbedded()) {
		_loop = uv_default_loop();
	}
	else {
		//	Create loop
		_loop = uv_loop_new();
		if (!_loop) {
			LERROR("io::uv::Dispatcher", "cannot create loop");
			return false;
		}
	}

	//	Create and initialize wakeup watcher
	_wakeup = (uv_async_t *)calloc( 1, sizeof(uv_async_t) );
	if (!_wakeup) {
		if (!isEmbedded()) {
			free(_loop);
		}
		LWARN("io::uv::Dispatcher", "cannot create async event");
		return false;
	}
	_wakeup->data = this;
	uv_async_init(_loop, _wakeup, impl::wakeup_cb);

	//	Register into dispatcher
	_tasks->registerTarget( this, "io::uv::Dispatcher" );

	return true;
}

void Dispatcher::finalize() {
	if (isEmbedded()) {
		//	Close async
		uv_close( (uv_handle_t *)_wakeup, impl::wakeup_close_cb );
	}

	//	Run all tasks in queue
	_tasks->runAll();

	//	Remove io from dispatcher
	_tasks->unregisterTarget( this );

	//	Cleanup all events
	while (!_ioEvents.empty()) {
		Events::iterator it = _ioEvents.begin();
		LWARN("io::uv::Dispatcher", "stopping lost IO event");
		stopIO( static_cast<impl::MyData *>((*it)->data)->id );
	}

	while (!_timers.empty()) {
		Timers::iterator it = _timers.begin();
		LWARN("io::uv::Dispatcher", "stopping lost timer event");
		stopTimer( static_cast<impl::MyData *>((*it)->data)->id );
	}

	if (!isEmbedded()) {
		//	Destroy loop
		uv_loop_close( _loop );

		free(_wakeup);
	}
	_loop = NULL;
	_wakeup = NULL;

	{	//	Clean task callback
		util::task::Type empty;
		_tasks->onPost( empty );
	}
}

void Dispatcher::run() {
	LINFO( "io::uv::Dispatcher", "Run begin" );

	if (!isEmbedded()) {
		//	Wait for events to arrive
		uv_run (_loop, UV_RUN_DEFAULT);
	}
	else {
		LERROR( "io::uv::Dispatcher", "Cannot run in embedded loop" );
		assert(false);
	}

	LINFO( "io::uv::Dispatcher", "Run end" );
}

void Dispatcher::exit() {
	LINFO( "io::uv::Dispatcher", "Exit" );
	uv_stop( _loop );
}

//	Dispatcher
task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}

void Dispatcher::onPost() {
	//	wakeup loop
	uv_async_send( _wakeup );
}

//	Timer events
id::Ident Dispatcher::addTimer( int ms, const Callback &callback ) {
	LDEBUG("io::uv::Dispatcher", "Add timer: ms=%d", ms);

	// Setup user info
	impl::MyData *d = impl::initUser( allocID(), this, callback );
	if (!d) {
		return id::Ident();
	}

	//	Create timer
	uv_timer_t *timer = (uv_timer_t *)calloc(1, sizeof(uv_timer_t));
	uv_timer_init(_loop,timer);
	timer->data = d;

	//	Add to timers list
	_timers.push_back( timer );

	//	Start timer
	uv_timer_start(timer,impl::timer_cb,ms,0);

	return d->id;
}

void Dispatcher::stopTimer( id::Ident &id ) {
	if (id::isValid(id)) {
		Timers::iterator it=std::find_if(
			_timers.begin(),
			_timers.end(),
			impl::EventFinder<uv_timer_t>(id->getID())
		);
		if (it != _timers.end()) {
			uv_timer_t *timer = (*it);
			_timers.erase(it);
			uv_timer_stop( timer );
			impl::freeUser( timer->data );
			free(timer);
		}
		id.reset();
	}
	else {
		LWARN("io::uv::Dispatcher", "invalid timer %ld", id->getID());
	}
}

//	IO events
id::Ident Dispatcher::addIO( DESCRIPTOR fd, const Callback &callback ) {
	// Setup user info
	impl::MyData *d = impl::initUser( allocID(), this, callback );
	if (!d) {
		return id::Ident();
	}

	//	Create poll event
	uv_poll_t *io = (uv_poll_t *)calloc( 1, sizeof(uv_poll_t) );
	uv_poll_init( _loop, io, fd );
	io->data = d;

	//	Add to timers list
	_ioEvents.push_back( io );

	//	Start poll
	uv_poll_start( io, UV_READABLE, impl::io_cb );

	return d->id;
}

void Dispatcher::stopIO( id::Ident &id ) {
	if (id::isValid(id)) {
		Events::iterator it=std::find_if(
			_ioEvents.begin(),
			_ioEvents.end(),
			impl::EventFinder<uv_poll_t>(id->getID())
		);
		if (it != _ioEvents.end()) {
			uv_poll_t *io = (*it);
			_ioEvents.erase(it);
			uv_poll_stop( io );
			impl::freeUser( io->data );
			free(io);
		}
		id.reset();
	}
	else {
		LWARN("io::uv::Dispatcher", "invalid io event %ld", id->getID());
	}
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

