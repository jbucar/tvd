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
 * schedule.h
 *
 *  Created on: Jul 13, 2011
 *      Author: gonzalo
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "window.h"
#include "schedulerow.h"
#include "timeslots.h"
#include "scrollbar.h"

namespace wgt {

class Schedule : public wgt::Window {
public:
	Schedule(int x, int y, int w, int h, const colorMap_t &colorMap);
	virtual ~Schedule();

	void draw(View* view);

	ScheduleRowPtr getRow(size_t ix);
	size_t rowCount();
	const ScheduleRowPtr& header();
	const std::vector<ScheduleRowPtr>& rows();

	void rows(const std::vector<std::string>& titles, int visible);
	void init(const std::vector<TimeSlots::time_slot>& slots, const std::vector<std::string>& headers);
	void init(const boost::posix_time::ptime& currentTime, int periodLength, int slotCount);
	void setSelected(int row);

	TimeSlots& timeSlots();
	const TimeSlots::time_slot& currentSlot();
	void currentSlot(const TimeSlots::time_slot& slot);

	void widths(int title, int slot);
	void sep(int sep);
	void rowHeight(int n);
	void showCount(int n);

	int cellWidth();
	int titleWidth();
	int rowSep();
	int rowHeight();
	int rowWidth();
	int visibleCount();

	void shiftLeft();
	void shiftRight();

	void moveSlotsTo(boost::posix_time::ptime time);
	void resetSlots();

	void setShowSelected(   const std::string& showCallback);
	void setChannelSelected(const std::string& channelCallback);
	void setMoreShows(      const std::string& moreShowsCallback);

	void showSelected(int n);
	void channelSelected(int n);
	void needMoreShows();

	void dumpShows();

	Scrollbar *scrollbar();

protected:
	virtual bool onUpArrow();
	virtual bool onDownArrow();

	// Helpers
	void addRow( const std::string &title, bool handleKey );

private:
	ScheduleRowPtr _header;
	std::vector<ScheduleRowPtr> _rows;

	TimeSlots _timeSlots;

	int _cell_width;
	int _title_width;
	int _row_sep;
	int _row_height;

	int _visibleCount;
	int _rowCount;
	int _visibleSlots;
	TimeSlots::time_slot _currentSlot;
	Scrollbar *_scrollbar;

	std::string _lshowselected;
	std::string _lchannelselected;
	std::string _lmoreshows;

	void updateDate();
	void selectedRowChanged(unsigned int oldFrom);
};

DEF_SHARED(Schedule);

}

#endif /* SCHEDULE_H_ */
