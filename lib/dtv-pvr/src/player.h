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

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <boost/signals2.hpp>
#include <string>

#define INVALID_CHANNEL -1

namespace util {
	class Settings;
}

namespace pvr {

class Channel;
class Channels;
class TunerPlayer;

namespace parental {
	class Session;
}

namespace filter {
	struct ChannelFilter {
		virtual bool filter( Channel *ch ) const=0;
	};
	struct Basic : public ChannelFilter {
		Basic( bool showMobile );
		virtual bool filter( Channel *ch ) const;
	private:
		bool _showMobile;
	};
	struct Favorite : public Basic {
		Favorite( bool showMobile );
		virtual bool filter( Channel *ch ) const;
	};
}

/**
 * Representa un reproductor de canales.
 */
class Player {
public:
	explicit Player( int id );
	virtual ~Player();

	//	Start/stop/config
	void start( bool loadOldChannel=true );
	void stop();
	void resetConfig();

	//	Change channel
	int change( int channelID );
	typedef boost::signals2::signal<void (int, Channel *)> ChangeChannelSignal;
	ChangeChannelSignal &onChangeChannel();
	typedef boost::signals2::signal<void (void)> ShowChangedSignal;
	ShowChangedSignal &onShowChanged();
	typedef boost::function<void (const std::string &url, bool started)> MediaChangedCallback;
	void onMediaChanged( const MediaChangedCallback &callback );

	//	Status
	int current() const;
	const std::string &currentNetworkName() const;
	void status( int &signal, int &quality );
	const std::string tunerName() const;

	//	Protected
	bool isProtected() const;
	typedef boost::signals2::signal<void (bool isprotected)> ChannelProtectedSignal;
	ChannelProtectedSignal &onChannelProtected();

	//	Change channel utility
	bool showMobile() const;
	void showMobile( bool show );
	int nextChannel( int first, int factor ) const;
	int nextFavorite( int first, int factor ) const;
	int next( int first, int factor, struct filter::ChannelFilter *filter ) const;

	//	Player methods
	int nextVideo();
	int videoCount() const;
	void videoInfo( info::Basic &info, int id=-1 ) const;

	int nextAudio();
	int audioCount() const;
	void audioInfo( info::Audio &info, int id=-1 ) const;

	int nextSubtitle();
	int subtitleCount() const;
	void subtitleInfo( info::Basic &info, int id=-1 ) const;

	int playVideoStream( int id, std::string &url );
	void stopVideoStream( int playerID );

protected:
	virtual Channels *channels() const=0;
	virtual util::Settings *settings() const=0;
	virtual parental::Session *session() const=0;

	//	Notifications
	void onChannelStarted( bool start );
	void onChannelRemoved( Channel *ch, bool allRemoved );
	void onChannelChanged( Channel *ch );
	void onSessionChanged( bool state );
	void onTimeValid( bool isValid );

	//	Aux
	void updateMedia();
	void notifyMediaChanged( bool start );
	bool checkProtected();
	bool changeProtected( bool isProtected );
	void saveCurrent();
	void stopCurrent( bool save=false, bool needReturnPlayer=true );

private:
	int _id;
	int _current;
	Channel *_ch;
	TunerPlayer *_mp;
	bool _showMobile;
	ChangeChannelSignal _onChange;
	ShowChangedSignal _onShowChanged;
	boost::signals2::connection _sChannelRemoved, _sChannelChanged, _sSessionChanged, _sTimeChanged;
	bool _channelStarted;
	bool _channelProtected;
	bool _mediaStarted;
	MediaChangedCallback _onMediaChanged;
	ChannelProtectedSignal _onProtected;
};

template<typename T>
class PlayerImpl : public Player {
public:
	PlayerImpl( int id, T *obj ) : Player(id), _obj(obj) {}

	virtual Channels *channels() const { return _obj->channels(); }
	virtual util::Settings *settings() const { return _obj->settings(); }
	virtual parental::Session *session() const { return _obj->session(); }

private:
	T *_obj;
};

}

