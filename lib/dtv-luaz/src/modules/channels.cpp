/******************************************************************************

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

#include "channel.h"
#include "../lua.h"
#include <zapper/zapper.h>
#include <pvr/parental/control.h>
#include <pvr/show.h>
#include <pvr/channels.h>
#include <pvr/channel.h>
#include <pvr/time.h>
#include <pvr/pvr.h>
#include <util/log.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/foreach.hpp>

namespace luaz {
namespace channels {

namespace impl {
	static pvr::Channels *channels = NULL;
}

static inline std::string formatTime( const boost::posix_time::ptime& time, const std::string& fmt ) {
	if (time.is_not_a_date_time()) {
		return "";
	} else {
		boost::posix_time::time_facet *outputFacet = new boost::posix_time::time_facet();
		outputFacet->format(fmt.c_str());
		std::stringstream ss;
		ss.imbue(std::locale(std::locale::classic(), outputFacet));
		ss << time;
		return ss.str();
	}
}

static inline pvr::Channel *get( lua_State *L ) {
	int chID = luaL_checkint(L, 1);
	DTV_ASSERT(chID >= 0);
	pvr::Channel *ch=impl::channels->get( chID );
	DTV_ASSERT(ch);
	//	ch->show();
	return ch;
}

static void pushProgramGuide( lua_State *L, pvr::Show *program ) {
	lua_newtable(L);
	lua::setField( L, "name",            program->name() );
	lua::setField( L, "description",     program->description() );
	lua::setField( L, "parentalAge",     program->parentalControl()->getAgeName() );
	lua::setField( L, "parentalContent", program->parentalControl()->getContentName() );

	lua::setField( L, "length", program->timeRange().length().minutes() );

	lua_pushstring(L, "startDate" );
	lua_newtable(L);
	lua::setField( L, "time", formatTime(program->timeRange().begin(), "%H:%M"));
	lua::setField( L, "day",  formatTime(program->timeRange().begin(), "%Y-%m-%d"));
	lua_settable(L, -3);

	lua_pushstring(L, "stopDate" );
	lua_newtable(L);
	lua::setField( L, "time", formatTime(program->timeRange().end(), "%H:%M"));
	lua::setField( L, "day",  formatTime(program->timeRange().end(), "%Y-%m-%d"));
	lua_settable(L, -3);
}

static void pushShowInfo( lua_State *L, pvr::Channel *ch, pvr::Show *program ) {
	lua::setField( L, "content",         ch->category() );
	lua::setField( L, "parentalAge",     ch->parentalControl()->getAgeName() );
	lua::setField( L, "parentalContent", ch->parentalControl()->getContentName() );
	lua::setField( L, "startTime",       formatTime(program->timeRange().begin(), "%H:%M"));
	lua::setField( L, "endTime",         formatTime(program->timeRange().end(), "%H:%M"));
	lua::setField( L, "percentage",      program->percentage() );
	lua::setField( L, "showName",        program->name() );
	lua::setField( L, "showDescription", program->description() );
}

static void pushChannel( lua_State *L, pvr::Channel *ch ) {
	lua_newtable(L);
	lua::setField( L, "channelID", ch->channelID() );
	lua::setField( L, "channel",   ch->channel() );
	lua::setField( L, "name",      ch->name() );
	lua::setField( L, "logo",      ch->logo() );
	lua::setField( L, "oneSeg",    ch->isMobile() ? 1 : 0 );
	lua::setField( L, "blocked",   ch->isBlocked() ? 1 : 0 );
	lua::setField( L, "favorite",  ch->isFavorite() ? 1 : 0 );
}

static void onNewChannel( lua_State *L, pvr::Channel *ch ) {
	lua_getfield( L, LUA_GLOBALSINDEX, "zapperNewChannelFound" );
	pushChannel( L, ch );
	lua_call( L, 1, 0 );
}

static int l_channelCount( lua_State *L ) {
	const int n = (int)impl::channels->getAll().size();
	lua_pushnumber( L, n);
	return 1;
}

static int l_getAll( lua_State *L ) {
	const pvr::ChannelList &chs = impl::channels->getAll();
	lua_newtable(L);
	for (size_t ch=0; ch<chs.size(); ch++) {
		//	Key
		lua_pushnumber( L, ch +1);

		//	Value
		pushChannel( L, chs[ch] );

		//	Set to Services
		lua_settable( L, -3 );
	}
	return 1;
}

static int l_get( lua_State *L ) {
	pushChannel( L, get(L) );
	return 1;
}

static int l_getInfo( lua_State *L ) {
	//	Get channel
	pvr::Channel *ch=get(L);

	//	Get EPG program
	pvr::Show *prg=ch->getCurrentShow();
	DTV_ASSERT(prg);
	//prg->show();

	pushChannel( L, ch );
	pushShowInfo( L, ch, prg );
	return 1;
}

static int l_getShowsBetween( lua_State *L ) {
	pvr::Channel::Shows progs;
	pvr::Channel *ch;

	{	//	Get parameters
		int narg = 1;
		const int chID = luaL_checkint(L, narg++);
		const char *time1 = luaL_checkstring(L, narg++);
		const char *time2 = luaL_checkstring(L, narg++);

		//	Get channel
		ch=impl::channels->get( chID );
		DTV_ASSERT(ch);

		//	Get shows
		using boost::posix_time::ptime;
		using boost::posix_time::time_period;
		using boost::posix_time::time_from_string;
		time_period timeRange(time_from_string(time1), time_from_string(time2));
		ch->getShowsBetween( progs, timeRange );
	}

	int index=1;
	lua_newtable(L);
	BOOST_FOREACH( pvr::Show *program, progs ) {
		lua_pushnumber( L, index++ ); // Key
		pushProgramGuide( L, program );
		lua_settable( L, -3 ); // Set table
	}
	return 1;
}

static int l_remove( lua_State *L ) {
	impl::channels->remove( luaL_checkint(L, 1) );
	return 0;
}

static int l_isProtected( lua_State *L ) {
	pvr::Channel *ch=get(L);
	lua_pushboolean( L, ch->isProtected() ? 1 : 0 );
	return 1;
}

static int l_isBlocked( lua_State *L ) {
	pvr::Channel *ch=get(L);
	lua_pushnumber( L, ch->isBlocked() ? 1 : 0 );
	return 1;
}

static int l_toggleBlocked( lua_State *L ) {
	pvr::Channel *ch=get(L);
	ch->toggleBlocked();
	return 0;
}

static int l_toggleFavorite( lua_State *L ) {
	pvr::Channel *ch=get(L);
	ch->toggleFavorite();
	return 0;
}

static const struct luaL_Reg channels_methods[] = {
	{ "count",           l_channelCount    },
	{ "get",             l_get             },
	{ "getShowsBetween", l_getShowsBetween },
	{ "getInfo",         l_getInfo         },
	{ "getAll",          l_getAll          },
	{ "toggleFavorite",  l_toggleFavorite  },
	{ "remove",          l_remove          },
	{ "isBlocked",       l_isBlocked       },
	{ "toggleBlocked",   l_toggleBlocked   },
	{ "isProtected",     l_isProtected     },
	{ NULL,              NULL              }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::channels = zapper->pvr()->channels();
}

void finalize( void ) {
	impl::channels = NULL;
}

void start( lua_State *st ) {
	{	//	Connect to new channel signal
		boost::function<void (pvr::Channel *)> fnc = boost::bind(&onNewChannel,st,_1);
		impl::channels->onNewChannel().connect( fnc );
	}

	luaL_register( st, "channels", channels_methods );
}

void stop( void ) {
}

}
}
