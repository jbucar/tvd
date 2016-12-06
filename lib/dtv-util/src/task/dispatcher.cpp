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
#include "../log.h"
#include "../mcr.h"

namespace util {
namespace task {

namespace impl {

class TargetImpl {
public:
	TargetImpl( Target target, const std::string &name )
		: _target(target), _name(name), _enabled(true) {}

	Target target() const { return _target; }
	bool enabled() const { return _enabled; }
	void enable(bool st) { _enabled = st; }
	const std::string &name() const { return _name; }
	void name( const std::string &name ) { _name = name; }

private:
	Target _target;
	std::string _name;
	bool _enabled;
};

struct TargetFinder {
	TargetFinder( Target target ) : _target(target) {}

	bool operator()( const TargetImpl *t ) const {
		return t->target() == _target;
	}

	Target _target;
};

}


Dispatcher::Dispatcher()
{
	registerTarget( NULL, "util::task" );
}

Dispatcher::~Dispatcher()
{
	CLEAN_ALL( impl::TargetImpl *, _targets );
}

void Dispatcher::registerTarget( Target target, const std::string &name ) {
	LDEBUG( "task", "Register target: target=%p, name=%s", target, name.c_str() );
	_mutex.lock();

	Targets::const_iterator it = std::find_if(
		_targets.begin(),
		_targets.end(),
		impl::TargetFinder(target)
	);
	impl::TargetImpl *t = (it != _targets.end()) ? (*it) : NULL;

	if (t) {
		t->enable(true);
		t->name(name);
	} else {
		_targets.push_back( new impl::TargetImpl(target,name) );
	}
	_mutex.unlock();
}

std::string Dispatcher::name( Target target ) {
	LDEBUG( "task", "name: target=%p", target);

	Targets::const_iterator it = std::find_if(
		_targets.begin(),
		_targets.end(),
		impl::TargetFinder(target)
	);

	return (it != _targets.end()) ? (*it)->name() : "";
}

void Dispatcher::unregisterTarget( Target target ) {
	LDEBUG( "task", "Unregister target: target=%p", target);
	_mutex.lock();
	Targets::const_iterator it = std::find_if(
		_targets.begin(),
		_targets.end(),
		impl::TargetFinder(target)
	);
	if (it != _targets.end()) {
		(*it)->enable(false);
	}
	_mutex.unlock();
}

void Dispatcher::run( Target target, const Type &task ) {
	_mutex.lock();
	Targets::const_iterator it = std::find_if(
		_targets.begin(),
		_targets.end(),
		impl::TargetFinder(target)
	);
	impl::TargetImpl *t = (it != _targets.end()) ? (*it) : NULL;
	_mutex.unlock();

	if (t) {
		if (t->enabled()) {
			try {
				task();
			} catch ( const std::exception &e ) {
				LWARN("task", "Catch exception in task: target=%s, error=%s", t->name().c_str(), e.what());
			}
		}
		else {
			LWARN("task", "Target disabled: name=%s", t->name().c_str());
		}
	}
	else {
		LWARN("task", "Target not found: %p", target);
	}
}

void Dispatcher::clear() {
	_mutex.lock();
	CLEAN_ALL( impl::TargetImpl *, _targets );
	_mutex.unlock();
}

}
}

