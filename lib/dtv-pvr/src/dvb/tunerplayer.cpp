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

#include "tunerplayer.h"
#include "tuner.h"
#include "delegate.h"
#include "show.h"
#include "captionviewer.h"
#include "../channel.h"
#include "../captionplayer.h"
#include <mpegparser/demuxer/descriptors/demuxers.h>
#include <mpegparser/muxer/ts/out/rtp.h>
#include <mpegparser/service/extension/ewbs/extension.h>
#include <mpegparser/service/extension/player/tsdelegate.h>
#include <mpegparser/service/extension/player/caption/delegate.h>
#include <mpegparser/service/extension/player/esplayer.h>
#include <mpegparser/service/extension/player/streaminfo.h>
#include <mpegparser/service/extension/player/extension.h>
#include <mpegparser/service/extension/player/player.h>
#include <util/assert.h>
#include <util/log.h>

namespace pvr {
namespace dvb {

TunerPlayer::TunerPlayer( Tuner *tuner )
	: _tuner(tuner)
{
	DTV_ASSERT(tuner);
	_player = NULL;
	_ccPlayer = NULL;
}

TunerPlayer::~TunerPlayer()
{
	DTV_ASSERT(!_player);
}

bool TunerPlayer::initialize() {
	LDEBUG( "dvb", "Initialize channel player" );

	//	Create MPEG player
	_player = _tuner->delegate()->createPlayer();
	if (!_player) {
		LWARN( "dvb", "Cannot create mpeg player extension" );
		return false;
	}

	if (_tuner->delegate()->withCC()) {
		//	Add CC stream player
		CaptionViewer *view = _tuner->delegate()->createCaptionViewer(true);
		tuner::player::Delegate *dlg = new tuner::player::es::cc::Delegate(view);
		_player->addPlayer( new tuner::player::es::Player( dlg, tuner::pes::type::subtitle, -1) );
	}

	//	Initialize player
	_player->onStarted( boost::bind(&TunerPlayer::onStartMedia,this,_1) );
	if (!_player->initialize()) {
		LWARN( "dvb", "Cannot initialize mpeg player" );
		return false;
	}

	//	Attach extension to tuner
	_tuner->attach( _player->extension() );

	return true;
}

void TunerPlayer::finalize() {
	LDEBUG( "dvb", "Finalize channel player" );

	//	Detach extension from tuner
	_tuner->detach( _player->extension() );

	_ccPlayer = NULL;

	//	Finalize player
	_player->finalize();
	DEL(_player);
}

//	Player methods
void TunerPlayer::play( Channel *ch ) {
	LDEBUG( "dvb", "Play channel" );

	id::ChannelID id;
	if (!id::fromString( ch->uri(), id )) {
		LWARN( "dvb", "Play: Cannot parse customID" );
		return;
	}

	//	Attach demuxer to service
	player()->playService( id.srvID );

	//	Play super impose if there are a emergency active
	if (_tuner->inEmergency( id.srvID )) {
		LDEBUG( "dvb", "Add super impose stream player" );

		//	Create super impose es player
		CaptionViewer *view = _tuner->delegate()->createCaptionViewer(false);
		tuner::player::Delegate *dlg = new tuner::player::es::cc::Delegate(view);

		//	Start
		player()->startAux( dlg, tuner::pes::type::subtitle );
	}

	//	Start service on tuner
	if (!_tuner->startService( id.networkID, id.srvID )) {
		LWARN( "dvb", "Cannot start MPEG service" );
	}
}

void TunerPlayer::stop( Channel *ch ) {
	LDEBUG( "dvb", "Stop channel" );

	id::ChannelID id;
	if (!id::fromString( ch->uri(), id )) {
		LWARN( "dvb", "Stop: Cannot parse customID" );
		return;
	}

	player()->stopService();

	_tuner->stopService( id.srvID );
}

int TunerPlayer::nextVideo() {
	return player()->nextVideo();
}

int TunerPlayer::videoCount() const {
	return player()->videoCount();
}

void TunerPlayer::videoInfo( int streamID, Channel *ch, info::Basic &info ) const {
	DTV_ASSERT(ch);
	const tuner::player::Stream *s = player()->getVideo( streamID );
	if (s) {
		info.pid = s->info.pid;
		info.codec = s->sInfo->name();
	}
}

int TunerPlayer::nextAudio() {
	return player()->nextAudio();
}

int TunerPlayer::audioCount() const {
	return player()->audioCount();
}

void TunerPlayer::audioInfo( int streamID, Channel *ch, info::Audio &info ) const {
	DTV_ASSERT(ch);
	const tuner::player::Stream *s = player()->getAudio( streamID );
	if (s) {
		info.pid = s->info.pid;
		info.codec = s->sInfo->name();

		bool set=false;

		//	Get current show
		Show *show = (Show *)ch->getCurrentShow();
		DTV_ASSERT(show);

		//	Parse audio language from current show
		set = show->audioLanguage( s->tag, info.lang );
		if (!set) {
			const tuner::desc::Descriptors &descs = s->info.descriptors;
			tuner::desc::LanguageDescriptor desc;
			if (DESC_PARSE( descs, language, desc )) {
				info.lang = desc.language.name();
			}
		}
	}
}

int TunerPlayer::nextSubtitle() {
	return player()->nextSubtitle();
}

int TunerPlayer::subtitleCount() const {
	return player()->subtitleCount();
}

void TunerPlayer::subtitleInfo( int streamID, Channel * /*ch*/, info::Basic &info ) const {
	const tuner::player::Stream *s = player()->getSubtitle( streamID );
	if (s) {
		info.pid = s->info.pid;
		info.codec = s->sInfo->name();
	}
}

int TunerPlayer::playVideoStream( int streamID, std::string &url ) {
	//	Setup es player
	tuner::muxer::ts::Output *out = new tuner::muxer::ts::RTPOutput( "127.0.0.1", 5000+streamID );
	tuner::player::Delegate *dlg = new tuner::player::ts::Delegate( streamID, out );

	//	Add and start
	int id = player()->startAux( dlg, tuner::pes::type::video, streamID );
	if (id < 0) {
		LERROR( "dvb", "Cannot play video stream: streamID=%d", streamID );
		return -1;
	}

	url = out->url();

	return id;
}

void TunerPlayer::stopVideoStream( int playerID ) {
	player()->stopAux( playerID );
}

//	Getters
const std::string TunerPlayer::url() const {
	return player()->url();
}

util::task::Dispatcher *TunerPlayer::dispatcher() const {
	return _tuner->delegate()->dispatcher();
}

tuner::player::Player *TunerPlayer::player() const {
	DTV_ASSERT(_player);
	return _player;
}

void TunerPlayer::onStartMedia( bool start ) {
	notifyStart( start );
}

}
}

