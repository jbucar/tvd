/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementaci�n de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los t�rminos de la Licencia P�blica General Reducida GNU como es publicada por la
  Free Software Foundation, seg�n la versi�n 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANT�A; ni siquiera la garant�a impl�cita de COMERCIALIZACI�N o ADECUACI�N
  PARA ALG�N PROP�SITO PARTICULAR. Para m�s detalles, revise la Licencia P�blica
  General Reducida GNU.

    Ud. deber�a haber recibido una copia de la Licencia P�blica General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "delegate.h"
#include "captionviewer.h"
#include <mpegparser/service/extension/player/player.h>
#include <mpegparser/service/extension/player/delegate.h>
#include <mpegparser/service/extension/player/tsdelegate.h>
#include <mpegparser/service/extension/player/esplayer.h>
#include <mpegparser/muxer/ts/out/rtp.h>
#include <mpegparser/provider/provider.h>
#include <util/log.h>

namespace pvr {
namespace dvb {

Delegate::Delegate()
{
}

Delegate::~Delegate()
{
}

const std::string Delegate::spec() const {
	return "ABNT";
}

bool Delegate::withEPG() const {
	return true;
}

bool Delegate::withEWBS() const {
	return true;
}

bool Delegate::withCC() const {
	return false;
}

int Delegate::areaCode() const {
	return -1;
}

CaptionViewer *Delegate::createCaptionViewer( bool isCC ) const {
	CaptionViewer *view = NULL;
	CaptionPlayer *cp = createCaptionPlayer( isCC );
	if (cp) {
		view = new CaptionViewer( isCC, cp, dispatcher() );
	}
	return view;
}

CaptionPlayer *Delegate::createCaptionPlayer( bool /*isCC*/ ) const {
	return NULL;
}

tuner::Provider *Delegate::createProvider() const {
	tuner::Provider *provider=createProviderImpl();
	if (!provider) {
		return NULL;
	}
	provider->setDispatcher( dispatcher() );
	return provider;
}

tuner::Provider *Delegate::createProviderImpl() const {
	return tuner::Provider::create();
}

tuner::player::Player *Delegate::createPlayer() const {
	//	Create player delegate
	tuner::player::Delegate *dlg = createPlayerDelegate();

	//	Create and setup player
	tuner::player::Player *player = new tuner::player::Player( dlg );
	player->addDefaultsStreams();
	player->addPlayer( new tuner::player::es::Player(tuner::pes::type::video) );
	player->addPlayer( new tuner::player::es::Player(tuner::pes::type::audio) );

	return player;
}

tuner::player::Delegate *Delegate::createPlayerDelegate() const {
	return new tuner::player::AvDelegate();
}

tuner::player::Delegate *Delegate::createPlayerDelegate( const std::string &use ) const {
	if (use == "av") {
		return new tuner::player::AvDelegate();
	}
	else if (use == "decode") {
		return new tuner::player::Delegate();
	}
	else if (use == "ts") {
		tuner::muxer::ts::Output *out = new tuner::muxer::ts::RTPOutput( "127.0.0.1", 5000 );
		return new tuner::player::ts::Delegate(out);
	}

	return NULL;
}

}
}

