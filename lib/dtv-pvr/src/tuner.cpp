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

#include "tuner.h"
#include "tunerplayer.h"
#include "logos/service.h"
#include "show.h"
#include "channel.h"
#include "channels.h"
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>

namespace pvr {

Tuner::Tuner( const std::string &name )
	: _name(name)
{
	_delegate = NULL;
	_defaultShow = NULL;
}

Tuner::~Tuner()
{
	delete _defaultShow;
	DTV_ASSERT(!_delegate);
	DTV_ASSERT(_players.empty());
	DTV_ASSERT(_freePlayers.empty());
}

//	Initialization
bool Tuner::initialize( TunerDelegate *delegate ) {
	DTV_ASSERT(delegate);
	LDEBUG( "Tuner", "Initialize" );
	_delegate = delegate;
	if (!init()) {
		_delegate = NULL;
		return false;
	}
	return true;
}

bool Tuner::init() {
	return true;
}

bool Tuner::isInitialized() const {
	return _delegate;
}

void Tuner::finalize() {
	if (isInitialized()) {
		LDEBUG( "Tuner", "Finalize" );

		if (isScanning()) {
			LWARN( "Tuner", "The tuner is scanning, cancel ..." );
			cancelScan();
		}

		if (_players.size() != _freePlayers.size()) {
			LWARN( "Tuner", "Some players not returned" );
		}

		BOOST_FOREACH( TunerPlayer *player, _players ) {
			player->finalize();
			delete player;
		}
		_players.clear();
		{	//	Free queue
			std::queue<TunerPlayer *> empty;
			std::swap( _freePlayers, empty );
		}

		fin();

		_delegate = NULL;
	}
}

void Tuner::fin() {
}

//	Scan
bool Tuner::isScanning() const {
	return false;
}

void Tuner::startScan() {
}

void Tuner::cancelScan() {
}

void Tuner::addChannel( Channel *ch ) {
	//	Setup channel
	ch->tuner( this );

	//	Add to db
	channels()->add( ch );
}

void Tuner::onStartNetworkScan( const std::string &net ) {
	delegate()->onStartNetworkScan( name(), net );
}

void Tuner::onEndScan() {
	delegate()->onEndScan( name() );
}

//	Emergency
void Tuner::notifyEmergency( bool state, int chID/*=-1*/ ) {
	delegate()->onEmergency( state, chID );
}

Channels *Tuner::channels() const {
	return delegate()->channels();
}

Show *Tuner::defaultShow() {
	if (!_defaultShow) {
		_defaultShow = createDefaultShow();
	}
	return _defaultShow;
}

Show *Tuner::createDefaultShow() const {
	return new Show();
}

//	Logos
logos::Service *Tuner::logos() const {
	return delegate()->logos();
}

const std::string Tuner::findLogo( Channel *ch ) const {
	if (isInitialized()) {
		return logos()->find( ch );
	}
	return std::string("");
}

//	Players
TunerPlayer *Tuner::defaultPlayer() {
	if (_players.empty()) {
		TunerPlayer *p = reservePlayer();
		DTV_ASSERT(p);
		freePlayer(p);
	}
	return _players[0];
}

TunerPlayer *Tuner::reservePlayer() {
	DTV_ASSERT( isInitialized() );

	TunerPlayer *player = NULL;
	if (_freePlayers.empty()) {
		int max = maxPlayers();
		if (max < 0 || _players.size() < (size_t)max) {
			//	Alloc a new player
			player = allocPlayer();
			if (!player || !player->initialize()) {
				LWARN( "tuner", "Cannot allocate/initialize player: player=%p", player );
				DEL(player);
			}
			else {
				_players.push_back( player );
			}
		}
	}
	else {
		player = _freePlayers.front();
		_freePlayers.pop();
	}

	return player;
}

void Tuner::freePlayer( TunerPlayer *p ) {
	DTV_ASSERT( isInitialized() );
	DTV_ASSERT(p);
	_freePlayers.push( p );
}

size_t Tuner::allocatedPlayers() const {
	return _players.size();
}

int Tuner::maxPlayers() const {
	return -1;
}

//	Status
size_t Tuner::networks() const {
	return 0;
}

void Tuner::status( int &signal, int &quality ) {
	if (!isInitialized() || !fetchStatus( signal, quality )) {
		signal = -1;
		quality = -1;
	}
}

bool Tuner::fetchStatus( int & /*signal*/, int & /*quality*/ ) {
	return false;
}

//	Aux
Channel *Tuner::findChannel( const std::string &uri ) const {
	DTV_ASSERT( isInitialized() );
	const ChannelList &chs = channels()->getAll();
	ChannelList::const_iterator it=std::find_if(
		chs.begin(),
		chs.end(),
		boost::bind(&Channel::uri,_1) == uri
	);
	return (it == chs.end()) ? NULL : (*it);
}

//	Getters
const std::string &Tuner::name() const {
	return _name;
}

parental::Session *Tuner::session() const {
	return delegate()->session();
}

TunerDelegate *Tuner::delegate() const {
	DTV_ASSERT(_delegate);
	return _delegate;
}

}

