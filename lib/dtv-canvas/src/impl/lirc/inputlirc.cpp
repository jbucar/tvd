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

// To allow simulated events run lircd daemon with -a option: lircd -a
// To send events to lircd use: irsend SIMULATE "0000000000000000 00 button_name remote_name"
// Example: irsend SIMULATE "0000000000000000 00 RED mhp_rc"

#include "inputlirc.h"
#include "../../inputmanager.h"
#include "../../system.h"
#include <util/io/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <util/keydefs.h>
#include <util/fs.h>
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <lirc/lirc_client.h>

REGISTER_INIT_CONFIG( gui_input_lirc ) {
	root().addValue( "lirc", "LIRC input enabled", false );
}

namespace canvas {
namespace input {
namespace lirc {

InputLIRC::InputLIRC( System *sys )
	: Input(sys)
{
	_config=NULL;
}

InputLIRC::~InputLIRC() {
}

//	Getter
std::string InputLIRC::name() const {
	return "lirc";
}

bool InputLIRC::initialize() {
	LINFO("InputLIRC", "initialize!");

	//	Init lirc
	int socket = lirc_init((char *) "canvas", 1);
	if (socket < 0) {
		LWARN("InputLIRC", "cannot initialize lirc" );
		return false;
	}

	//	Read config
	std::string cfg = util::fs::make( util::fs::sysConfDir(), "lirc_input.conf" );
	if (lirc_readconfig( (char *)cfg.c_str(), &_config, NULL ) != 0) {
		LWARN("InputLIRC", "cannot read config file: file=%s", cfg.c_str() );
		lirc_deinit();
		return false;
	}

	//	Setup io
	_ioId = sys()->io()->addIO( socket, boost::bind(&InputLIRC::onEvent, this));
	if (!util::id::isValid(_ioId)) {
		LWARN("InputLIRC", "cannot register connection io");
		lirc_deinit();
		return false;
	}

	return true;
}

void InputLIRC::finalize() {
	LINFO("InputLIRC", "finalize!");

	sys()->io()->stopIO( _ioId );

	lirc_freeconfig(_config);
	lirc_deinit();
}

void InputLIRC::onEvent() {
	char *code;
	lirc_nextcode(&code);
	if (code != NULL) {
		LTRACE("InputLIRC", "On lirc event: code=%s", code );
		char *c;
		while ((lirc_code2char(_config,code,&c) == 0) && c != NULL) {
			util::key::type key = util::key::getKey(c);
			if (key) {
				LDEBUG("InputLIRC", "dispatchKey:%s", util::key::getKeyName(key));
				sys()->input()->dispatchKey( key, false );
				sys()->input()->dispatchKey( key, true );
			} else {
				LWARN("InputLIRC", "Key not maped code='%s'", code);
			}
		}
		free(code);
	}
	else {
		LWARN("InputLIRC", "cannot get valid key code" );
	}
}

}
}
}

