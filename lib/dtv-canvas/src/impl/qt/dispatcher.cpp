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
#include "mytimer.h"
#include "myqioevent.h"
#include <util/assert.h>
#include <util/task/dispatcher.h>
#include <util/cfg/cfg.h>
#include <util/id/pool.h>
#include <QApplication>

namespace canvas {
namespace qt {

//	Begin implementation
namespace impl {
static int argc = 1;
static const char *argv[] = { "hello", NULL };
}

class QTask : public QEvent {
public:
	QTask( int taskEvent, util::task::Target tgt, const util::task::Type &theTask )
		: QEvent( (QEvent::Type)taskEvent ), target( tgt ), task(theTask) {}

	util::task::Target target;
	util::task::Type task;
};

class QDispatcher;

class SysApplication : public QApplication {
public:
	SysApplication( int &argc, char **argv );
	~SysApplication();

	void postTask( QTask *task ) { postEvent( this, task ); }
	virtual void customEvent(QEvent * e);
	util::task::Dispatcher *tasks();

private:
	QDispatcher *_tasks;
};

class QDispatcher : public util::task::Dispatcher {
public:
	QDispatcher( SysApplication *sys ) {
		_sys = sys;
		_taskEvent = QEvent::registerEventType();
	}

	virtual void post( util::task::Target target, const util::task::Type &task ) {
		_sys->postTask( new QTask(_taskEvent,target,task) );
	}
	
	void onEvent( QEvent *e ) {
		if (e->type() == _taskEvent) {
			QTask *tmp = (QTask *)e;
			run( tmp->target, tmp->task );
		}
	}

private:
	SysApplication *_sys;
	int _taskEvent;
};


SysApplication::SysApplication( int &argc, char **argv )
	: QApplication( argc, argv, true )
{
	_tasks = new QDispatcher(this);
}

SysApplication::~SysApplication()
{
	delete _tasks;
}

void SysApplication::customEvent(QEvent * e) {
	_tasks->onEvent( e );
}

util::task::Dispatcher *SysApplication::tasks() {
	return _tasks;
}
//	End implementation


Dispatcher::Dispatcher()
{
	_qt = NULL;
	_pool = new util::id::Pool("qt");
}

Dispatcher::~Dispatcher()
{
	delete _pool;
	DTV_ASSERT(!_qt);
}

//	Initialization
bool Dispatcher::initialize() {
	//	Setup qt application
	impl::argv[0] = util::cfg::getValue<std::string>("gui.window.title").c_str();
	_qt = new SysApplication( impl::argc, (char **)impl::argv );
	return true;
}
	
void Dispatcher::finalize() {
	DEL(_qt);
}

//	Run/exit io event notification loop
void Dispatcher::run() {
	_qt->exec();
}

void Dispatcher::exit() {
	_qt->quit();	
}

//	Dispatcher
util::task::Dispatcher *Dispatcher::dispatcher() const {
	return _qt->tasks();
}

//	Timer events
util::id::Ident Dispatcher::addTimer( int ms, const util::io::Callback &callback ) {
	MyTimer *timer = new MyTimer(this);
	Timer *ev = new Timer(_pool->alloc(), callback, this);
	return startEvent( ev, timer->attach(ms), timer, _timerList );
}

void Dispatcher::stopTimer( util::id::Ident &id ) {
	MyTimer *timer;
	if (stopEvent( id, _timerList, timer )) {
		timer->cancel();
		delete timer;
	}
}

void Dispatcher::onTimer( MyTimer *timer ) {
	util::id::Ident id=dispatchEventByValue( _timerList, timer );
	stopTimer( id );
}

//	IO events
util::id::Ident Dispatcher::addIO( util::DESCRIPTOR fd, const util::io::Callback &callback ) {
#ifdef _WIN32
	UNUSED(fd);
	UNUSED(callback);
	return util::id::Ident();
#else
	IOEvent *ev = new IOEvent(_pool->alloc(), callback, this);
	QSocketNotifier *sn = new QSocketNotifier(fd, QSocketNotifier::Read);
	sn->setEnabled(true);
	MyQIOEvent *qev = new MyQIOEvent(ev, sn);
	QObject::connect( sn, SIGNAL(activated(int)), qev, SLOT(dispatch()) );
	return startEvent( ev, sn->isEnabled(), qev, _ioList );
#endif
}

void Dispatcher::stopIO( util::id::Ident &id ) {
#ifdef _WIN32
	UNUSED(id);
#else
	MyQIOEvent *qev = NULL;
	stopEvent( id, _ioList, qev );
	DEL(qev);
#endif
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
	
