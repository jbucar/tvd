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

#pragma once

#include "types.h"
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include <deque>

namespace canvas {

class System;
class MediaPlayer;
typedef MediaPlayer* MediaPlayerPtr;

/**
 * Representa un reproductor de video.
 * A partir de esta clase se pueden crear instancias de @c MediaPlayer para reproducir videos.
 */
class Player {
public:
	Player();
	virtual ~Player();

	//	Initialization
	bool initialize();
	void finalize();

	/**
	 * Crea un @c MediaPlayer utilizando el @c System @b sys.
	 * @param sys Instancia de la clase @c System.
	 * @return La nueva instancia de @c MediaPlayer.
	 */
	MediaPlayer *create( System *sys );
	void destroy( MediaPlayerPtr &player );

	//	Single player (implementation)
	void enqueue( MediaPlayer *mp );
	void dequeue( MediaPlayer *mp );
	bool isCurrent( MediaPlayer *mp ) const;
	MediaPlayer *currentActive() const;
	virtual bool supportMultiplePlayers() const;

	//	Instance creation
	static Player *create( const std::string &val="" );

	//	Implementation
	void mainLoopThreadId( const boost::thread::id &id );
	const boost::thread::id &mainLoopThreadId() const;

protected:
	//	Types
	typedef std::pair<MediaPlayer *, std::string> QueuedPlayer;

	//	Media player
	virtual bool init();
	virtual void fin();
	virtual MediaPlayer *createMediaPlayer( System *sys ) const=0;
	bool findMP( MediaPlayer *mp, const QueuedPlayer &queued );
	void removeFromQueue( MediaPlayer *mp );
	size_t enqueuedCount() const;

private:
	bool _initialized;
	std::vector<MediaPlayer *> _players;
	boost::thread::id _threadId;

	//	Single player implementation
	bool _useSinglePlayer;
	MediaPlayer *_singlePlayer;
	MediaPlayer *_currentActive;
	std::deque<QueuedPlayer> _activePlayers;
};

}

