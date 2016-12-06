/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "module.h"
#include "channels.h"
#include "channel.h"
#include "player.h"
#include "display.h"
#include "middleware.h"
#include "tuner.h"
#include "update.h"
#include "control.h"
#include "log.h"
#include "net.h"
#include "network.h"
#include "storage.h"
#include "mixer.h"
#include <zapper/zapper.h>
#include <boost/foreach.hpp>
#include <boost/signals2.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace luaz {
namespace module {

//	Implementation
namespace impl {
	DO_MODULE(tuner);
	DO_MODULE(control);
	DO_MODULE(channels);
	DO_MODULE(channel);
	DO_MODULE(player);
	DO_MODULE(display);
	DO_MODULE(middleware);
	DO_MODULE(update);
	DO_MODULE(log);
	DO_MODULE(net);
	DO_MODULE(network);
	DO_MODULE(storage);
	DO_MODULE(mixer);

	std::vector<Module *> modules;
}

//	Services virtual methods
void addModule( Module *mod ) {
	impl::modules.push_back( mod );
}

void initialize( zapper::Zapper *zapper ) {
	addModule(&impl::mod_tuner);
	addModule(&impl::mod_control);
	addModule(&impl::mod_channels);
	addModule(&impl::mod_channel);
	addModule(&impl::mod_player);
	addModule(&impl::mod_display);
	addModule(&impl::mod_middleware);
	addModule(&impl::mod_update);
	addModule(&impl::mod_log);
	addModule(&impl::mod_net);
	addModule(&impl::mod_storage);
	addModule(&impl::mod_network);
	addModule(&impl::mod_mixer);

	BOOST_FOREACH( Module *mod, impl::modules ) {
		if (mod->initialize) {
			(*mod->initialize)( zapper );
		}
	}
}

void finalize() {
	BOOST_FOREACH( Module *mod, impl::modules ) {
		if (mod->finalize) {
			(*mod->finalize)();
		}
	}
}

void start( lua_State *st ) {
	BOOST_FOREACH( Module *mod, impl::modules ) {
		if (mod->start) {
			(*mod->start)( st );
		}
	}
}

void stop() {
	BOOST_FOREACH( Module *mod, impl::modules ) {
		if (mod->stop) {
			(*mod->stop)();
		}
	}
}

}
}
