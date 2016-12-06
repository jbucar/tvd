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
 * datetime.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: gonzalo
 */

#include "datetime.h"

namespace wgt {

namespace datetime {

std::pair<float,float> timeMap(const std::vector<time_period>& slots, std::pair<ptime,ptime> program)
{
	return timeMap(slots, program.first, program.second);
}

std::pair<float,float> timeMap(const std::vector<time_period>& slots, ptime p_begin, ptime p_end)
{
	float begin = -1;
	float end   = -1;
	int slot  = 0;

	// if slots range is included in the show's range
	if (p_begin <= slots.front().begin() && slots.back().last() <= p_end) {
		begin = 0;
		end   = (float) slots.size();
	} else {
		for (std::vector<time_period>::const_iterator s_it = slots.begin(); s_it != slots.end(); ++s_it) {
			ptime s_begin = (*s_it).begin();
			ptime s_end   = (*s_it).last();
			time_duration slot_duration  = s_end - s_begin;
			float slot_duration_in_minutes = (float)(slot_duration.total_seconds()/60);

			//begin
			if (s_begin <= p_begin && p_begin <= s_end && begin == -1) {
				time_duration program_start  = p_begin - s_begin;
				float program_start_in_minutes = (float)(program_start.total_seconds()/60);

				begin = (float)slot + (program_start_in_minutes / slot_duration_in_minutes);
			}
			//end
			if (s_begin <= p_end && p_end <= s_end && end == -1) {
				time_duration program_end  = p_end - s_begin;
				float program_end_in_minutes = (float)(program_end.total_seconds()/60);

				end = (float)slot + (program_end_in_minutes / slot_duration_in_minutes);
			}
			slot++;
		}
		if (begin != -1.0f || end != -1.0f) {
			if (begin == -1) {
				begin = 0.0f;
			}
			if (end == -1) {
				end = (float)slot;
			}
		}
	}
	return std::make_pair(begin, end);
}


} // datetime
} // wgt
