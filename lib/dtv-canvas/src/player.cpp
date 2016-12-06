/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "player.h"
#include "mediaplayer.h"
#include "singlemediaplayer.h"
#include "impl/dummy/player.h"
#include "generated/config.h"
#if CANVAS_PLAYER_USE_VLC
#	include "impl/vlc/player.h"
#endif
#if CANVAS_PLAYER_USE_XINE
#	include "impl/xine/player.h"
#endif
#if CANVAS_PLAYER_USE_GST
#	include "impl/gst/player.h"
#endif
#if CANVAS_PLAYER_USE_FFMPEG
#	include "impl/ffmpeg/player.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/log.h>
#include <util/assert.h>

#if CANVAS_PLAYER_USE_VLC
#	define GUI_PLAYER_DEFAULT "vlc"
#elif CANVAS_PLAYER_USE_XINE
#	define GUI_PLAYER_DEFAULT "xine"
#elif CANVAS_PLAYER_USE_GST
#	define GUI_PLAYER_DEFAULT "gst"
#else
#	define GUI_PLAYER_DEFAULT "dummy"
#endif

namespace canvas {

REGISTER_INIT_CONFIG( gui_player ) {
	root().addNode( "player" )
		.addValue( "use", "Player to initialize", std::string(GUI_PLAYER_DEFAULT) )
		.addValue( "single", "Use single player", false );
};

/**
 * Método estático para crear instancias de @c Player según la opción seteada en el árbol de configuración (Ver módulo @em dtv-util}.
 * @return Nueva instancia de @c Player
 */
Player *Player::create( const std::string &val /*=""*/ ) {
	std::string use = val.empty() ? util::cfg::getValue<std::string> ("gui.player.use") : val;
	LINFO( "player", "Using player: use=%s", use.c_str() );

#if CANVAS_PLAYER_USE_VLC
	if (use == "vlc") {
		return new vlc::Player();
	}
#endif

#if CANVAS_PLAYER_USE_XINE
	if (use == "xine") {
		return new xine::Player();
	}
#endif

#if CANVAS_PLAYER_USE_GST
	if (use == "gst") {
		return new gst::Player();
	}
#endif

#if CANVAS_PLAYER_USE_FFMPEG
	if (use == "ffmpeg") {
		return new ffmpeg::Player();
	}
#endif

	return new dummy::Player();
}

/**
 * Constructor base. Luego de crear una instancia se debe llamar a initialize().
 */
Player::Player()
{
	_initialized = false;
	_singlePlayer = NULL;
	_currentActive = NULL;
	_useSinglePlayer = util::cfg::getValue<bool> ("gui.player.single");
}

/**
 * Destructor base. Antes de destruir una instancia se debe llamar a finalize().
 */
Player::~Player()
{
	DTV_ASSERT(!_initialized);
	DTV_ASSERT(!_singlePlayer);
	DTV_ASSERT(!_currentActive);
}

/**
 * Inicializa el estado del @c Player.
 * @return true si la inicialización finalizo correctamente, false en caso contrario.
 */
bool Player::initialize() {
	DTV_ASSERT(!_initialized);
	if (init()) {
		_singlePlayer = NULL;
		_currentActive = NULL;
		_initialized = true;
	}
	return _initialized;
}

bool Player::init() {
	return true;
}

/**
 * Finaliza el estado del @c Player.
 */
void Player::finalize() {
	if (_initialized) {
		if (_players.size()) {
			LWARN( "Player", "Not all players destroyed" );
		}
		CLEAN_ALL( MediaPlayer *,_players );

		fin();

		DEL(_singlePlayer);
		_currentActive = NULL;
		_initialized = false;
	}
}

void Player::fin() {
}

MediaPlayer *Player::create( System *sys ) {
	MediaPlayer *player;

	DTV_ASSERT(_initialized);
	DTV_ASSERT(sys);

	//	Check if we can create more media players
	if (!supportMultiplePlayers()) {
		if (!_singlePlayer) {
			//	Create real media player
			_singlePlayer = createMediaPlayer(sys);
			_singlePlayer->mainLoopThreadId(_threadId);
		}

		player = new single::MediaPlayer(sys, this, _singlePlayer);
	}
	else {
		player = createMediaPlayer(sys);
		player->mainLoopThreadId(_threadId);
	}

	DTV_ASSERT(player);
	_players.push_back( player );

	LDEBUG( "Player", "Create player: player=%p", player );

	return player;
}

void Player::destroy( MediaPlayerPtr &player ) {
	std::vector<MediaPlayer *>::iterator it=std::find( _players.begin(), _players.end(), player );
	if (it != _players.end()) {
		(*it)->stop();
		removeFromQueue(*it);
		delete (*it);
		_players.erase( it );
	}
	else {
		LERROR( "Player", "Player not found: player=%p", player );
		if (player) {
			DTV_ASSERT(false);
		}
	}
	player = NULL;
}

bool Player::supportMultiplePlayers() const {
	return !_useSinglePlayer;
}

//	Single player implementation
void Player::enqueue( MediaPlayer *mp ) {
	LDEBUG( "Player", "Enquque player begin: mp=%p, _currentActive=%p, queued=%d", mp, _currentActive, _activePlayers.size() );
	if (_currentActive != mp) {
		MediaPlayer *tmp = _currentActive;
		_currentActive = mp;
		if (tmp) {
			tmp->onEnqueued();
			tmp->stop();
			_activePlayers.push_front(std::make_pair(tmp, tmp->url()));
		}
		removeFromQueue(_currentActive);
	}
	LDEBUG( "Player", "Enquque player end: mp=%p, _currentActive=%p, queued=%d", mp, _currentActive, _activePlayers.size() );
}

bool Player::findMP( MediaPlayer *mp, const QueuedPlayer &queued ) {
	return mp == queued.first;
}

void Player::dequeue( MediaPlayer *mp ) {
	LDEBUG( "Player", "Dequeue player begin: mp=%p, _currentActive=%p, queued=%d", mp, _currentActive, _activePlayers.size() );
	if (_currentActive == mp) {
		if (_activePlayers.empty()) {
			_currentActive = NULL;
		} else {
			const QueuedPlayer &qp = _activePlayers.front();
			_currentActive = qp.first;
			std::string url = qp.second;
			_activePlayers.pop_front();

			_currentActive->play(url);
			_currentActive->onDequeued();
		}
	}
	else {
		removeFromQueue( mp );
	}
	LDEBUG( "Player", "Dequeue player end: mp=%p, _currentActive=%p, queued=%d", mp, _currentActive, _activePlayers.size() );
}

bool Player::isCurrent( MediaPlayer *mp ) const {
	return _currentActive == mp;
}

MediaPlayer* Player::currentActive() const {
	return supportMultiplePlayers() ? NULL : _currentActive;
}

void Player::removeFromQueue( MediaPlayer *mp ) {
	std::deque<QueuedPlayer>::iterator it = std::find_if(_activePlayers.begin(), _activePlayers.end(), boost::bind(&Player::findMP, this, mp, _1));
	if (it!=_activePlayers.end()) {
		_activePlayers.erase(it);
	}
}

size_t Player::enqueuedCount() const {
	return _activePlayers.size();
}

void Player::mainLoopThreadId( const boost::thread::id &id ) {
	_threadId = id;
	BOOST_FOREACH( MediaPlayer *mp, _players ) {
		mp->mainLoopThreadId(_threadId);
	}
}

const boost::thread::id &Player::mainLoopThreadId() const {
	return _threadId;
}

}
