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
#include <util/assert.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <util/cfg/configregistrator.h>
#include <vlc/vlc.h>
#include <string.h>
#include <stdio.h>

namespace canvas {
namespace vlc {

REGISTER_INIT_CONFIG( gui_player_vlc ) {
	root().addNode( "vlc" )
		.addValue( "quiet", "Vlc --quiet option", true );
};

Player::Player()
{
	_vlc = NULL;
}

Player::~Player()
{
	DTV_ASSERT(!_vlc);
}

libvlc_instance_t *Player::instance() const {
	return _vlc;
}

//	Initialization
bool Player::init() {
	LINFO( "vlc", "Initialize" );
	const char *args[] = {
		"-I", "dummy", /* Don't use any interface */
		"--no-disable-screensaver",
		"--no-osd",
		"--no-video-title-show",
		"--ignore-config",
		"--mouse-hide-timeout=5000",
		"--no-stats",
		//		"--extraintf=logger", // Log anything
		//		"--verbose=2", // Be much more verbose then normal for debugging purpose
		"--no-xlib",
		//  No log if set
		util::cfg::getValue<bool>("gui.player.vlc.quiet") ? "--quiet" : "--verbose=2",
		NULL
	};
	int argc = sizeof(args)/sizeof(args[0]);
	argc--;
	_vlc = libvlc_new( argc, args );
	return _vlc ? true : false;
}

void Player::fin() {
	LINFO( "vlc", "Finalize" );
	libvlc_release (_vlc);
	_vlc = NULL;
}

//	Media player
canvas::MediaPlayer *Player::createMediaPlayer( System *sys ) const {
	return new MediaPlayer( sys, (Player *)this );
}

}
}

