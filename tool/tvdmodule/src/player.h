/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-module implementation.

    DTV-module is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-module is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-module.

    DTV-module es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-module se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <node/util.h>
#include <boost/signals2.hpp>

namespace pvr {
	class Player;
	class Channel;
}

namespace tvd {

class MediaPlayer;

class Player : public node::ObjectWrap {
public:
	Player( pvr::Player *ch, MediaPlayer *mp );
	virtual ~Player();

	static void Init();

	//	Status
	DECL_METHOD(current);
	DECL_METHOD(isProtected);
	DECL_METHOD(currentNetworkName);
	DECL_METHOD(status);

	//	Change channel
	DECL_METHOD(change);
	DECL_METHOD(showMobile);
	DECL_METHOD(nextChannel);
	DECL_METHOD(nextFavorite);

	//	Player methods
	DECL_METHOD(nextVideo);
	DECL_METHOD(videoInfo);

	DECL_METHOD(nextAudio);
	DECL_METHOD(audioInfo);

	DECL_METHOD(nextSubtitle);
	DECL_METHOD(subtitleInfo);

	pvr::Player *player() const;
	pvr::Player *stop();
	MediaPlayer *media() const;

protected:
	static pvr::Player *player(const v8::FunctionCallbackInfo<v8::Value> &args);

	void onProtectedChanged( bool isProtected );
	void onCurrentShowChanged();
	void onChannelChanged( int, pvr::Channel *ch );
	void onMedia( const std::string &url, bool started );
	void onClosedCaption( const std::string &text );

private:
	pvr::Player *_player;
	MediaPlayer *_media;
	boost::signals2::connection _cProtected, _cChanged, _cShowChanged;
	static v8::Persistent<v8::Function> _constructor;
};

}

