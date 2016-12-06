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

#include "player.h"
#include "channel.h"
#include "channels.h"
#include "tunerplayer.h"
#include "tuner.h"
#include "time.h"
#include "parental/session.h"
#include <util/settings/settings.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <math.h>

#define SETTINGS_CURRENT "pvr.player_%d.current"
#define SETTINGS_MOBILE  "pvr.player_%d.showMobile"

namespace pvr {

namespace filter {

Basic::Basic( bool showMobile )
	: _showMobile(showMobile) {}

bool Basic::filter( Channel *ch ) const {
	bool filterMobile = !(!_showMobile && ch->isMobile());
	bool filterProtected = ch->isProtected() ? false : true;
	return filterMobile && filterProtected;
}

Favorite::Favorite( bool showMobile )
	: Basic(showMobile) {}

bool Favorite::filter( Channel *prog ) const {
	return Basic::filter( prog ) && prog->isFavorite();
}

}

static const std::string settingsKey( const char *name, int id ) {
	return util::format( name, id );
}

Player::Player( int id )
{
	_id = id;
	_current = -1;
	_ch = NULL;
	_mp = NULL;
	_showMobile = false;
	_channelStarted = false;
	_channelProtected = false;
	_mediaStarted = false;
}

Player::~Player()
{
	DTV_ASSERT(!_ch);
	DTV_ASSERT(!_mp);
	DTV_ASSERT(!_channelStarted);
	DTV_ASSERT(!_channelProtected);
	DTV_ASSERT(!_mediaStarted);
}

void Player::start( bool loadOldChannel/*=true*/ ) {
	DTV_ASSERT(!_ch);
	DTV_ASSERT(!_mp);
	DTV_ASSERT(!_channelStarted);
	DTV_ASSERT(!_channelProtected);
	DTV_ASSERT(!_mediaStarted);

	LDEBUG( "Player", "Start player" );

	//	Setup channels notifications
	_sChannelRemoved = channels()->onChannelRemoved().connect( boost::bind(&Player::onChannelRemoved,this,_1,_2) );
	_sSessionChanged = session()->onActiveChanged().connect( boost::bind(&Player::onSessionChanged,this,_1) );
	_sTimeChanged = time::clock()->onTimeValidChanged().connect( boost::bind(&Player::onTimeValid,this,_1) );

	{	//	Load mobile
		bool show = false;
		settings()->get( settingsKey(SETTINGS_MOBILE,_id), show );
		showMobile( show );
	}

	if (loadOldChannel) {	//	Load current
		int startChannel = INVALID_CHANNEL;
		settings()->get( settingsKey(SETTINGS_CURRENT,_id), startChannel );
		change( startChannel );
	}
}

void Player::stop() {
	LDEBUG( "Player", "Stop player" );
	DTV_ASSERT(_sChannelRemoved.connected());

	stopCurrent();

	_sTimeChanged.disconnect();
	_sSessionChanged.disconnect();
	_sChannelRemoved.disconnect();
}

void Player::resetConfig() {
	showMobile( false );
	stopCurrent(true);
}

//	Status
int Player::current() const {
	return _current;
}

const std::string &Player::currentNetworkName() const {
	return _ch->network();
}

void Player::status( int &signal, int &quality ) {
	if (_ch) {
		DTV_ASSERT(_ch->isOnline());
		_ch->tuner()->status( signal, quality );
	}
	else {
		signal = -1;
		quality = -1;
	}
}

bool Player::isProtected() const {
	return _ch ? _ch->isProtected() : false;
}

Player::ChannelProtectedSignal &Player::onChannelProtected() {
	return _onProtected;
}

const std::string Player::tunerName() const {
	return _ch ? _ch->tunerName() : "";
}

int Player::change( int channelID ) {
	if (channelID != _current) {
		bool changed=false;

		LDEBUG( "Player", "Change to channel: chID=%d", channelID );

		//	Get channel from channel manager
		Channel *ch = channels()->get( channelID );
		if (ch && ch->isOnline()) {
			//	If same tuner, no reserve another player
			TunerPlayer *mp = (_ch && _ch->tunerName() == ch->tunerName()) ? _mp : ch->tuner()->reservePlayer();
			if (mp) {
				//	Stop current channel
				stopCurrent( false, (_mp == mp) ? false : true );

				//	Save and notify
				_current = ch->channelID();
				_ch = ch;
				changed=true;
				_sChannelChanged = ch->onChanged().connect( boost::bind(&Player::onChannelChanged,this,_1) );

				//	Setup channel player
				_mp = mp;
				mp->onMediaChanged( boost::bind(&Player::onChannelStarted,this,_1) );

				//	Start player
				_mp->play( ch );
			}
		}
		else if (channelID == INVALID_CHANNEL) {
			//	Stop current
			stopCurrent();
			changed=true;
		}

		if (changed) {
			_onChange( _current, ch );
			saveCurrent();
		}
	}
	return _current;
}

Player::ChangeChannelSignal &Player::onChangeChannel() {
	return _onChange;
}

Player::ShowChangedSignal &Player::onShowChanged() {
	return _onShowChanged;
}

void Player::onMediaChanged( const MediaChangedCallback &callback ) {
	_onMediaChanged = callback;
}

//	Notifications
void Player::onChannelStarted( bool start ) {
	LDEBUG( "Player", "Channel started changed: start=%d", start );
	DTV_ASSERT(_ch);
	if (_channelStarted != start) {
		_channelStarted = start;
		checkProtected();
		updateMedia();
	}
}

void Player::onChannelChanged( Channel *ch ) {
	LDEBUG( "Player", "Current channel changed" );
	DTV_ASSERT(_ch);
	if (ch == _ch) {
		if (checkProtected()) {
			updateMedia();
		}

		_onShowChanged();
	}
}

void Player::onSessionChanged( bool /*state*/ ) {
	if (_ch && checkProtected()) {
		updateMedia();
	}
}

void Player::onTimeValid( bool /*isValid*/ ) {
	if (_ch && checkProtected()) {
		updateMedia();
	}
}

void Player::onChannelRemoved( Channel *ch, bool removeAll ) {
	if (removeAll) {
		LDEBUG( "Player", "All channels removed, stop current" );
		//	Stop current
		stopCurrent(true);
	}
	else if (ch->channelID() == _current) {	//	Is my channel removed?
		LDEBUG( "Player", "The current channel is to be removed. Change to next channel" );
		//	Get next channel not blocked
		change( nextChannel( _current, 1 ) );
	}
}

//	Player state changed
void Player::notifyMediaChanged( bool start ) {
	LDEBUG( "Player", "Media changed: start=%d", start );
	_mediaStarted = start;
	if (!_onMediaChanged.empty()) {
		_onMediaChanged( _mp->url(), start );
	}
}

bool Player::checkProtected() {
	return changeProtected( _ch->isProtected() );
}

bool Player::changeProtected( bool isProtected ) {
	if (isProtected != _channelProtected) {
		_channelProtected = isProtected;
		_onProtected( isProtected );
		return true;
	}
	return false;
}

void Player::updateMedia() {
	DTV_ASSERT(_ch);

	LDEBUG( "Player", "Update media: channel(started=%d,protected=%d), media=%d",
		_channelStarted, _channelProtected, _mediaStarted );

	if (_channelStarted != _mediaStarted) {
		//	Need start?
		if (_channelStarted) {
			if (!_channelProtected) {
				notifyMediaChanged( true );
			}
		}
		else {	//	Need stop
			notifyMediaChanged( false );
		}
	}
	else if (_mediaStarted && _channelProtected) {	//	channel Started && media started -> stop
		notifyMediaChanged(false);
	}
}

void Player::stopCurrent( bool save /*=false*/, bool needReturnPlayer/*=true*/ ) {
	if (_current != INVALID_CHANNEL) {
		DTV_ASSERT(_ch);
		DTV_ASSERT(_mp);

		LDEBUG( "Player", "Stop current channel" );

		//	Disconnect from channel notifications
		_sChannelChanged.disconnect();

		//	Stop media
		_mp->stop( _ch );

		//	Force stop
		onChannelStarted(false);

		//	Returns player to tuner
		if (needReturnPlayer) {
			TunerPlayer::MediaChangedCallback empty;
			_mp->onMediaChanged( empty );
			_ch->tuner()->freePlayer(_mp);
		}
		_mp = NULL;

		//	Save state
		changeProtected(false);
		_current = INVALID_CHANNEL;
		_ch = NULL;
		if (save) {
			saveCurrent();
		}
	}
}

void Player::saveCurrent() {
	settings()->put( settingsKey(SETTINGS_CURRENT,_id), _current );
	settings()->commit();
}

//	Change channel utility
bool Player::showMobile() const {
	return _showMobile;
}

void Player::showMobile( bool show ) {
	if (_showMobile != show) {
		_showMobile = show;
		settings()->put( settingsKey(SETTINGS_MOBILE,_id), _showMobile );
		settings()->commit();
	}
}

int Player::nextChannel( int first, int factor ) const {
	filter::Basic filter(_showMobile);
	return next( first, factor, &filter );
}

int Player::nextFavorite( int first, int factor ) const {
	filter::Favorite filter(_showMobile);
	return next( first, factor, &filter );
}


#define EUCLIDEAN_REMINDER(a, b) ((a % b) >= 0) ? a % b : (a % b) + std::abs(b);

int Player::next( int firstChannelID, int factor, struct filter::ChannelFilter *filter ) const {
	DTV_ASSERT(filter);
	int result = INVALID_CHANNEL;
	const ChannelList &chs = channels()->getAll();
	size_t max = chs.size();
	if (max > 0) {
		//	Find index for firstChannelID
		int firstIndex = INVALID_CHANNEL;
		if (firstChannelID != INVALID_CHANNEL) {
			for (size_t p=0; p<max; p++) {
				Channel *ch=chs[p];
				if (ch->channelID() == firstChannelID) {
					firstIndex=(int)p;
				}
			}
		}

		//	Find next
		for (size_t p=0; p<max; p++) {
			if (factor == -1 && firstIndex == INVALID_CHANNEL) {
				firstIndex = max-1;
			}
			else {
				int tmp = int(firstIndex + factor );
				firstIndex = EUCLIDEAN_REMINDER(tmp, int(max));
			}

			Channel *ch=chs[firstIndex];
			if (filter->filter(ch)) {
				result = ch->channelID();
				break;
			}
		}
	}

	LTRACE( "Player", "Next channel: chID=%d, factor=%d, channels=%d, result=%d",
		firstChannelID, factor, max, result );

	return result;
}

//	Player methods
int Player::nextVideo() {
	return _mp ? _mp->nextVideo() : -1;
}

int Player::videoCount() const {
	return _mp ? _mp->videoCount() : 0;
}

void Player::videoInfo( info::Basic &info, int id /*=-1*/ ) const {
	if (_ch && _mp) {
		_mp->videoInfo(id,_ch,info);
	}
}

int Player::nextAudio() {
	return _mp ? _mp->nextAudio() : -1;
}

int Player::audioCount() const {
	return _mp ? _mp->audioCount() : 0;
}

void Player::audioInfo( info::Audio &info, int id /*=-1*/ ) const {
	if (_ch && _mp) {
		_mp->audioInfo(id,_ch,info);
	}
}

int Player::nextSubtitle() {
	return _mp ? _mp->nextSubtitle() : -1;
}

int Player::subtitleCount() const {
	return _mp ? _mp->subtitleCount() : 0;
}

void Player::subtitleInfo( info::Basic &info, int id /*=-1*/ ) const {
	if (_ch && _mp) {
		_mp->subtitleInfo(id,_ch,info);
	}
}

int Player::playVideoStream( int id, std::string &url ) {
	if (_ch && _mp) {
		return _mp->playVideoStream( id, url );
	}
	return -1;
}

void Player::stopVideoStream( int playerID ) {
	if (_ch && _mp) {
		_mp->stopVideoStream( playerID );
	}
}

}

