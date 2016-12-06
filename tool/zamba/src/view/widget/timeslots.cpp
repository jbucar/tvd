/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

/*
 * TimeSlots.cpp
 *
 *  Created on: Jul 13, 2011
 *      Author: gonzalo
 */

#include "timeslots.h"

namespace wgt
{

TimeSlots::TimeSlots()
	: _from(0)
	, _visible(0)
{}

void TimeSlots::visibleSlots(int n)
{
	_visible = n;
	_from = 0;
	fillVisibleSlots();
}

void TimeSlots::reset()
{
	_from = 0;
	fillVisibleSlots();
}

void TimeSlots::setSlots(const std::vector<TimeSlots::time_slot>& slots)
{
	_slots = slots;
	fillVisibleSlots();
}

void TimeSlots::fillVisibleSlots()
{
	_visibleSlots.clear();
	for (size_t i=_from; i<_from+_visible && i < _slots.size(); ++i) {
		_visibleSlots.push_back(_slots[i]);
	}
}

void TimeSlots::moveSlotsTo(boost::posix_time::ptime time)
{
	size_t f = _slots.size() - 1;
	for (; f > 0  && !_slots[f].contains(time); --f)
		;
	if (0 < f && f < _slots.size()) {
		_from = f;
	}
	fillVisibleSlots();
}

bool TimeSlots::shiftRight()
{
	if (_from+_visible < _slots.size()) {
		++_from;
		fillVisibleSlots();
		return true;
	}
	return false;
}

bool TimeSlots::shiftLeft()
{
	if (_from > 0) {
		--_from;
		fillVisibleSlots();
		return true;
	}
	return false;
}

const std::vector<TimeSlots::time_slot>& TimeSlots::allSlots()
{
	return _slots;
}

const std::vector<TimeSlots::time_slot>& TimeSlots::visibleSlots()
{
	return _visibleSlots;
}

boost::posix_time::ptime TimeSlots::visibleFrom()
{
	return _visibleSlots.front().begin();
}

boost::posix_time::ptime TimeSlots::visibleTo()
{
	return _visibleSlots.back().last();
}

size_t TimeSlots::visibleFromIndex()
{
	return _from;
}

size_t TimeSlots::visibleToIndex()
{
	return _from + _visible;
}

const TimeSlots::time_slot& TimeSlots::slotAt(size_t ix)
{
	return _slots[std::min(ix,_slots.size()-1)];
}

int TimeSlots::visible()
{
	return _visible;
}

TimeSlots::time_slot TimeSlots::visibleRange()
{
	return boost::posix_time::time_period(visibleFrom(), visibleTo());
}


} // wgt
