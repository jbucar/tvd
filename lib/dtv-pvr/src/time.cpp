/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "time.h"
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

namespace pvr {

namespace time {

static Time *impl = NULL;

void init( Time *timeSource ) {
	delete impl;
	impl = timeSource;
}

void fin() {
	DEL(impl);
}

bool isInitialized() {
	return impl ? true : false;
}

Time *clock() {
	if (!impl) {
		LWARN( "time", "Using local time as time source" );
		init( new Time(0,"ARG") );
	}
	return impl;
}

std::string asString( const bpt::ptime &p ) {
	return util::format( "%d/%d %d:%d:%d",
		p.date().day().as_number(),
		p.date().month().as_number(),
		p.time_of_day().hours(),
		p.time_of_day().minutes(),
		p.time_of_day().seconds()
	);
}

}	//	namespace time

Time::Time( int offset, const std::string &countryCode )
	: _offset(offset), _countryCode(countryCode)
{
}

Time::~Time()
{
}

//	Getters
bool Time::isValid() const {
	return true;
}

void Time::get( bpt::ptime &value ) {
	value = bpt::microsec_clock::local_time();
}

const std::string &Time::countryCode() const {
	return _countryCode;
}

bpt::ptime &Time::localTime( bpt::ptime &value ) const {
	if (_offset) {
		value += bpt::hours(_offset);
	}
	return value;
}

void Time::notify( bool isValid ) {
	_signal( isValid );
}

//  Signals
Time::TimeValidSignal &Time::onTimeValidChanged() {
	return _signal;
}

}

