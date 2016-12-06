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
#include <xine.h>
#include <xine/xineutils.h>

namespace canvas {
namespace xine {

Player::Player()
{
	_xine = NULL;
}

Player::~Player()
{
	DTV_ASSERT(!_xine);
}

//	Initialization
bool Player::init() {
	LDEBUG("xine", "Init");
	
	//	Create xine object
	_xine = xine_new();
	if (!_xine) {
		LERROR("xine", "cannot create xine object");
		return false;
	}

	//	Set log verobity
	xine_engine_set_param(_xine, XINE_VERBOSITY_DEBUG, 1);

	{	//	Initialize xine
		std::string cfg = xine_get_homedir();
		cfg += "/.xine/config";
		LINFO("xine", "Using config file: cfg=%s", cfg.c_str());
		xine_config_load( _xine, cfg.c_str() );
		xine_init(_xine);
	}

	return true;
}

void Player::fin() {
	LDEBUG("xine", "Fin");
	if (_xine) {
		xine_exit(_xine);
		_xine = NULL;
	}
}

canvas::MediaPlayer *Player::createMediaPlayer( canvas::System *sys ) const {
	return new MediaPlayer( sys, _xine );
}

}
}
