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

#pragma once

#include "types.h"
#include <util/id/pool.h>
#include <util/log.h>
#include <vector>

namespace canvas {

template<typename T>
class Event {
public:
	Event( const util::id::Ident &id, const EventCallback &callback, void *context ) : _callback(callback), _id(id), _context(context) {}
	explicit Event( const EventCallback &callback ) : _callback(callback) {}
	virtual ~Event() {}

	void dispatch() { _callback( _id ); }
	const util::id::Ident &id() const { return _id; }
	void *context() const { return _context; }
	T value() const { return _value; }
	void value( const T &val ) { _value = val; }
	
	void setID( const util::id::Ident &id, const T &value ) {
		_id = id;
		_value = value;
	}
	
private:
	EventCallback _callback;
	util::id::Ident _id;
	T _value;
	void *_context;
};

template<typename Ev>
struct EventFinder {
	EventFinder( util::id::Ident &id ) : _id(id) {}

	bool operator()( const Ev *ev ) const { return ev->id() == _id; }

	util::id::Ident &_id;

private:
	EventFinder &operator=( const EventFinder & /*ef*/ ) {
		return *this;
	}
};

template<typename Ev, typename V>
struct EventFinderByValue {
	EventFinderByValue( const V &value ) : _value(value) {}
	bool operator()( const Ev *ev ) const { return ev->value() == _value; }
	const V &_value;
};

template<typename Ev, typename V>
inline util::id::Ident startEvent( Ev *ev, bool result, const V &value, std::vector<Ev *> &l ) {
	util::id::Ident	id;
	if (result) {
		ev->value(value);
		l.push_back(ev);
		id=ev->id();
	}
	else {
		LWARN( "System", "Cannot start event" );
		delete ev;
	}
	return id;
}

template<typename Ev, typename V>
inline bool stopEvent( util::id::Ident &id, std::vector<Ev *> &l, V &value ) {
	bool result=false;

	if (util::id::isValid(id)) {
		typename std::vector<Ev *>::iterator it = find_if(
			l.begin(),
			l.end(),
			EventFinder<Ev>(id)
		);
		if (it != l.end()) {
			Ev *ev = (*it);
			value = ev->value();
			delete ev;
			l.erase(it);
			result=true;
		}
		else {
			LWARN( "System", "Cannot stop event" );
		}

		id.reset();
	}
	
	return result;
}

template<typename Ev, typename V>
inline util::id::Ident dispatchEventByValue( std::vector<Ev *> &l, V &value ) {
	util::id::Ident	id;
	typename std::vector<Ev *>::const_iterator it = find_if(
		l.begin(),
		l.end(),
		EventFinderByValue<Ev,V>(value)
	);
	if (it != l.end()) {
		Ev *ev = (*it);
		id = ev->id();
		ev->dispatch();
	}
	else {
		LWARN( "System", "Cannot stop event" );		
	}
	return id;
}

template<typename Ev>
inline util::id::Ident dispatchEvent( std::vector<Ev *> &l, Ev *ev ) {
	util::id::Ident	id;
	typename std::vector<Ev *>::const_iterator it = find(
		l.begin(),
		l.end(),
		ev
	);
	if (it != l.end()) {	//	Is valid?
		id = ev->id();
		ev->dispatch();
	}
	else {
		LWARN( "System", "Cannot stop event" );		
	}
	return id;
}


}
