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

#include "zapper.h"
#include "provider.h"
#include "player.h"
#include "audio.h"
#include "screen.h"
#include "../../middleware/middleware.h"
#include "../../dvb/delegate.h"
#include <canvas/factory.h>
#include <util/cfg/cfg.h>
#include <util/log.h>
#include <util/assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define	FACTORY_RESET_CODE	164

namespace zapper {

Zapper *Zapper::create( const std::string &name ) {
	return new st::Zapper( name );
}

}

namespace st {

class Factory : public canvas::Factory {
public:
	Factory() {}
	virtual ~Factory() {}

	virtual Screen *createScreen() const {
		return new Screen();
	}

	virtual canvas::Player *createPlayer() const {
		return new Player();
	}

	virtual canvas::Audio *createAudio() const {
		return new Audio();
	}
};

class DVBDelegate : public zapper::dvb::Delegate {
public:
	explicit DVBDelegate( zapper::Zapper *zapper ) : zapper::dvb::Delegate(zapper) {}

protected:
	virtual tuner::Provider *createProviderImpl() const {
		return new Provider();
	}

	virtual tuner::player::Delegate *createPlayerDelegate() const {
		return pvr::dvb::Delegate::createPlayerDelegate( "av" );
	}
};

Zapper::Zapper( const std::string &name )
	: zapper::Zapper( name )
{
}

Zapper::~Zapper()
{
}

bool Zapper::initHardware() {
	LINFO("st", "Start hardware");

	//	Setup mpeg player to use AV player
	DTV_ASSERT( util::cfg::getValue<bool>( "zapper.mpeg.cc" ) == true );

	//	Initialize SDK
	if (!STSDK_Init()) {
		LERROR("st", "Cannot initialize hardware");
		return false;
	}

	if(isResetButtonPressed()) {
		LINFO("st", "Requested Factory reset.");
		exitCode( FACTORY_RESET_CODE );
		return false;
	}

	return true;
}

void Zapper::finHardware() {
	LINFO("st", "Stop hardware");
	STSDK_Term();
}

pvr::dvb::Delegate *Zapper::createTunerDelegate() const {
	return new DVBDelegate( (zapper::Zapper *)this );
}

canvas::Factory *Zapper::createCanvas() const {
	return new Factory();
}

util::key::type Zapper::translateKey( util::key::type key ) {
	if (!middleware()->isAnyRunning()) {
		if (key == util::key::green) {
			//	Translate to epg if ginga is not running
			key = util::key::epg;
		}
		else if (key == util::key::red) {
			//	Translate to chlist if ginga is not running
			key = util::key::chlist;
		}
	}
	return key;
}

}
