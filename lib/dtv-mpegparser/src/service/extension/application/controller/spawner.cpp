/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "spawner.h"
#include "runtask.h"
#include "../application.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/process/process.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace tuner {
namespace app {

Spawner::Spawner()
{
}

Spawner::~Spawner()
{
	DTV_ASSERT(_tasks.empty());
}

//	Spawner methods
void Spawner::run( RunTask *item ) {
	DTV_ASSERT( item );

	LDEBUG( "Spawner", "Run task: name=%s", item->app()->name().c_str() );

	//	Add item into tasks
	Task *task = new Task();
	task->item = item;
	task->proc = NULL;
	_tasks[item] = task;

	//	Try start
	runTask( task );
}

void Spawner::stop( RunTask *item, bool kill ) {
	Tasks::iterator it= _tasks.find( item );
	if (it != _tasks.end()) {
		Task *task = (*it).second;

		//	Stop and remove task from list
		stopTask( task, kill );

		_tasks.erase( it );
		delete task;
	}
}

void Spawner::runTask( Task *task ) {
	if (!running()) {
		RunTask *item = task->item;
		DTV_ASSERT(item);
		DTV_ASSERT(!task->proc);

		//	Setup runtask
		Application *app = task->item->app();
		DTV_ASSERT(app);
		app->starting();

		//	Setup proc
		util::Process *proc = new util::Process( item->cmd() );

		//	Setup env
		proc->clearEnvironment();
		BOOST_FOREACH( const RunTask::Params::value_type &val, item->env() ) {
			proc->addToEnvironment( val.first, val.second );
		}

		//	Setup parameters
		proc->clearParams();
		BOOST_FOREACH( const RunTask::Params::value_type &val, item->params() ) {
			std::string param = "--" + val.first + "=" + val.second;
			proc->addParam( param );
		}

		LDEBUG( "Spawner", "Start process: cmd=%s, pid=%d", item->cmd().c_str(), proc->pid() );
		if (proc->run()) {
			task->proc = proc;
			app->started();
		}
		else {
			delete proc;
			app->stop();
		}
	}
}

void Spawner::stopTask( Task *task, bool kill ) {
	if (task->proc) {
		util::Process *proc = task->proc;
		LDEBUG( "Spawner", "Stop process: pid=%d", proc->pid() );

		//	Stop process if necesary
		if (proc->isRunning()) {
			proc->kill( kill ? 0 : task->item->app()->exitTimeout() );
		}

		delete proc;
		task->proc = NULL;
	}
}

int Spawner::running() const {
	int count=0;
	BOOST_FOREACH( const Tasks::value_type &item, _tasks ) {
		if (item.second->proc && item.second->proc->isRunning()) {
			count++;
		}
	}
	return count;
}

}
}

