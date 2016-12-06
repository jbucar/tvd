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

#include "delegate.h"
#include "../captionviewer.h"
#include "../zapper.h"
#include "generated/config.h"
#include <mpegparser/service/extension/player/player.h>
#include <mpegparser/service/extension/player/extension.h>
#include <mpegparser/service/extension/time/timeextension.h>
#include <mpegparser/provider/provider.h>
#include <mpegparser/resourcemanager.h>
#include <util/cfg/cfg.h>
#include <util/log.h>
#include <boost/filesystem.hpp>

namespace zapper {
namespace dvb {

namespace bfs = boost::filesystem;

Delegate::Delegate( Zapper *zapper )
	: _zapper(zapper)
{
}

Delegate::~Delegate()
{
}

util::task::Dispatcher *Delegate::dispatcher() const {
	return _zapper->dispatcher();
}

tuner::ResourceManager *Delegate::createResources() const {
	//	Create resources manager from ramDisk
	bfs::path tmp = _zapper->ramDisk();
	tmp /= "resources";
	return new tuner::ResourceManager(
		tmp.string(),
		RESOURCES_MEMORY_BLOCKS,
		RESOURCES_MAX_MODULES,
		RESOURCES_MAX_MODULE_SIZE
	);
}

const std::string Delegate::spec() const {
	return util::cfg::getValue<std::string>("zapper.mpeg.spec");
}

bool Delegate::withEPG() const {
	return util::cfg::getValue<bool>("zapper.mpeg.useEPG");
}

bool Delegate::withEWBS() const {
	return util::cfg::getValue<bool>("zapper.mpeg.useEWBS");
}

bool Delegate::withCC() const {
	return util::cfg::getValue<bool>("zapper.mpeg.cc");
}

int Delegate::areaCode() const {
	return util::cfg::getValue<int>("zapper.areaCode");
}

tuner::player::Delegate *Delegate::createPlayerDelegate() const {
	const std::string &use = util::cfg::getValue<std::string>("zapper.mpeg.player");
	return pvr::dvb::Delegate::createPlayerDelegate( use );
}

pvr::CaptionPlayer *Delegate::createCaptionPlayer( bool isCC ) const {
	return new CaptionViewer(_zapper->sys(), isCC ? 0 : 1 );
}

}
}

