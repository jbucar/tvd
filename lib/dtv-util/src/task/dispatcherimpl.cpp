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

#include "dispatcherimpl.h"
#include <boost/foreach.hpp>

namespace util {
namespace task {

class Task {
public:
	Task( Target tgt, const Type &fnc ) { target = tgt; method = fnc; }
	~Task() {}

	Target target;
	Type method;
};

DispatcherImpl::DispatcherImpl()
{
}

DispatcherImpl::~DispatcherImpl()
{
}

void DispatcherImpl::onPost( const Type &task ) {
	_onPost = task;
}

void DispatcherImpl::post( Target target, const Type &task ) {
	_tMutex.lock();
	_tasks.push( new Task( target, task ) );
	_tMutex.unlock();

	if (!_onPost.empty()) {
		_onPost();
	}
}
	
void DispatcherImpl::runAll() {
	Task *task;
	std::vector<Task *> tasks;

	//	Get all task to run
	_tMutex.lock();
	while (!_tasks.empty()) {
		task = _tasks.front();
		_tasks.pop();
		tasks.push_back( task );
	}
	_tMutex.unlock();

	//	Run tasks
	BOOST_FOREACH( Task *task, tasks ) {
		post( task );
	}	
}

void DispatcherImpl::runOne() {
	Task *task=NULL;

	//	Get all task to run
	_tMutex.lock();
	while (!_tasks.empty()) {
		task = _tasks.front();
		_tasks.pop();
	}
	_tMutex.unlock();

	if (task) {
		post( task );
	}
}

void DispatcherImpl::post( Task *task ) {
	run( task->target, task->method );
	delete task;
}
	
void DispatcherImpl::clear() {
	Dispatcher::clear();
	
	Task *task;
	_tMutex.lock();
	while (!_tasks.empty()) {
		task = _tasks.front();
		_tasks.pop();
		delete task;
	}
	_tMutex.unlock();		
}

}
}

