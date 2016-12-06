/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <canvas/types.h>
#include <vector>
#include <map>
#include <string>

namespace connector {
	class EditingCommandData;
}

namespace player {

class System;
class Player;

/**
 * La reproducción de los players ocurre dentro de un dispositivo, el cual es representado por un objeto @c Device.
 * Todos los objetos @c Device pertenecen a un @c System. El objeto @c Device actúa como fábrica
 * de los @c Players y administra a los mismos.
 */
class Device {
public:
	explicit Device( System *sys );
	virtual ~Device();

	//	Player methods
	Player *create( const std::string &url, const std::string &mime );
	virtual Player *create( type::type typeOfPlayer );
	void destroy( Player *player );

	//	Initialize
	virtual bool initialize();
	virtual void finalize();

	//	Start/stop
	virtual void start();
	virtual void stop();
	void exit();

	//	Input manager
	virtual void onReserveKeys( const ::util::key::Keys &keys );
	void dispatchKey( ::util::key::type keystroke, bool isUp );
	void dispatchButton( unsigned int button, int x, int y, bool isPress );
	void dispatchEditingCommand( connector::EditingCommandData *event );

	//	External resources
	bool download( const std::string &url, std::string &newFile );

	//	Factory methods
	virtual canvas::MediaPlayer *createMediaPlayer();
	virtual void destroy( canvas::MediaPlayerPtr &mp );

	virtual canvas::WebViewer *createWebViewer( canvas::Surface * );

	//	Getters
	System *systemPlayer() const;
	canvas::System *system() const;
	virtual bool haveVideo() const;
	virtual bool haveAudio() const;

protected:
	typedef std::map<std::string,std::string> DownloadCache;

	Player *addPlayer( Player *player );
	bool tryDownload( const std::string &url, std::string &file );
	virtual canvas::Factory *createFactory() const;

private:
	System *_sys;
	canvas::System *_canvas;
	std::vector<Player *> _players;
	DownloadCache _downloadCache;
};

}

