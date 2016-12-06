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

#include "channel.h"
#include "show.h"
#include "tuner.h"
#include "time.h"
#include "channels.h"
#include "parental/control.h"
#include "parental/session.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

#define PRINT_PERIOD(p)	\
	time::asString(p.begin()).c_str(), time::asString(p.end()).c_str()

#define CHECK_SESSION	  \
	if (isOnline()) { \
		parental::Session *session = _tuner->session(); \
		DTV_ASSERT(session); \
		DTV_ASSERT(!session->isBlocked()); \
	}

#define THRESHOLD 300 // 5 minutes

namespace pvr {

namespace impl {
	static Show defaultShow;

	struct OverlapsWith {
		OverlapsWith( const bpt::time_period &period ) : _period(period) {
		}

		bool operator()( const Show *s ) const {
			bpt::time_period intersection = s->timeRange().intersection(_period);
			bool res = s->timeRange().intersects(_period);
			if (res && (intersection.length().total_seconds() < THRESHOLD)) {
				LWARN( "Channel", "addShow: shows overlaps for less than %d seconds, this may be an error.", THRESHOLD);
			}
			return res;
		}

		const bpt::time_period &_period;
	private:
		OverlapsWith &operator=( const OverlapsWith & /*other*/ ) {
			return *this;
		}
	};

}

Channel::Channel( const std::string &name, const std::string &channel, const std::string &network )
	: _channelID(-1), _name(name), _channel(channel), _network(network)
{
	_isFavorite = false;
	_isBlocked = false;
	_isMobile = false;
	_isFree = true;
	_parental = NULL;

	_tuner = NULL;
}

Channel::Channel()
	: _channelID(-1), _name("Invalid"), _channel("99.99"), _network("Invalid")
{
	_isFavorite = false;
	_isBlocked = false;
	_isMobile = false;
	_isFree = true;
	_parental = NULL;

	_tuner = NULL;
}

Channel::~Channel()
{
	delete _parental;
	CLEAN_ALL( Show *, _shows );
}

//	Channel ID
int Channel::channelID() const {
	return _channelID;
}

void Channel::channelID( int channelID ) {
	_channelID = channelID;
}

//	Channel information
const std::string &Channel::name() const {
	return _name;
}

const std::string &Channel::channel() const {
	return _channel;
}

const std::string &Channel::network() const {
	return _network;
}

const std::string &Channel::logo() {
	if (isOnline()) {
		_logo = tuner()->findLogo( this );
	}
	return _logo;
}

bool Channel::isMobile() const {
	return _isMobile;
}

void Channel::mobile( bool isMobile ) {
	if (_isMobile != isMobile) {
		_isMobile = isMobile;
		notify(true);
	}
}

void Channel::isFree( bool isFree ) {
	if (_isFree != isFree) {
		_isFree = isFree;
		notify(true);
	}
}

bool Channel::isFree() const {
	return _isFree;
}

bool Channel::isProtected() const {
	return isProtected(parentalControl());
}

bool Channel::isProtected( parental::Control *control ) const {
	if (isOnline()) {
		parental::Session *session = _tuner->session();
		DTV_ASSERT(session);
		if (!session->isBlocked()) {
			return false;
		}
		else {
			return isBlocked() || (session->isAllowed(control) ? false : true);
		}
	}
	else {
		return false;
	}
}

category::Type Channel::category() const {
	Show *s = getCurrentShow();
	return s->category();
}

//	Shows
void Channel::addShow( Show *s ) {
	DTV_ASSERT(s);

	// Remove shows that overlaps with the new show
	_shows.erase(std::remove_if(_shows.begin(), _shows.end(), impl::OverlapsWith(s->timeRange())), _shows.end());

	_shows.push_back( s );
	if (s == getCurrentShow(false)) {
		notify();
	}
}

Show *Channel::getCurrentShow() const {
	return getCurrentShow(true);
}

Show *Channel::getCurrentShow( bool excludeProtected ) const {
	bpt::ptime now;
	time::clock()->get( now );
	return getShow(now, excludeProtected);
}

Show *Channel::getShow( const bpt::ptime &t ) const {
	return getShow(t, true);
}

Show *Channel::getShow( const bpt::ptime &t, bool excludeProtected ) const {
	LTRACE( "Channel", "Get current show" );
	const bpt::time_period period( t, bpt::seconds(1) );
	BOOST_FOREACH( Show *show, _shows ) {
		bool cond = !excludeProtected || (excludeProtected && !isProtected(show->parentalControl()));
		if (show->timeRange().intersects(period) && cond) {
			LTRACE( "Channel", "Get show: period(%s,%s) show(%s,%s)",
				PRINT_PERIOD(period), PRINT_PERIOD(show->timeRange()) );
			return show;
		}
	}
	return isOnline() ? _tuner->defaultShow() : &impl::defaultShow;
}

void Channel::getShowsBetween( Shows &shows, const bpt::time_period &period ) const {
	DTV_ASSERT(shows.empty());

	//	Filter shows
	BOOST_FOREACH( Show *show, _shows ) {
		if (!isProtected(show->parentalControl()) && show->timeRange().intersects(period)) {
			shows.push_back( show );
		}
	}

	LTRACE( "Channel", "Get shows: period(%s,%s), shows=%d",
		PRINT_PERIOD(period), shows.size() );
}

//	Parental
parental::Control *Channel::parentalControl() const {
	// ABNT NBR 15608-3:2008 12.4.4:
	//	Parental rating information can also be inserted in the PMT, in the event the broadcaster does not wish to configure
	//	such information by event. In this case, the priority of the receiver should be attributed as follows: PMT > EIT.
	if (_parental) {
		return _parental;
	}
	else {
		Show *s = getCurrentShow(false);
		return s->parentalControl();
	}
}

void Channel::parentalControl( parental::Control *parental ) {
	bool send = true;
	if (_parental && parental && (*parental == *_parental)) {
		send = false;
	}

	DEL(_parental);
	_parental = parental;

	if (send) {
		notify();
	}
}

//	User
bool Channel::isFavorite() const {
	return _isFavorite;
}

void Channel::toggleFavorite() {
	_isFavorite = !_isFavorite;
	notify(true);
}

void Channel::toggleBlocked() {
	CHECK_SESSION;
	_isBlocked = !_isBlocked;
	notify(true);
}

void Channel::unblock() {
	CHECK_SESSION;
	if (_isBlocked) {
		_isBlocked = false;
		notify(true);
	}
}

bool Channel::isBlocked() const {
	return _isBlocked;
}

Channel::Changed &Channel::onChanged() {
	return _onChanged;
}

void Channel::notify( bool markDirty/*=false*/ ) {
	if (isOnline()) {
		_onChanged(this);
		if (markDirty) {
			_tuner->channels()->save();
		}
	}
}

void Channel::debug() const {
	LDEBUG( "channel", "name=%s(%d), channel=%s",
		_name.c_str(), _channelID, _channel.c_str() );
}

//	Tuner implementation
void Channel::tuner( Tuner *tuner ) {
	DTV_ASSERT(tuner);
	_tuner = tuner;
	if (_tunerName.empty()) {
		_tunerName = tuner->name();
	}
}

bool Channel::isOnline() const {
	return (_channelID != -1 && _tuner) ? true : false;
}

Tuner *Channel::tuner() const {
	DTV_ASSERT(_tuner);
	return _tuner;
}

const std::string &Channel::tunerName() const {
	return _tunerName;
}

const std::string &Channel::uri() const {
	return _uri;
}

void Channel::uri( const std::string &id ) {
	_uri = id;
}

}

