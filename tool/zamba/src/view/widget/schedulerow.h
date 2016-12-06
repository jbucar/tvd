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
 * schedulerow.h
 *
 *  Created on: Jul 11, 2011
 *      Author: gonzalo
 */

#ifndef SCHEDULEROW_H_
#define SCHEDULEROW_H_

#include "window.h"

#include "label.h"
#include "textarea.h"
#include "timeslots.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace wgt
{

class View;
class Schedule;

class ScheduleRow : public wgt::Window
{
public:
	ScheduleRow(Schedule* schedule, int x, int y, const colorMap_t &colorMap);
	virtual ~ScheduleRow();

	void draw(View* view);

	virtual void activate(bool b, bool fromTop=true);
	void widths(int title, int slot);
	void sep(int sep);
	void title(const std::string& title);
	void addCell(const boost::posix_time::time_period& period, const std::string& text, int id);

	void titleColors(bool active, const Color& textColor, const Color& bgColor);
	void cellColors(bool selected, const Color& textColor, const Color& bgColor);

	void moveLabels();
	void arrangeLabels();

	void shiftLeft();
	void shiftRight();
	void synch();

	int currentCellId();

	struct Cell {
		boost::posix_time::time_period _period;
		TextAreaPtr _widget;
		bool _filler;
		int _id;

		Cell(const boost::posix_time::time_period& period, TextAreaPtr widget, int id)
			: _period(period), _widget(widget), _filler(id==-1), _id(id)
		{};
	};
	const std::vector<Cell>& cells();
	const TextAreaPtr title();

	void dumpShows();

protected:
	virtual bool onLeftArrow();
	virtual bool onRightArrow();

	Cell& currentCell();

	Color& textColor(bool selected);
	Color& getBgColor(bool selected);

	int firstVisibleCell();
	void pushCell(const boost::posix_time::time_period& period, const std::string& text, int id);
	void fillGaps(int where);
	void updateFirstCell();
	void makeFiller(boost::posix_time::time_period& p, int where);
	void selectCell(Cell& cell, bool b);

private:
	TextAreaPtr _title;
	std::vector<Cell> _cells;
	Schedule* _schedule;

	int _currentCell;
};

DEF_SHARED(ScheduleRow);

}

#endif /* SCHEDULEROW_H_ */
