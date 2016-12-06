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
 * TimeSlots.h
 *
 *  Created on: Jul 13, 2011
 *      Author: gonzalo
 */

#ifndef TIMESLOTS_H_
#define TIMESLOTS_H_

#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace wgt
{

class TimeSlots
{
public:
	typedef boost::posix_time::time_period time_slot;

	TimeSlots();
	void visibleSlots(int n);
	void setSlots(const std::vector<time_slot>& slots);
	bool shiftRight();
	bool shiftLeft();
	void moveSlotsTo(boost::posix_time::ptime time);
	int visible();

	const std::vector<time_slot>& allSlots();
	const std::vector<time_slot>& visibleSlots();
	boost::posix_time::ptime visibleFrom();
	boost::posix_time::ptime visibleTo();
	time_slot visibleRange();

	size_t visibleFromIndex();
	size_t visibleToIndex();
	const time_slot& slotAt(size_t ix);

	void reset();
private:
	void fillVisibleSlots();

	std::vector<time_slot> _slots;
	std::vector<time_slot> _visibleSlots;
	size_t _from;
	size_t _visible;
};

typedef boost::shared_ptr<TimeSlots> TimeSlotsPtr;

}

#endif /* TIMESLOTS_H_ */
