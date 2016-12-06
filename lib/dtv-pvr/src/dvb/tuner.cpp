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
#include "delegate.h"
#include "tunerplayer.h"
#include "control.h"
#include "show.h"
#include "../channel.h"
#include "../channels.h"
#include "../time.h"
#include <mpegparser/service/extension/ewbs/extension.h>
#include <mpegparser/service/extension/epg/epgextension.h>
#include <mpegparser/service/serviceprovider.h>
#include <mpegparser/service/extension/extension.h>
#include <mpegparser/demuxer/psi/eit.h>
#include <mpegparser/demuxer/descriptors/lcn.h>
#include <mpegparser/provider/provider.h>
#include <mpegparser/tuner/tuner.h>
#include <util/url.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>

namespace pvr {
namespace dvb {

namespace bpt=boost::posix_time;

namespace id {

std::string toString( const ChannelID &id ) {
	//	Make custom channelID
	util::Url url;
	url.type( "dvb" );
	url.addParam( "networkID", id.networkID );
	url.addParam( "nitID", id.nitID );
	url.addParam( "tsID", id.tsID );
	url.addParam( "srvID", id.srvID );
	return url.id();
}

std::string toString( size_t netID, tuner::ID nitID, tuner::ID tsID, tuner::ID srvID ) {
	//	Make custom channelID
	util::Url url;
	url.type( "dvb" );
	url.addParam( "networkID", netID );
	url.addParam( "nitID", nitID );
	url.addParam( "tsID", tsID );
	url.addParam( "srvID", srvID );
	return url.id();
}

std::string toString( size_t netID, const tuner::ServiceID &srvID ) {
	return toString( netID, srvID.nitID, srvID.tsID, srvID.programID );
}

bool fromString( const std::string &str, ChannelID &id ) {
	util::Url url(str);
	if (url.type() != "dvb") {
		return false;
	}
	if (!url.getParam( "networkID", id.networkID )) {
		return false;
	}
	if (!url.getParam( "nitID", id.nitID )) {
		return false;
	}
	if (!url.getParam( "tsID", id.tsID )) {
		return false;
	}
	if (!url.getParam( "srvID", id.srvID )) {
		return false;
	}
	return true;
}

}	//	namespace id


//	Scan iterator implementation
class ScanIterator : public tuner::ScanIterator {
public:
	ScanIterator( Tuner *tuner ) : _tuner(tuner) {}
	virtual ~ScanIterator() {}

	virtual void onNetwork( const std::string &name ) {
		_tuner->onStartNetworkScan( name );
	}

	virtual void onEndScan() {
		_tuner->onEndScan();
	}

	void onServiceReady( tuner::Service *srv, bool inScan ) {
		_tuner->onServiceReady( srv, inScan );
	}

private:
	Tuner *_tuner;
};

Tuner::Tuner( Delegate *delegate )
	: pvr::Tuner( "dvb" ), _delegate(delegate)
{
	DTV_ASSERT(delegate);

	{	//	Create tuner
		tuner::Provider *provider = delegate->createProvider();
		DTV_ASSERT(provider);

		tuner::ResourceManager *resMgr = delegate->createResources();
		DTV_ASSERT(resMgr);

		_tuner = new tuner::Tuner( provider, resMgr );
	}

	//	Setup listener
	_scanIter = new ScanIterator(this);
	_tuner->onServiceReady( boost::bind(&ScanIterator::onServiceReady,_scanIter,_1,_2) );

	_ewbs = NULL;
}

Tuner::~Tuner()
{
	delete _scanIter;
	delete _tuner;
}

bool Tuner::init() {
	if (_tuner->initialize()) {
		//	Add EPG extension if necesary
		if (_delegate->withEPG()) {
			//	Create EPG extension
			tuner::EPGExtension *extension = new tuner::EPGExtension();
			extension->onEitParsed( boost::bind( &Tuner::onEit, this, _1 ) );

			//	Attach extension to tuner
			_tuner->attach( extension );
		}

		//	Add EWBS extension if necesary
		if (_delegate->withEWBS()) {
			_ewbs  = new tuner::ewbs::Extension( _delegate->areaCode() );
			_ewbs->onEmergency( boost::bind(&Tuner::onEmergency,this,_1) );
			_tuner->attach( _ewbs );
		}

		//	Start tuner
		_tuner->start();

		return true;
	}
	return false;
}

void Tuner::fin() {
	_ewbs = NULL;
	_tuner->stop();
	_tuner->finalize();
}

//	Scan
bool Tuner::isScanning() const {
	return _tuner->isScanning();
}

void Tuner::startScan() {
	_tuner->startScan( _scanIter );
}

void Tuner::cancelScan() {
	_tuner->stopScan();
}

void Tuner::onServiceReady( tuner::Service *srv, bool inScan ) {
	std::string id = id::toString(srv->networkID(), srv->serviceID());

	if (inScan) {
		//	Create new channel from service
		Channel *ch = new Channel(
			srv->name(),
			tuner::desc::lcn::get( srv ),
			_tuner->currentNetworkName()
		);

		//	Setup uri
		ch->uri( id );

		//	Setup parental control
		ch->parentalControl( Control::parse( srv->descriptors(tuner::service::desc::pmt) ) );

		//	Setup mobile
		if (srv->type() == tuner::service::type::one_seg) {
			ch->mobile( true );
		}

		//	Setup if channel is free
		ch->isFree( srv->isScrambled() ? false : true );

		addChannel( ch );
	}
	else {
		Channel *ch = findChannel( id );
		if (ch) {
			//	Update parental control
			ch->parentalControl( Control::parse( srv->descriptors(tuner::service::desc::pmt) ) );
		}
	}
}

//	Status
size_t Tuner::networks() const {
	return _tuner->networks();
}

void Tuner::status( int &signal, int &quality ) {
	tuner::status::Type st;
	_tuner->status( st );
	signal = st.signal;
	quality = st.snr;
}

//	Players
pvr::TunerPlayer *Tuner::allocPlayer() const {
	return new TunerPlayer( (Tuner *)this );
}

//	EWBS
bool Tuner::inEmergency( tuner::ID srvID ) const {
	if (!_ewbs) {
		return false;
	}
	return _ewbs->inEmergency( srvID );
}

Channel *Tuner::findEmergencyChannel( tuner::ID srvID ) {
	DTV_ASSERT( isInitialized() );
	const ChannelList &chs = channels()->getAll();
	BOOST_FOREACH( Channel *ch, chs ) {
		id::ChannelID id;
		if (!id::fromString( ch->uri(), id )) {
			LWARN( "dvb", "Play: Cannot parse customID" );
			continue;
		}

		if (id.srvID == srvID) {
			return ch;
		}
	}
	return NULL;
}

void Tuner::onEmergency( bool state ) {
	if (state) {
		DTV_ASSERT(_ewbs);
		const tuner::ewbs::EmergencyInfo &info = _ewbs->getAll();
		BOOST_FOREACH( const tuner::ewbs::EmergencyInfoService &eSrv, info ) {
			//	Get channel
			Channel *ch = findEmergencyChannel( eSrv.srvID );
			if (!ch) {
				LWARN( "dvb", "Invalid emergency channel" );
				continue;
			}

			//	Save and notify
			notifyEmergency( state, ch->channelID() );
			return;
		}
	}
	else {
		notifyEmergency( state );
	}
}

//	EPG
pvr::Show *Tuner::createDefaultShow() const {
	return new Show();
}

void Tuner::onEit( const boost::shared_ptr<tuner::Eit> &eit ) {
	Channel *ch = findChannel( id::toString(_tuner->currentNetwork(), eit->nitID(), eit->tsID(), eit->serviceID()) );
	if (ch) {
		const tuner::eit::Events &events = eit->events();
		BOOST_FOREACH(tuner::eit::Event *evt, events) {
			ch->addShow( new Show(evt,_delegate->spec()) );
		}
	}
}

//	Implementation
void Tuner::attach( tuner::Extension *ext ) {
	DTV_ASSERT(ext);
	LDEBUG( "dvb", "Attach extension: %p", ext );
	_tuner->attach( ext );
}

void Tuner::detach( tuner::Extension *ext ) {
	DTV_ASSERT(ext);
	LDEBUG( "dvb", "Detach extension: %p", ext );
	_tuner->detach( ext );
}

bool Tuner::startService( size_t networkID, tuner::ID srvID ) {
	return _tuner->startService( networkID, srvID );
}

void Tuner::stopService( tuner::ID srvID ) {
	_tuner->stopService( srvID );
}

//	Getter
Delegate *Tuner::delegate() const {
	return _delegate;
}

}
}

