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

#include "../../src/show.h"
#include "../../src/channel.h"
#include "../../src/channels.h"
#include "../../src/player.h"
#include "../../src/tunerplayer.h"
#include "../../src/tuner.h"
#include "../../src/channels/persister.h"
#include "../../src/logos/service.h"
#include "../../src/logos/provider.h"
#include "../../src/parental/session.h"
#include "../../src/parental/control.h"
#include <util/settings/settings.h>

struct OkTestFilter : pvr::filter::Basic {
	OkTestFilter( int &calls, bool showMobile ) : pvr::filter::Basic(showMobile), _calls(calls) {}

	virtual bool filter( pvr::Channel * /*ch*/ ) const {
		_calls++;
		return true;
	}

	int &_calls;
};

struct FailTestFilter : pvr::filter::Basic {
	FailTestFilter( int &calls, bool showMobile ) : pvr::filter::Basic(showMobile), _calls(calls) {}

	virtual bool filter( pvr::Channel * /*ch*/ ) const {
		_calls++;
		return false;
	}

	int &_calls;
};

struct BasicTestFilter : pvr::filter::Basic {
	BasicTestFilter( int &calls, bool showMobile ) : pvr::filter::Basic(showMobile), _calls(calls) {}

	virtual bool filter( pvr::Channel *ch ) const {
		_calls++;
		return pvr::filter::Basic::filter( ch );
	}

	int &_calls;
};

class BasicControl : public pvr::parental::Control {
public:
	BasicControl( bool sex, bool violence, bool drugs, int age ) {
		_sexRestricted = sex;
		_violenceRestricted = violence;
		_drugsRestricted = drugs;
		_parentalAge = age;
	}
	virtual ~BasicControl() {}

	virtual bool hasSex() const { return _sexRestricted; }
	virtual bool hasViolence() const { return _violenceRestricted; }
	virtual bool hasDrugs() const { return _drugsRestricted; }
	virtual int age() const { return _parentalAge; }

private:
	bool _sexRestricted;
	bool _violenceRestricted;
	bool _drugsRestricted;
	int _parentalAge;

};

inline BasicControl *sexControl() {
	return new BasicControl(true, false, false, 0);
}

inline BasicControl *violenceControl() {
	return new BasicControl(false, true, false, 0);
}

inline BasicControl *drugsControl() {
	return new BasicControl(false, false, true, 0);
}

inline BasicControl *ageControl( int age ) {
	return new BasicControl(false, false, true, age);
}

inline BasicControl *allControl() {
	return new BasicControl(true, true, true, 16);
}

inline BasicControl *nothingControl() {
	return new BasicControl(false, false, false, 0);
}

class TunerPlayer : public pvr::TunerPlayer {
public:
	TunerPlayer() {}

	//	Player methods
	virtual void play( pvr::Channel * /*ch*/ ) {
		notifyStart( true );
	}
	virtual void stop( pvr::Channel * /*ch*/ ) {
		notifyStart( false );
	}

	virtual int nextVideo() { return 1; }
	virtual int videoCount() const { return 1; }

	virtual int nextAudio() { return 1; }
	virtual int audioCount() const { return 1; }

	virtual int nextSubtitle() { return -1; }
	virtual int subtitleCount() const { return 0; }

	//	Getters
	virtual const std::string url() const { return "url://pepe"; }
};

class BasicTuner : public pvr::Tuner {
public:
	BasicTuner() : pvr::Tuner("prueba") { _inScan = false; }

	virtual bool isScanning() const { return _inScan; }
	virtual void startScan() { _inScan = true; }
	virtual void cancelScan() { _inScan = false; onEndScan(); }

protected:
	virtual pvr::TunerPlayer *allocPlayer() const { return new TunerPlayer(); }
	virtual bool fetchStatus( int &signal, int &quality ) { signal=10; quality=10; return true; }

	bool _inScan;
};

class BasicTunerDelegate : public pvr::TunerDelegate {
public:
	explicit BasicTunerDelegate( pvr::logos::Service *l, pvr::Channels *chs ) : _logos(l), _channels(chs)
	{
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();

		_session = new pvr::parental::Session(_settings);
	}
	virtual ~BasicTunerDelegate() {
		delete _session;

		_settings->finalize();
		delete _settings;
	}

	//	Getters
	virtual pvr::logos::Service *logos() const { return _logos; }
	virtual pvr::Channels *channels() const { return _channels; }
	virtual pvr::parental::Session *session() const { return _session; }

private:
	util::Settings *_settings;
	pvr::logos::Service *_logos;
	pvr::Channels *_channels;
	pvr::parental::Session *_session;
};

class CountPersister : public pvr::channels::Persister {
public:
	CountPersister( int &count ) : _count(count) {}

	virtual bool save( const pvr::ChannelList & /*l*/ ) {
		_count++;
		return true;
	}

	virtual bool load( pvr::ChannelList & /*l*/ ) {
		return true;
	}

	int &_count;
};

class PersistenceChannels : public pvr::Channels {
public:
	PersistenceChannels( int &save, pvr::parental::Session *session ) : pvr::Channels(new CountPersister(save), session) {}
	virtual ~PersistenceChannels() {};
};

class BasicTunerDelegateImpl : public pvr::TunerDelegate {
public:
	BasicTunerDelegateImpl() : _networks(0), _session(NULL)
	{
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();

		_session = new pvr::parental::Session(_settings);
		_channels = new pvr::Channels( new pvr::channels::Persister(), _session );
	}

	virtual ~BasicTunerDelegateImpl() {
		delete _channels;
		delete _session;

		_settings->finalize();
		delete _settings;
	}

	//	Getters
	virtual pvr::logos::Service *logos() const { return (pvr::logos::Service *)&_logos; }
	virtual pvr::Channels *channels() const { return _channels; }
	virtual pvr::parental::Session *session() const { return _session; }

	//	Scan notifications
	virtual void onStartNetworkScan( const std::string & /*tuner*/, const std::string & /*net*/ ) { _networks++; }

	int _networks;

private:
	util::Settings *_settings;
	pvr::parental::Session *_session;
	pvr::logos::Service _logos;
	pvr::Channels *_channels;
};

class TunerInitFail : public BasicTuner {
protected:
	virtual bool init() { return false; }
};

class TunerStatusFail : public BasicTuner {
protected:
	virtual bool fetchStatus( int & /*signal*/, int & /*quality*/ ) { return false; }
};

class TunerCantReserve : public BasicTuner {
protected:
	virtual pvr::TunerPlayer *allocPlayer() const { return NULL; }
};

class PlayerImpl: public pvr::Player {
public:
	PlayerImpl( pvr::Channels *chs, util::Settings *settings, pvr::parental::Session *session ) : pvr::Player(0), _settings(settings), _chs(chs), _session(session) {}
	virtual ~PlayerImpl() {}

protected:
	virtual pvr::Channels *channels() const {
		return _chs;
	}

	virtual util::Settings *settings() const {
		return _settings;
	}

	virtual pvr::parental::Session *session() const {
		return _session;
	}

private:
	util::Settings *_settings;
	pvr::Channels *_chs;
	pvr::parental::Session *_session;
};

class BasicLogosProvider : public pvr::logos::Provider {
public:
	BasicLogosProvider() : pvr::logos::Provider( "dummy", "1.0" )
	{
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();
	}

	virtual ~BasicLogosProvider() {
		_settings->finalize();
		delete _settings;
	}

	virtual bool find( pvr::Channel * /*ch*/, std::string &logoFile, int &priority ) {
		logoFile = "pepe";
		priority = 0;
		return true;
	}

	virtual util::Settings *settings() const {
		return _settings;
	}

protected:
	virtual bool defaultState() const {
		return true;
	}

	util::Settings *_settings;
};

class NotFoundLogosProvider : public BasicLogosProvider {
public:
	virtual bool find( pvr::Channel * /*ch*/, std::string & /*logoFile*/, int & /*priority*/ ) {
		return false;
	}
};

class ParamLogosProvider : public BasicLogosProvider {
public:
	ParamLogosProvider( const std::string &result, int priority ) : _result(result), _priority(priority) {}
	virtual ~ParamLogosProvider() {}

	virtual bool find( pvr::Channel * /*ch*/, std::string &logoFile, int &priority ) {
		logoFile = _result;
		priority = _priority;
		return true;
	}

	std::string _result;
	int _priority;
};
