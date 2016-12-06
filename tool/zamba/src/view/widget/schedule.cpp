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
 * schedule.cpp
 *
 *  Created on: Jul 13, 2011
 *      Author: gonzalo
 */

#include "schedule.h"
#include "../view.h"
#include "../modules/wgt.h"
#include <util/mcr.h>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/date_facet.hpp>

namespace wgt {

Schedule::Schedule(int x, int y, int w, int h, const colorMap_t &colorMap)
	: Window(x,y, w,h, colorMap)
	, _cell_width(90)
	, _title_width(100)
	, _row_sep(4)
	, _row_height(20)
	, _visibleCount(6)
	, _rowCount(0)
	, _visibleSlots(3)
	, _currentSlot(boost::posix_time::time_period(boost::posix_time::ptime(), boost::posix_time::ptime()))
{
	_timeSlots.visibleSlots(_visibleSlots);
	_scrollbar = new Scrollbar();
	_scrollbar->scrollColor(getColor("scrollColor"));
	_scrollbar->bgColor(getColor("scrollBgColor"));
}

Schedule::~Schedule()
{
	DEL(_scrollbar);
}

void Schedule::widths( int title, int slot ) {
	_title_width = title;
	_cell_width  = slot;
}

void Schedule::sep( int sep ) {
	_row_sep = sep;
}

int Schedule::cellWidth() {
	return _cell_width;
}

int Schedule::titleWidth() {
	return _title_width;
}

int Schedule::rowSep() {
	return _row_sep;
}

int Schedule::rowHeight() {
	return _row_height;
}

int Schedule::rowWidth() {
	return _title_width + _row_sep + _visibleSlots*(_cell_width + _row_sep);
}

void Schedule::rowHeight( int n ) {
	_row_height = n;
}

void Schedule::showCount( int n ) {
	_visibleCount = n;
}

int Schedule::visibleCount() {
	return _visibleCount;
}

ScheduleRowPtr Schedule::getRow( size_t ix ) {
	return _rows.at(ix);
}

const ScheduleRowPtr &Schedule::header() {
	return _header;
}

const std::vector<ScheduleRowPtr> &Schedule::rows() {
	return _rows;
}

size_t Schedule::rowCount() {
	return _rows.size();
}

void Schedule::draw( View* view ) {
	view->draw(this);
	view->draw(_scrollbar);
}

void Schedule::addRow( const std::string &title, bool handleKey ) {
	ScheduleRowPtr row = boost::make_shared<ScheduleRow>(this, 0,0, _colorMap);
	row->title(title);
	row->titleColors( false, _colorMap["textTitleColor"], _colorMap["titleColor"] );
	row->titleColors( true, _colorMap["selectedTextTitleColor"], _colorMap["selectedTitleColor"] );
	row->cellColors( false, _colorMap["textCellColor"], _colorMap["cellColor"] );
	row->cellColors( true, _colorMap["selectedTextCellColor"], _colorMap["selectedCellColor"] );
	_rows.push_back(row);
	addChild(row, handleKey);
}

void Schedule::rows( const std::vector<std::string> &titles, int visible ) {
	_visibleCount = visible;
	BOOST_FOREACH(std::string title, titles) {
		addRow(title, true);
	}
	// Fill table with empty rows
	for (int i=_rows.size(); i<_visibleCount; ++i) {
		addRow("", false);
	}
	_rowCount = _rows.size();

	_scrollbar->initFromTo(_activeChild, _visibleCount, _rowCount);
	_scrollbar->init( x()+w()-24, y()+2+_row_height+_row_sep, _visibleCount, _rowCount, _row_height, _row_sep );
	_scrollbar->updateFromTo( _activeChild, _visibleCount, _rowCount );
	_scrollbar->update( _activeChild, _visibleCount, _rowCount, 2 );

	int r_y = rowSep() + rowHeight();
	for (unsigned int i=_scrollbar->from(); i <= _scrollbar->to(); ++i) {
		ScheduleRowPtr row = getRow(i);
		row->y(r_y);
		row->arrangeLabels();
		r_y += rowSep() + rowHeight();
	}

}

void Schedule::init( const std::vector<TimeSlots::time_slot> &slots, const std::vector<std::string> &headers ) {
	_timeSlots.setSlots(slots);

	removeChildren();

	_rows.clear();

	_header.reset();
	_header = boost::make_shared<ScheduleRow>(this, 0,0, _colorMap);
	_header->title("");
	_header->cellColors( true, _colorMap["textHeaderColor"], _colorMap["headerColor"] );
	_header->cellColors( false, _colorMap["textHeaderColor"], _colorMap["headerColor"] );
	_header->titleColors( true, _colorMap["textHeaderColor"], _colorMap["headerTitleColor"] );
	_header->titleColors( false, _colorMap["textHeaderColor"], _colorMap["headerTitleColor"] );
	_header->title()->alignment(alignment::hRight | alignment::vCenter);
	for (size_t i=0; i<headers.size(); ++i) {
		_header->addCell(slots[i], headers[i], -1);
	}
	_header->activate(false);
	_header->moveLabels();

	addChild(_header, false);
	updateDate();
}

void Schedule::init( const boost::posix_time::ptime &currentTime, int periodLength, int slotCount ) {
	std::vector<TimeSlots::time_slot> slots;
	std::vector<std::string> headers;
	int m = (currentTime.time_of_day().minutes()/periodLength) * periodLength;
	boost::posix_time::ptime initialTime(currentTime.date(), boost::posix_time::time_duration(currentTime.time_of_day().hours(), m, 0,0));
	boost::posix_time::time_duration halfhour = boost::posix_time::minutes(periodLength);

	for(int i=0;i<slotCount;++i) {
		std::ostringstream os;
		os << std::setw(2) << std::setfill('0') << initialTime.time_of_day().hours()
		   << ":"
		   << std::setw(2) << std::setfill('0') << initialTime.time_of_day().minutes();

		headers.push_back(os.str());

		boost::posix_time::time_period range(initialTime, halfhour + boost::posix_time::seconds(1));
		slots.push_back(range);
		initialTime += halfhour;
	}

	init(slots, headers);
}

TimeSlots &Schedule::timeSlots() {
	return _timeSlots;
}

void Schedule::updateDate() {
	boost::gregorian::date::ymd_type first(_timeSlots.visibleFrom().date().year_month_day());
	std::stringstream str;
	str << first.day << "/" << int(first.month) << "/" << first.year;
	_header->title()->text(str.str());
}

void Schedule::shiftLeft() {
	if (_timeSlots.shiftLeft()) {
		_header->synch();
		updateDate();
		BOOST_FOREACH(ScheduleRowPtr row, _rows) {
			row->arrangeLabels();
		}
		needsRepaint(true);
	}
}

void Schedule::shiftRight() {
	if (_timeSlots.shiftRight()) {
		_header->synch();
		updateDate();
		BOOST_FOREACH(ScheduleRowPtr row, _rows) {
			row->arrangeLabels();
		}
		needsRepaint(true);
	}
}

void Schedule::moveSlotsTo( boost::posix_time::ptime time ) {
	_timeSlots.moveSlotsTo(time);
	_header->synch();
	BOOST_FOREACH(ScheduleRowPtr row, _rows) {
		row->synch();
	}
}

void Schedule::setShowSelected( const std::string &showCallback ) {
	_lshowselected = showCallback;
}

void Schedule::setChannelSelected( const std::string &channelCallback ) {
	_lchannelselected = channelCallback;
}

void Schedule::setMoreShows( const std::string &moreShowsCallback ) {
	_lmoreshows = moreShowsCallback;
}

void Schedule::showSelected( int n ) {
	if (!_lshowselected.empty()) {
		Wgt::call(_lshowselected, n);
	}
}

void Schedule::channelSelected( int n ) {
	if (!_lchannelselected.empty()) {
		Wgt::call(_lchannelselected, n);
	}
}

void Schedule::needMoreShows() {
	if (!_lmoreshows.empty()) {
		Wgt::call(_lmoreshows, _activeChild);
	}
}

void Schedule::setSelected( int row ) {
	const size_t oldChild = _activeChild;

	selectChild(row);
	if (oldChild != _activeChild) {
		selectedRowChanged(_scrollbar->from());
	}
}

void Schedule::selectedRowChanged( unsigned int oldFrom ) {
	_scrollbar->updateFromTo( _activeChild, _visibleCount, _rowCount );
	_scrollbar->update( _activeChild, _visibleCount, _rowCount, 2 );

	if (_scrollbar->from() != oldFrom) {
		needMoreShows();
	}

	channelSelected(_activeChild);
	showSelected(_rows[_activeChild]->currentCellId());

	int r_y = y() + rowSep() + rowHeight();
	for (unsigned int i=_scrollbar->from(); i <= _scrollbar->to(); ++i) {
		ScheduleRowPtr row = getRow(i);
		row->y(r_y);
		row->arrangeLabels();

		r_y += rowSep() + rowHeight();
	}

	needsRepaint(true);
}

bool Schedule::onUpArrow() {
	WidgetPtr oldChild = activeChild();

	selectPreviousChild(false);
	WidgetPtr newChild = activeChild();
	if (oldChild != newChild) {
		oldChild->activate(false);
		newChild->activate(true, false);
		selectedRowChanged(_scrollbar->from());
	}

	return true;
}

bool Schedule::onDownArrow() {
	WidgetPtr oldChild = activeChild();

	selectNextChild(false);
	WidgetPtr newChild = activeChild();
	if (oldChild != newChild) {
		oldChild->activate(false);
		newChild->activate(true, true);
		selectedRowChanged(_scrollbar->from());
	}

	return true;
}

void Schedule::dumpShows() {
	BOOST_FOREACH(ScheduleRowPtr row, _rows) {
		if (!row->title()->text().empty()) {
			row->dumpShows();
		}
	}
}

const TimeSlots::time_slot &Schedule::currentSlot() {
	return _currentSlot;
}

void Schedule::currentSlot( const TimeSlots::time_slot &slot ) {
	_currentSlot = _timeSlots.visibleRange().intersection(slot);
}

void Schedule::resetSlots() {
	_timeSlots.reset();
	_currentSlot = _timeSlots.visibleRange();
	updateDate();
}

Scrollbar *Schedule::scrollbar() {
	return _scrollbar;
}

} //wgt
