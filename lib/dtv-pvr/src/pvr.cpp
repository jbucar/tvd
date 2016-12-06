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

#include "pvr.h"
#include "channels.h"
#include "tuner.h"
#include "player.h"
#include "time.h"
#include "channels/settingspersister.h"
#include "channels/xmlpersister.h"
#include "parental/session.h"
#include "logos/service.h"
#include "logos/provider.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>

namespace pvr {

class PvrDelegate : public TunerDelegate {
public:
	explicit PvrDelegate( Pvr *pvr ) : _pvr(pvr) {}
	virtual ~PvrDelegate() {}

	//	Getters
	virtual logos::Service *logos() const {
		return _pvr->logos();
	}

	virtual Channels *channels() const {
		return _pvr->channels();
	}

	virtual parental::Session *session() const {
		return _pvr->session();
	}

	//	Scan notifications
	virtual void onStartNetworkScan( const std::string &tuner, const std::string &net ) {
		_pvr->onStartNetworkScan( tuner, net );
	}

	virtual void onEndScan( const std::string &tuner ) {
		_pvr->onEndScan( tuner );
	}

	//	Emergency
	virtual void onEmergency( bool state, int ch ) {
		_pvr->onEmergency( state, ch );
	}

private:
	Pvr *_pvr;
};


Pvr::Pvr( util::Settings *settings, const std::string &file /* ="" */ )
	: _settings(settings)
{
	DTV_ASSERT(settings);

	_tunerDelegate = new PvrDelegate(this);
	_logos = new logos::Service();
	_session = new parental::Session(settings);

	{	//	Setup channels
		channels::Persister *persister;
		if (file.empty()) {
			persister = new channels::settings::Persister(settings);
		}
		else {
			persister = new channels::xml::Persister(file);
		}
		_channels = new Channels( persister, _session );
	}

	_initialized = false;
	_tunersInScan = 0;
	_defaultPlayer = NULL;
}

Pvr::~Pvr()
{
	delete _tunerDelegate;
	delete _logos;
	delete _channels;
	delete _session;
	DTV_ASSERT(_tuners.empty());
	DTV_ASSERT(_players.empty());
	DTV_ASSERT(!_initialized);
}

bool Pvr::initialize() {
	DTV_ASSERT(!_initialized);
	DTV_ASSERT(!_tunersInScan);

	LDEBUG( "pvr", "Initialize" );

	//	Check for time source initialized!
	if (!time::isInitialized()) {
		LERROR( "pvr", "Time source invalid. Please call to time::init" );
		return false;
	}

	//	Check for tuners
	if (_tuners.empty()) {
		LWARN( "pvr", "No tuner present!" );
	}

	//	Load channels DB
	bool result = _channels->load( boost::bind(&Pvr::tuner,this,_1) );
	if (!result) {
		LERROR( "pvr", "Cannot load channel database" );
		return false;
	}

	//	Initialize tuners
	BOOST_FOREACH( Tuner *tuner, _tuners ) {
		if (!tuner->initialize( _tunerDelegate )) {
			LERROR( "pvr", "Cannot initialize tuner: name=%s", tuner->name().c_str() );
			CLEAN_ALL( Tuner *, _tuners );
			return false;
		}
	}

	//	Start logos service after tuner, so each tuner can add more providers
	_logos->start();

	_initialized = true;

	return true;
}

void Pvr::finalize() {
	if (_initialized) {
		LDEBUG( "pvr", "Finalize" );

		if (!_players.empty()) {
			LWARN( "pvr", "There are some player not released" );
			BOOST_FOREACH( Player *player, _players ) {
				player->stop();
			}
		}

		//	Finalize all tuners
		BOOST_FOREACH( Tuner *tuner, _tuners ) {
			tuner->finalize();
		}

		//	Stop logos service
		_logos->stop();

		//	Reset channel DB
		_channels->reset();

		_initialized = false;

		DTV_ASSERT(!_tunersInScan);
	}

	//	Always destroy tuners and players
	CLEAN_ALL( Player *, _players );
	CLEAN_ALL( Tuner *, _tuners );
}

void Pvr::resetConfig() {
	DTV_ASSERT(_initialized);

	if (isScanning()) {
		cancelScan();
	}

	BOOST_FOREACH( Player *p, _players ) {
		p->resetConfig();
	}
	session()->resetConfig();
	channels()->removeAll();
	logos()->resetConfig();
}

void Pvr::addTuner( Tuner *tuner ) {
	DTV_ASSERT(tuner);
	DTV_ASSERT(!_initialized);
	LDEBUG( "pvr", "Add tuner: name=%s", tuner->name().c_str() );
	_tuners.push_back( tuner );
}

void Pvr::onEmergency( bool state, int chID ) {
	if (state) {
		if (!_defaultPlayer) {
			LWARN( "pvr", "In Emergency, but no default player available" );
			return;
		}

		if (chID >= 0) {
			//	Restart channel if is the current
			if (defaultPlayer()->current() == chID) {
				defaultPlayer()->change( -1 );
			}

			//	Play channel
			defaultPlayer()->change( chID );
		}
	}
}

//	Scan
bool Pvr::isScanning() const {
	return _tunersInScan > 0;
}

void Pvr::startScan() {
	DTV_ASSERT(_initialized);

	if (!_tunersInScan) {
		LDEBUG( "pvr", "Start scan" );

		//	Remove all channels
		_channels->beginTransaction();
		_channels->removeAll();

		//	Notify
		_tunersInScan = _tuners.size();
		_onScanChanged( scan::begin, Pvr::ScanNetwork() );

		//	Start
		BOOST_FOREACH( Tuner *t, _tuners ) {
			t->startScan();
		}

		//	Notify if no tuners
		if (!_tunersInScan) {
			_onScanChanged( scan::end, Pvr::ScanNetwork() );
		}
	}
}

void Pvr::cancelScan() {
	DTV_ASSERT(_initialized);

	if (_tunersInScan > 0) {
		LDEBUG( "pvr", "Cancel scan" );
		BOOST_FOREACH( Tuner *t, _tuners ) {
			t->cancelScan();
		}
	}
}

Pvr::ScanSignal &Pvr::onScanChanged() {
	return _onScanChanged;
}

void Pvr::onStartNetworkScan( const std::string &tuner, const std::string &net ) {
	LDEBUG( "pvr", "Start network scan: tuner=%s, network=%s", tuner.c_str(), net.c_str() );
	_onScanChanged( scan::network, Pvr::ScanNetwork(net) );
}

void Pvr::onEndScan( const std::string &tuner ) {
	DTV_ASSERT(_tunersInScan > 0);
	LDEBUG( "pvr", "End scan: tuner=%s", tuner.c_str() );

	_tunersInScan--;
	if (!_tunersInScan) {
		_channels->endTransaction();
		_onScanChanged( scan::end, Pvr::ScanNetwork() );
	}
}

size_t Pvr::networks() const {
	size_t result=0;
	BOOST_FOREACH( Tuner *t, _tuners ) {
		result += t->networks();
	}
	return result;
}

//	Players
Player *Pvr::allocPlayer( bool useDefault/*=true*/ ) {
	DTV_ASSERT(_initialized);
	size_t s = _players.size();
	Player *p = new PlayerImpl<Pvr>(++s,this);
	_players.push_back( p );
	if (useDefault) {
		_defaultPlayer = p;
	}
	return p;
}

void Pvr::freePlayer( Pvr::PlayerPtr &p ) {
	DTV_ASSERT(_initialized);
	Players::iterator it = std::find(
		_players.begin(),
		_players.end(),
		p
	);
	if (it != _players.end()) {
		if (p == _defaultPlayer) {
			_defaultPlayer = NULL;
		}
		delete p;
		p = NULL;
		_players.erase(it);
	}
}

Player *Pvr::defaultPlayer() const {
	return _defaultPlayer;
}

//	Getters
Tuner *Pvr::tuner( const std::string &name ) const {
	Tuners::const_iterator it=std::find_if(
		_tuners.begin(),
		_tuners.end(),
		boost::bind(&Tuner::name,_1) == name
	);
	return (it != _tuners.end()) ? (*it) : NULL;
}

Channels *Pvr::channels() const {
	return _channels;
}

parental::Session *Pvr::session() const {
	return _session;
}

logos::Service *Pvr::logos() const {
	return _logos;
}

util::Settings *Pvr::settings() const {
	return _settings;
}

}

