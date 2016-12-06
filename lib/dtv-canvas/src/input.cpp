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

#include "input.h"
#include "system.h"
#include "generated/config.h"
#if CANVAS_INPUT_USE_LIRC
#       include "impl/lirc/inputlirc.h"
#endif
#if CANVAS_INPUT_USE_LINUX
#       include "impl/linux/inputlinux.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/mcr.h>


REGISTER_INIT_CONFIG( gui_input ) {
	root().addNode( "input" );
}

namespace canvas {

void Input::create( System *sys ) {
	DTV_ASSERT(sys);
	UNUSED(sys);
#if CANVAS_INPUT_USE_LIRC
	const bool &uselirc = util::cfg::getValue<bool> ("gui.input.lirc");
	if (uselirc) {
		sys->addInput( new input::lirc::InputLIRC(sys) );
	}
#endif
#if CANVAS_INPUT_USE_LINUX
	const bool &uselinux = util::cfg::getValue<bool> ("gui.input.linux.enabled");
	if (uselinux) {
		sys->addInput( new input::event::InputLinux(sys) );
	}
#endif
}

Input::Input( System *sys )
	: _sys( sys )
{
}

Input::~Input() {
}

bool Input::initialize() {
	return true;
}

void Input::finalize() {
}

System *Input::sys() const {
	return _sys;
}

}

