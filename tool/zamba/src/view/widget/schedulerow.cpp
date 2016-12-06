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
 * schedulerow.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: gonzalo
 */

#include "schedulerow.h"

#include "../view.h"
#include "../datetime.h"
#include "schedule.h"
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/math/special_functions/round.hpp>

namespace wgt
{

ScheduleRow::ScheduleRow(Schedule* schedule, int x, int y, const colorMap_t &colorMap)
	: Window(x,y, schedule->rowWidth(),schedule->rowHeight(), colorMap)
	, _schedule(schedule)
	, _currentCell(-1)
{
}

ScheduleRow::~ScheduleRow()
{}

void ScheduleRow::draw(View* view)
{
	view->draw(this);
}

int ScheduleRow::currentCellId()
{
	return currentCell()._id;
}

int ScheduleRow::firstVisibleCell()
{
	const boost::posix_time::time_period& p = _schedule->currentSlot();

	for (size_t i=0; i<_cells.size(); ++i) {
		Cell& cell = _cells[i];
		boost::posix_time::time_period r = p.intersection(cell._period);
		if (!r.is_null()) {
			return i;
		}
	}

	return (_cells.size())? 0 : -1;
}

const std::vector<ScheduleRow::Cell>& ScheduleRow::cells()
{
	return _cells;
}

const TextAreaPtr ScheduleRow::title()
{
	return _title;
}

void ScheduleRow::activate(bool b, bool fromTop)
{
	Window::activate(b,fromTop);
	_title->activate(b);
	updateFirstCell();
	needsRepaint(true);
}

void ScheduleRow::updateFirstCell()
{
	if (_currentCell >= 0) {
		selectCell(currentCell(), false);
	}
	_currentCell = firstVisibleCell();
	if (_currentCell >= 0) {
		selectCell(currentCell(), true);
	}
}

Color& ScheduleRow::textColor(bool selected)
{
	return (isActive() && selected) ? _colorMap["selectedTextColor"] : _colorMap["textColor"];
}

Color& ScheduleRow::getBgColor(bool selected)
{
	return (isActive() && selected) ? _colorMap["selectedColor"] : _colorMap["bgColor"];
}

void ScheduleRow::titleColors(bool active, const Color& textColor, const Color& bgColor)
{
	if (active) {
		_title->setColor( "selectedColor", bgColor );
		_title->setColor( "selectedTextColor", textColor );
	} else {
		_title->setColor( "bgColor", bgColor );
		_title->setColor( "textColor", textColor );
	}
}

void ScheduleRow::cellColors(bool selected, const Color& textColor, const Color& bgColor)
{
	if (selected) {
		_colorMap["selectedColor"] = bgColor;
		_colorMap["selectedTextColor"] = textColor;
	} else {
		_colorMap["bgColor"] = bgColor;
		_colorMap["textColor"] = textColor;
	}
}

void ScheduleRow::title(const std::string& title)
{
	_title = TextAreaPtr(new TextArea(0,0, _schedule->titleWidth(),4+h(), _colorMap, title, 14, 1, _schedule->rowSep(), alignment::vCenter|alignment::hLeft, 2));
	addChild(_title, false);

	boost::posix_time::time_period p(_schedule->timeSlots().allSlots().front().begin(), _schedule->timeSlots().allSlots().back().last());
	pushCell(p, "", -1);
}

void ScheduleRow::addCell(const boost::posix_time::time_period& period, const std::string& text, int id)
{
	if (!period.is_null() && period.length().total_seconds()/60.0 > 1.0) {
		size_t i = _cells.size()-1;
		for (; i>0 && _cells[i]._period > period; --i) {}

		const int border = 2; // space between cells
		TextAreaPtr ta = TextAreaPtr(new TextArea(0, 0, 0, 2*border+h(), _colorMap, text, 14, 1, 3, alignment::vCenter|alignment::hLeft, border));

		Cell cell(period, ta, id);
		cell._widget->setVisible(false);

		if (_cells[i]._filler) {
			_cells[i] = cell;
		} else {
			_cells.insert(_cells.begin()+i, cell);
		}

		fillGaps(i);
	}
}

void ScheduleRow::fillGaps(int where)
{
	Cell cell = _cells.at(where);
	if (where+1 <  (int) _cells.size()) { // gap to the front
		Cell& nextCell = _cells.at(where+1);
		boost::posix_time::time_period p(cell._period.last() + boost::posix_time::millisec(1), nextCell._period.begin());
		makeFiller(p, where+1);
	} else {
		boost::posix_time::time_period p(cell._period.last() + boost::posix_time::millisec(1), _schedule->timeSlots().allSlots().back().last());
		makeFiller(p, where+1);
	}

	if (where-1 >= 0) { // gap to the back:
		Cell& prevCell = _cells.at(where-1);
		boost::posix_time::time_period p(prevCell._period.last() + boost::posix_time::millisec(1), cell._period.begin() );
		makeFiller(p, where);
	}
	// initial gap
	boost::posix_time::time_period p(_schedule->timeSlots().allSlots().front().begin(), _cells.front()._period.begin());
	makeFiller(p, where);
}

void ScheduleRow::makeFiller(boost::posix_time::time_period& p, int where)
{
	if (!p.is_null() &&  p.length().total_seconds()/60.0 > 1.0 ) {
		const int border = 2; // space between cells
		TextAreaPtr ta = TextAreaPtr(new TextArea(0, 0, 0, 2*border+h(), _colorMap, "", 14, 1, 3, alignment::vCenter|alignment::hLeft, border));
		ta->colorMap( colorMap() );

		Cell cell(p, ta, -1);
		cell._widget->setVisible(false);
		_cells.insert(_cells.begin() + where, cell);
	}
}

void ScheduleRow::pushCell(const boost::posix_time::time_period& period, const std::string& text, int id)
{
	if (!period.is_null() && period.length().total_seconds()/60 > 1) {
		const int border = 2; // space between cells
		TextAreaPtr ta = TextAreaPtr(new TextArea(0, 0, 0, 2*border+h(), _colorMap, text, 14, 1, 3, alignment::vCenter|alignment::hLeft, border));
		ta->colorMap( colorMap() );

		Cell cell(period, ta, id);
		cell._widget->setVisible(false);
		_cells.push_back(cell);

		if (_currentCell == -1) {
			updateFirstCell();
		}
	}
}

void ScheduleRow::moveLabels()
{
	if (_cells.size()) {
		boost::posix_time::time_period p(_schedule->timeSlots().visibleFrom(), _schedule->timeSlots().visibleTo());
		LTRACE( "ScheduleRow", "moveLabels(), slots period: %s", boost::posix_time::to_simple_string(p).c_str() );

		for (size_t i=0; i<_cells.size(); ++i) {
			Cell& cell = _cells[i];
			boost::posix_time::time_period r = p.intersection(cell._period);
			if (!r.is_null() && r.length().total_seconds()/60.0 > 1.0) {
				std::pair<float,float> range = datetime::timeMap(_schedule->timeSlots().visibleSlots(), cell._period.begin(), cell._period.last() + boost::posix_time::seconds(1));
				const int show_x = boost::math::iround(range.first*(float)_schedule->cellWidth());
				const int width  = boost::math::iround((range.second - range.first)*(float)_schedule->cellWidth());
				cell._widget->x(_schedule->titleWidth() + _schedule->rowSep() + show_x);
				cell._widget->w(width);

				cell._widget->setVisible(true);
			} else {
				cell._widget->setVisible(false);
			}
			cell._widget->setColor( "textColor", textColor((int) i==_currentCell) );
			cell._widget->setColor( "bgColor", getBgColor((int) i==_currentCell) );
			//selectCell(cell, i==_currentCell);
		}
	}
}

ScheduleRow::Cell& ScheduleRow::currentCell()
{
	return _cells[_currentCell];
}

bool ScheduleRow::onLeftArrow()
{
	shiftLeft();
	return true;
}

bool ScheduleRow::onRightArrow()
{
	shiftRight();
	return true;
}

void ScheduleRow::arrangeLabels()
{
	moveLabels();
	for (size_t i=0; i<_cells.size(); ++i) {
		_cells[i]._widget->fixOffsets(this);
	}
}

void ScheduleRow::shiftLeft()
{
	if (_cells.size()) {
		const int oldCurrent = _currentCell;
		const int newCell = std::max(0, _currentCell-1);
		bool shifted = false;

		std::vector<boost::posix_time::time_period> slots = _schedule->timeSlots().allSlots();
		boost::posix_time::time_period totalPeriod( slots.front().begin(), slots.back().last() );
		bool visible = totalPeriod.intersection(_cells[newCell]._period).length().total_seconds() >= 120;

		if (visible) {

			selectCell(_cells[oldCurrent], false);
			const int minutes = (currentCell()._period.begin() - _schedule->timeSlots().visibleFrom()).total_seconds()/60;
			LTRACE( "ScheduleRow", "shiftLeft(), minutes=%d", minutes );
			if (minutes < 0) {
				_schedule->shiftLeft();
				shifted = true;
			} else if (minutes > 0) {
				_currentCell = newCell;
			} else {
				_currentCell = newCell;
				_schedule->shiftLeft();
				shifted = true;
			}

			boost::posix_time::time_period visiblePeriod(_schedule->timeSlots().visibleFrom(), _schedule->timeSlots().visibleTo());
			boost::posix_time::time_period insetersecP = visiblePeriod.intersection(currentCell()._period);

			if ( (insetersecP.length().total_seconds() < 120) && !shifted ) {
				_schedule->shiftLeft();
				shifted = true;
			}

			selectCell(currentCell(), true);

			needsRepaint(oldCurrent != _currentCell || shifted);
		}

	}
}

void ScheduleRow::selectCell(Cell& cell, bool b)
{
	cell._widget->setColor( "textColor", textColor(b) );
	cell._widget->setColor( "bgColor", getBgColor(b) );

	if (b && isActive()) {
		_schedule->currentSlot(cell._period);
		_schedule->showSelected(cell._id);
	}
}

void ScheduleRow::shiftRight()
{
	if (_cells.size()) {
		const int oldCurrent = _currentCell;
		bool shifted = false;

		selectCell(currentCell(), false);
		const int m = (_schedule->timeSlots().visibleTo() - currentCell()._period.last()).total_seconds()/60;
		LTRACE( "ScheduleRow", "shiftRight(), minutes=%d", m );
		if (m < 1) {
			shifted = true;
		} else if (m > 1) {
			_currentCell = std::min((int)_cells.size()-1, _currentCell+1);
		} else {
			_currentCell = std::min((int)_cells.size()-1, _currentCell+1);
			shifted = true;
		}

		if (shifted) {
			_schedule->needMoreShows();
			_schedule->shiftRight();
			arrangeLabels();
		}

		selectCell(currentCell(), true);
		needsRepaint(oldCurrent != _currentCell || shifted);
	}
}

void ScheduleRow::synch()
{
	if (_cells.size()) {
		boost::posix_time::time_period p(_schedule->timeSlots().visibleFrom(), _schedule->timeSlots().visibleTo());
		if (!p.intersects(currentCell()._period)) {
			selectCell(currentCell(), false);

			for (size_t i=0; i<_cells.size(); ++i) {
				if (p.intersects(_cells[i]._period)) {
					_currentCell = i;
					break;
				}
			}

			selectCell(currentCell(), true);
		}
		arrangeLabels();
		needsRepaint(true);
	}
}

void ScheduleRow::dumpShows()
{
	for (size_t i=0; i<_cells.size(); ++i) {
		Cell& cell = _cells[i];
		const char *current = ( i == (unsigned int)_currentCell ) ? "*" : " ";
		LTRACE( "ScheduleRow", "%s show: %s, period: %s", current, ((cell._filler) ? "<filler>" : cell._widget->text().c_str()), boost::posix_time::to_simple_string(cell._period).c_str() );
	}
}

}
