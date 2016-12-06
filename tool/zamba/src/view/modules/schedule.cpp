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
 *  Created on: Jul 11, 2011
 *      Author: gonzalo
 */

#include "luawgt.h"
#include "wgt.h"
#include "../widget/schedulerow.h"
#include "../widget/schedule.h"
#include "canvas.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/make_shared.hpp>

namespace wgt {
namespace schedule {

std::map<WidgetId, SchedulePtr> _widgets;

int create(lua_State* L)
{
	int narg = 2;
	const int x = luaL_checkint(L, narg++);
	const int y = luaL_checkint(L, narg++);
	const int w = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);
	const colorMap_t colorMap = parseColorTable( L, narg++ );

	SchedulePtr ww = boost::make_shared<Schedule>(x,y, w,h, colorMap);

	WidgetId id = Wgt::view()->addWidget(ww);
	_widgets[id] = ww;

	lua_pushnumber(L, id);
	return 1;
}

int setRows(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	std::vector<std::string> titles;
	luaz::lua::readList(L, narg++, titles);
	const int visible = luaL_checkint(L, narg++);

	SchedulePtr schedule = _widgets[id];
	schedule->rows(titles, visible);

	return 0;
}

int setTitle(lua_State* L)
{
	int narg = 2;
	const WidgetId id       = luaL_checkint(L, narg++);
	const int ix            = luaL_checkint(L, narg++);
	const std::string title = luaL_checkstring(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->getRow(ix)->title(title);

	return 0;
}

int addCell(lua_State* L)
{
	using boost::posix_time::ptime;
	using boost::posix_time::time_from_string;
	using boost::posix_time::time_duration;
	using boost::posix_time::minutes;

	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int ix     = luaL_checkint(L, narg++);
	std::string ts1  = luaL_checkstring(L, narg++);
	std::string ts2  = luaL_checkstring(L, narg++);
	std::string text = luaL_checkstring(L, narg++);
	const int cellid = luaL_checkint(L, narg++);

	ptime start(time_from_string(ts1));
	ptime end(time_from_string(ts2));
	boost::posix_time::time_period value(start, end);

	SchedulePtr widget = _widgets[id];
	widget->getRow(ix)->addCell(value, text, cellid);

	return 0;
}

using boost::posix_time::ptime;
void str_list_to_time_period_list(std::vector<std::string>& source, std::vector<boost::posix_time::time_period>& dest, std::vector<std::string>& headers)
{
	using boost::posix_time::time_from_string;
	for (std::vector<std::string>::iterator it = source.begin(); it != source.end(); ++it) {
		ptime t1 = time_from_string((*it++));
		ptime t2 = time_from_string((*it++));
		std::string header = *it;
		//std::pair<ptime,ptime> value = std::pair<ptime,ptime>(t1,t2);
		boost::posix_time::time_period value(t1,t2);
		dest.push_back(value);
		headers.push_back(header);
	}
}

int setWidths(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int titlew  = luaL_checkint(L, narg++);
	const int cellw   = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->widths(titlew, cellw);

	return 0;
}

int setRowSep(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int sep = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->sep(sep);

	return 0;
}

int setRowHeight(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int h = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->rowHeight(h);

	return 0;
}

int setActive(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int ix      = luaL_checkint(L, narg++);
	const int active  = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->getRow(ix)->activate(active!=0);

	return 0;
}

int moveSlotsTo(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const std::string date_str(luaL_checkstring(L, narg++));
	boost::posix_time::ptime date = boost::posix_time::time_from_string(date_str);

	SchedulePtr widget = _widgets[id];
	widget->moveSlotsTo(date);

	return 0;
}

int setCurrent(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const int ix      = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->setSelected(ix);

	return 0;
}

int setShowSelectedCallback(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const std::string cbk(luaL_checkstring(L, narg++));

	SchedulePtr widget = _widgets[id];
	widget->setShowSelected(cbk);

	return 0;
}

int setChannelSelectedCallback(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const std::string cbk(luaL_checkstring(L, narg++));

	SchedulePtr widget = _widgets[id];
	widget->setChannelSelected(cbk);

	return 0;
}

int setMoreShowsCallback(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const std::string cbk(luaL_checkstring(L, narg++));

	SchedulePtr widget = _widgets[id];
	widget->setMoreShows(cbk);

	return 0;
}

int getVisibleRowRange(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];

	lua_pushinteger(L, widget->scrollbar()->from());
	lua_pushinteger(L, widget->scrollbar()->to());

	return 2;
}

int getVisibleTimeRange(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];

	std::string from(boost::posix_time::to_simple_string(widget->timeSlots().visibleFrom()));
	std::string   to(boost::posix_time::to_simple_string(widget->timeSlots().visibleTo()));

	lua_pushstring(L, from.c_str());
	lua_pushstring(L, to.c_str());

	return 2;
}

int getNextVisibleTimeRange(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	int c = 1;
	if (lua_gettop(L) == 3) {
		c = luaL_checkint(L, narg++);
	}

	SchedulePtr widget = _widgets[id];

	const int fromIx = widget->timeSlots().visibleToIndex();
	const int   toIx = widget->timeSlots().visibleToIndex()+c*widget->timeSlots().visible();
	std::string from(boost::posix_time::to_simple_string(widget->timeSlots().slotAt(fromIx).begin()));
	std::string   to(boost::posix_time::to_simple_string(widget->timeSlots().slotAt(toIx).last()));

	lua_pushstring(L, from.c_str());
	lua_pushstring(L, to.c_str());

	return 2;
}

int initSlots(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const std::string time_str(luaL_checkstring(L, narg++));
	const int length = luaL_checkint(L, narg++);
	const int count  = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->init(boost::posix_time::time_from_string(time_str), length, count);

	return 0;
}

int resetSlots(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->resetSlots();

	return 0;
}

int dumpShows(lua_State* L)
{
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);

	SchedulePtr widget = _widgets[id];
	widget->dumpShows();

	return 0;
}

int l_scrollImages( lua_State* L ) {
	int narg = 2;
	const WidgetId id = luaL_checkint(L, narg++);
	const char *up = luaL_checkstring(L, narg++);
	const char *down = luaL_checkstring(L, narg++);

	Scrollbar *scroll = _widgets[id]->scrollbar();
	scroll->arrowUp(up);
	scroll->arrowDown(down);

	return 0;
}

void init(lua_State *L) {
	const struct luaL_Reg methods[] = {
			{"new",      create},
			{"setRows",  setRows},
			{"title",    setTitle},
			{"addCell",  addCell},
			{"setWidths",      setWidths},
			{"setRowSep",      setRowSep},
			{"setRowHeight",   setRowHeight},
			{"setActive",      setActive},
			{"moveSlotsTo",    moveSlotsTo},
			{"setCurrent",     setCurrent},

			{"setShowSelectedCallback",    setShowSelectedCallback},
			{"setChannelSelectedCallback", setChannelSelectedCallback},
			{"setMoreShowsCallback",       setMoreShowsCallback},

			{"getVisibleRowRange",   getVisibleRowRange},
			{"getVisibleTimeRange",  getVisibleTimeRange},
			{"getNextVisibleTimeRange",  getNextVisibleTimeRange},
			{"initSlots",  initSlots},
			{"resetSlots", resetSlots},

			{"dumpShows", dumpShows},
			{"scrollImages",    l_scrollImages},
			{NULL,  NULL}
	};

	luaL_register(L, "schedule", methods);
}

} // schedule

} // wgt
