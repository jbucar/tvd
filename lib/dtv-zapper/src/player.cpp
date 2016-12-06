/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "player.h"
#include "zapper.h"
#include <pvr/player.h>
#include <pvr/pvr.h>
#include <canvas/mediaplayer.h>
#include <canvas/player.h>
#include <canvas/system.h>
#include <util/cfg/cfg.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

namespace zapper {
namespace channel {

Player::Player( Zapper *zapper )
	: _zapper(zapper)
{
	DTV_ASSERT(zapper);

	_player = NULL;
	_media = NULL;
}

Player::~Player()
{
	DTV_ASSERT(!_player);
	DTV_ASSERT(!_media);
}

bool Player::initialize() {
	LDEBUG( "Player", "Initialize" );

	{	//	Setup canvas::Mediaplayer
		canvas::System *sys = zapper()->sys();
		DTV_ASSERT(sys);

		//	Create media player
		_media = sys->player()->create( sys );
		if (!_media) {
			LWARN( "Player", "Cannot create mediaplayer" );
			return false;
		}
		_media->setFullScreen();
	}

	//	Setup channel player
	_player = zapper()->pvr()->allocPlayer();
	if (!_player) {
		LERROR( "Player", "Cannot create channel player" );
		return false;
	}
	_player->onMediaChanged( boost::bind(&Player::onMedia,this,_1,_2) );

	return true;
}

void Player::finalize() {
	LDEBUG( "Player", "Finalize" );
	zapper()->pvr()->freePlayer( _player );
	canvas::MediaPlayer::destroy( _media );
}

void Player::start() {
	player()->start();
}

void Player::stop() {
	player()->stop();
}

void Player::onMedia( const std::string &url, bool started ) {
	if (started) {
		_media->play( url );
	}
	else {
		_media->stop();
	}
}

//	Change channel
Player::ChangeChannelSignal &Player::onChangeChannel() {
	return player()->onChangeChannel();
}

int Player::change( int channel ) {
	return player()->change( channel );
}

int Player::current() const {
	return player()->current();
}

const std::string &Player::currentNetworkName() const {
	return player()->currentNetworkName();
}

int Player::nextChannel( int first, int factor ) const {
	return player()->nextChannel( first, factor );
}

int Player::nextFavorite( int first, int factor ) const {
	return player()->nextFavorite( first, factor );
}

Player::ChannelProtectedSignal &Player::onChannelProtected() {
	return player()->onChannelProtected();
}

void Player::status( int &signal, int &snr ) {
	player()->status( signal, snr );
}

//	Player methods
void Player::setFullScreen() {
	media()->setFullScreen();
}

void Player::resize( int x, int y, int w, int h ) {
	media()->moveResize( canvas::Rect(x, y, w, h) );
}

void Player::mute( bool needMute ) {
	media()->mute( needMute );
}

void Player::volume( canvas::Volume vol ) {
	media()->volume( vol );
}

canvas::audio::channel::type Player::getAudio() const {
	canvas::audio::channel::type audio = canvas::audio::channel::unknown;
	const canvas::AudioStreamInfo *aInfo = media()->streamsInfo().audioStreamInfo();
	if (aInfo) {
		audio = canvas::audio::get(*aInfo);
	}
	return audio;
}

canvas::aspect::type Player::getAspect() const {
	canvas::aspect::type aspect = canvas::aspect::unknown;
	const canvas::VideoStreamInfo *vInfo = media()->streamsInfo().videoStreamInfo();
	if (vInfo) {
		aspect = canvas::aspect::get(*vInfo);
	}
	return aspect;
}

canvas::mode::type Player::getResolution() const {
	canvas::mode::type mode = canvas::mode::unknown;
	const canvas::VideoStreamInfo *vInfo = media()->streamsInfo().videoStreamInfo();
	if (vInfo) {
		mode = canvas::mode::get(*vInfo);
	}
	return mode;
}

bool Player::showMobile() const {
	return player()->showMobile();
}

void Player::showMobile( bool show ) const {
	player()->showMobile(show);
}

int Player::nextVideo() {
	return player()->nextVideo();
}

int Player::videoCount() const {
	return player()->videoCount();
}

void Player::videoInfo( pvr::info::Basic &info, int id/*=-1*/ ) const {
	return player()->videoInfo( info, id );
}

int Player::nextAudio() {
	return player()->nextAudio();
}

int Player::audioCount() const {
	return player()->audioCount();
}

void Player::audioInfo( pvr::info::Audio &info, int id/*=-1*/ ) const {
	player()->audioInfo( info, id );
}

int Player::nextSubtitle() {
	return player()->nextSubtitle();
}

int Player::subtitleCount() const {
	return player()->subtitleCount();
}

void Player::subtitleInfo( pvr::info::Basic &info, int id/*=-1*/ ) const {
	player()->subtitleInfo( info, id );
}

bool Player::playVideoStream( int sID, std::string &url ) {
	return player()->playVideoStream( sID, url );
}

void Player::stopVideoStream( int sID ) {
	player()->stopVideoStream( sID );
}

bool Player::isBlocked() const {
	return player()->isProtected();
}

//	Getters
canvas::MediaPlayer *Player::media() const {
	DTV_ASSERT(_media);
	return _media;
}

pvr::Player *Player::player() const {
	DTV_ASSERT(_player);
	return _player;
}

Zapper *Player::zapper() const {
	return _zapper;
}

}
}
