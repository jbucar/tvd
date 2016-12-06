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

#include "channels.h"
#include "channel.h"
#include "channels/persister.h"
#include "parental/session.h"
#include "generated/config.h"
#include <util/mcr.h>
#include <util/log.h>
#include <util/assert.h>
#include <stdlib.h>

namespace pvr {

Channels::Channels( channels::Persister *persister, parental::Session *session )
	: _persister(persister), _session(session)
{
	DTV_ASSERT(persister);
	DTV_ASSERT(session);
	_nextID = -1;
	_transaction = 0;

	_cEnabled = _session->onEnabledChanged().connect(boost::bind(&Channels::onSessionEnabledChanged, this, _1));
}

Channels::~Channels()
{
	_cEnabled.disconnect();
	CLEAN_ALL(Channel *,_channels);
	delete _persister;
}

//	Initialization
bool Channels::load( const Channels::FncLoad &fnc ) {

	//	Load from db
	if (_persister->load( _channels )) {
		for (size_t i=0; i<_channels.size(); i++) {
			Channel *ch = _channels[i];

			{	//	Attach to tuner
				Tuner *tuner = fnc( ch->tunerName().c_str() );
				if (tuner) {
					ch->tuner( tuner );
				}
			}

			//	Find nextID
			if (_nextID <= ch->channelID()) {
				_nextID = ch->channelID();
			}
		}
		return true;
	}
	return false;
}

void Channels::beginTransaction() {
	_transaction++;
}

void Channels::endTransaction() {
	if (_transaction > 0) {
		_transaction--;
	}
	save();
}

void Channels::save() {
	if (!_transaction) {
		LDEBUG( "pvr", "Save channels: size=%d", _channels.size() );
		if (!_persister->save( _channels )) {
			LWARN( "pvr", "Cannot save list of channels" );
		}
	}
}

void Channels::reset() {
	CLEAN_ALL(Channel *,_channels);
	_nextID = -1;
}

//	Update methods
int Channels::add( Channel *ch ) {
	if (isProtected()) {
		LWARN( "pvr", "Cannot add channel. Database is protected!" );
		delete ch;
		return -1;
	}

#if FILTER_MOBILE_CHANNELS
	if (ch->isMobile()) {
		LWARN( "pvr", "Cannot add channel. Channel is mobile!: name=%s", ch->name().c_str() );
		delete ch;
		return -1;
	}
#endif

	DTV_ASSERT(ch);
	_nextID++;
	LDEBUG( "pvr", "Add channel: id=%d, name=%s", _nextID, ch->name().c_str() );
	ch->channelID( _nextID );
	_channels.push_back( ch );
	save();
	_onNewChannel( ch );

	return _nextID;
}

bool Channels::remove( int channelID ) {
	bool result=false;
	if (!isProtected()) {
		ChannelList::iterator it=std::find_if(
			_channels.begin(), _channels.end(), boost::bind(&Channel::channelID,_1) == channelID );
		if (it != _channels.end()) {
			Channel *ch=(*it);
			_channels.erase( it );
			erase( ch, false );
			save();
			result=true;
		}
	}
	return result;
}

void Channels::erase( Channel *ch, bool removeAll ) {
	LDEBUG( "channel", "Remove channel: channelID=%d, name=%s",
		ch->channelID(), ch->name().c_str() );
	_onChannelRemoved( ch, removeAll );
	delete ch;
}

void Channels::removeAll() {
	if (!isProtected()) {
		for (size_t i=0; i<_channels.size(); i++) {
			erase( _channels[i], true );
		}
		_channels.clear();
		save();

		//	Reset nextID
		_nextID = -1;
	}
}

void Channels::onSessionEnabledChanged( bool isEnabled ) {
	if (!isEnabled) {
		beginTransaction();
		BOOST_FOREACH(Channel *ch, _channels) {
			ch->unblock();
		}
		endTransaction();
	}
}

bool Channels::isProtected() const {
	DTV_ASSERT(_session);
	bool isBlocked = _session->isBlocked();
	if (isBlocked) {
		LWARN("Channels", "Session is blocked, the action cannot be performed");
	}
	return isBlocked;
}

//	Notifications
Channels::NewChannelSignal &Channels::onNewChannel() {
	return _onNewChannel;
}

Channels::ChannelRemovedSignal &Channels::onChannelRemoved() {
	return _onChannelRemoved;
}

//	Getters
const ChannelList &Channels::getAll() const {
	return _channels;
}

Channel *Channels::get( int channelID ) const {
	Channel *ch=NULL;
	ChannelList::const_iterator it=std::find_if(
		_channels.begin(), _channels.end(), boost::bind(&Channel::channelID,_1) == channelID );
	if (it != _channels.end()) {
		ch=(*it);
	}
	return ch;
}

}
