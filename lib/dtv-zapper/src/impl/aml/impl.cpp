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

#include "screen.h"
#include "window.h"
#include "player.h"
#include "provider.h"
#include "../../dvb/delegate.h"
#include "../../zapper.h"
#include <canvas/factory.h>
#include <canvas/audio/audioplayer.h>
#include <util/cfg/cfg.h>

namespace aml {

class Factory : public canvas::Factory {
public:
	Factory() {
	}

	virtual ~Factory() {}

	virtual Screen *createScreen() const {
		return new Screen();
	}

	virtual Window *createWindow() const {
		return new Window();
	}

	virtual Player *createPlayer() const {
		return new Player();
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

class Zapper : public zapper::Zapper {
public:
	explicit Zapper( const std::string &name ) : zapper::Zapper( name ) {}
	virtual ~Zapper() {}

protected:
	//	Instance creation
	virtual canvas::Factory *createCanvas() const {
		return new Factory();
	}

	pvr::dvb::Delegate *createTunerDelegate() const {
		return new DVBDelegate( (zapper::Zapper *)this);
	}

	util::key::type translateKey( util::key::type key ) {
		if (key == util::key::backspace) {
			return util::key::previous;
		}
		else if (key == util::key::nav) {
			return util::key::chlist;
		}
		return key;
	}
};

}	//	namespace aml

namespace zapper {

//	Instance creation
Zapper *Zapper::create( const std::string &name ) {
	//	Setup parameters
	util::cfg::setValue( "tuner.demuxer.psi.timeout", 7 );
	util::cfg::setValue( "tuner.demuxer.psi.filterTID", true );
	util::cfg::setValue( "tuner.provider.dvb.demux", 1 );
	util::cfg::setValue( "tuner.provider.dvb.buffer", 1600000 );
	util::cfg::setValue( "tuner.provider.dvb.genericPES", false );
	util::cfg::setValue( "gui.input.linux.enabled", true );

	return new aml::Zapper( name );
}

}	//	namespace zapper

