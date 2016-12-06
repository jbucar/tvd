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
#include <util/task/dispatcherimpl.h>
#include <util/id/pool.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <gtk/gtk.h>

namespace canvas {
namespace gtk {

namespace impl {

static gboolean ev_rcv( GIOChannel * /*chan*/, GIOCondition /*condition*/, Dispatcher::IOEvent *io ) {
	DTV_ASSERT(io);
	Dispatcher *disp = (Dispatcher *)io->context();
	DTV_ASSERT(disp);	
	disp->onDispatchIO( io );
	return TRUE;
}

static gboolean timeoutCallback( Dispatcher::Timer *timer ) {
	DTV_ASSERT(timer);
	Dispatcher *disp = (Dispatcher *)timer->context();
	DTV_ASSERT(disp);	
	disp->onDispatchTimer( timer );
	return FALSE;
}

static gboolean onTaskCallback( Dispatcher *disp ) {
	DTV_ASSERT(disp);
	disp->onTaskEvent();
	return FALSE;
}

}

Dispatcher::Dispatcher()
{
	_idleFunc = -1;
	_mainLoopRunning = false;
	_pool = new util::id::Pool("gtk");

	// Create task dispatcher
	_tasks = new util::task::DispatcherImpl();
	_tasks->onPost( boost::bind(&Dispatcher::onPost,this) );
}

Dispatcher::~Dispatcher()
{
	DTV_ASSERT(!_mainLoopRunning);
	delete _pool;
	delete _tasks;	
}

//	Initialization
bool Dispatcher::initialize() {
	_tasks->registerTarget( this, "canvas::gtk::System" );
	gtk_init(NULL, NULL);
	return true;
}

void Dispatcher::finalize() {
	_tasks->runAll();
	_tasks->unregisterTarget( this );	
}

//	Run/exit io event notification loop
void Dispatcher::run() {
	LINFO("gtk::System", "Main loop begin");
	
	_mainLoopRunning=true;

	//	Run loop
	gtk_main();

	LINFO("gtk::System", "Main loop end");
}

void Dispatcher::exit() {
	if (_mainLoopRunning) {
		_mainLoopRunning=false;
		//	Wakeup thread
		_tasks->post( this, boost::bind(&Dispatcher::onExit,this) );
	}
}

//	Dispatcher
util::task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}
 
void Dispatcher::onExit() {
 	gtk_main_quit();
}

void Dispatcher::onDispatchIO( IOEvent *io ) {
	dispatchEvent( _ioList, io );
}

void Dispatcher::onTaskEvent() {
	_tasks->runAll();
}
 
void Dispatcher::onPost() {
 	g_timeout_add( 1, (GSourceFunc)impl::onTaskCallback, (gpointer)this );
}

//	Timer events
util::id::Ident Dispatcher::addTimer( int ms, const util::io::Callback &callback ) {
	Timer *ev = new Timer(_pool->alloc(), callback, this);
	int id=g_timeout_add( ms, (GSourceFunc)impl::timeoutCallback, (gpointer)ev );
	return startEvent( ev, id != 0, id, _timerList );
}

void Dispatcher::stopTimer( util::id::Ident &id ) {
	int timerID;
	if (stopEvent( id, _timerList, timerID )) {
		g_source_remove( timerID );
	}	
}

void Dispatcher::onDispatchTimer( Timer *timer ) {
	//	Two pass, becouse in dispatch callback can unregister timer!
	util::id::Ident id=dispatchEvent( _timerList, timer );
	stopTimer( id );
}

//	IO events
util::id::Ident Dispatcher::addIO( util::DESCRIPTOR fd, const util::io::Callback &callback ) {
#ifdef _WIN32
	UNUSED(fd);
	UNUSED(callback);
	return util::id::Ident();
#else
	IOEvent *ev = new IOEvent(_pool->alloc(),callback,this);
	GIOChannel *chan = g_io_channel_unix_new(fd);
	int watchID=g_io_add_watch( chan, (GIOCondition) (G_IO_IN | G_IO_PRI), (GIOFunc)impl::ev_rcv, (gpointer)ev );
	g_io_channel_unref( chan );
	return startEvent( ev, watchID != 0, watchID, _ioList );
#endif
}

void Dispatcher::stopIO( util::id::Ident &id ) {
	int watchID;
	if (stopEvent( id, _ioList, watchID )) {
		g_source_remove( watchID );
	}
}

//	Socket events
util::id::Ident Dispatcher::addSocket( util::SOCKET sock, const util::io::Callback &callback ) {
#ifdef _WIN32
	IOEvent *ev = new IOEvent(_pool->alloc(),callback,this);
	GIOChannel *chan = g_io_channel_win32_new_socket (sock);
	int watchID=g_io_add_watch( chan, (GIOCondition) (G_IO_IN | G_IO_PRI), (GIOFunc)impl::ev_rcv, (gpointer)ev );
	g_io_channel_unref( chan );
	return startEvent( ev, watchID != 0, watchID, _ioList );
#else	
	return addIO( sock, callback );
#endif	
}

void Dispatcher::stopSocket( util::id::Ident &id ) {
	stopIO( id );
}

}
}
	
