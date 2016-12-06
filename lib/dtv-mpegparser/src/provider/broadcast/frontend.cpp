/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "frontend.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/cfg/configregistrator.h>
#include <util/serializer/xml.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace broadcast {

REGISTER_INIT_CONFIG( tuner_frontend ) {
	root().addNode( "frontend" )
		.addValue( "system", "DVB system", "isdbt" )
		.addValue( "config", "Config file", "" );
}

template<typename T>
static void addChannels( Configs &cfg, const std::vector<T *> &chs ) {
	BOOST_FOREACH( T *ch, chs ) {
		cfg.push_back( ch );
	}
}

Frontend::Frontend()
{
}

Frontend::~Frontend()
{
	DTV_ASSERT(_channels.size() == 0);
}

//	Config
void Frontend::cleanConfig() {
	CLEAN_ALL( Config *, _channels );
}

bool Frontend::loadConfig() {
	const std::string &system = util::cfg::getValue<std::string>("tuner.frontend.system");
	const std::string &file = util::cfg::getValue<std::string>("tuner.frontend.config");
	util::serialize::XML ser( file );

	if (system == "isdbt") {
		std::vector<IsdbtConfig *> chs;
		if (!ser.load( chs ) || chs.empty()) {
			IsdbtConfig::loadDefaults(chs);
		}
		addChannels(_channels,chs);
	}
	else if (system == "dvbs2" || system == "dvbs") {
		std::vector<SatelliteConfig *> chs;
		if (!ser.load( chs )) {
			LERROR( "Frontend", "Cannot load satellite transponders" );
			return false;
		}
		addChannels(_channels,chs);
	}

	//	Check for channels
	if (_channels.empty()) {
		LERROR( "Frontend", "Channel list empty" );
		return false;
	}

	return true;
}

//	Network
size_t Frontend::getCount() const {
	return _channels.size();
}

std::string Frontend::getName( size_t nIndex ) const {
	Config *cfg = get( nIndex );
	DTV_ASSERT(cfg);
	return cfg->name();
}

size_t Frontend::find( const std::string &name ) const {
	for (size_t i=0; i<_channels.size(); i++) {
		if (_channels[i]->name() == name) {
			return i;
		}
	}
	return size_t(-1);
}

bool Frontend::start( size_t nIndex ) {
	Config *cfg = get( nIndex );
	if (!cfg) {
		return false;
	}

	return cfg->start( this );
}

bool Frontend::start( IsdbtConfig * ) {
	LWARN( "Frontend", "Not implemented!" );
	return false;
}

bool Frontend::start( SatelliteConfig * ) {
	LWARN( "Frontend", "Not implemented!" );
	return false;
}

Config *Frontend::get( size_t nIndex ) const {
	if (nIndex >= _channels.size()) {
		LWARN( "Frontend", "Invalid index: index=%d, channels=%d", nIndex, _channels.size() );
		return NULL;
	}

	return _channels[nIndex];
}

}
}

