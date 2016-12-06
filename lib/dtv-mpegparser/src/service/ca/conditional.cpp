#include "conditional.h"
#include "system.h"
#include "delegate.h"
#include "decrypter.h"
#include "../service.h"
#include "../../demuxer/descriptors/demuxers.h"
#include "../../demuxer/ts.h"
#include "../../demuxer/psi/catdemuxer.h"
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace ca {

REGISTER_INIT_CONFIG( tuner_ca ) {
	root().addNode( "ca" )
		.addValue( "enable", "Enable CA", false );
}

Conditional *Conditional::create( FilterDelegate *filter ) {
	//	Create Conditional Access if necesary
	bool enableCA = util::cfg::getValue<bool>("tuner.ca.enable");
	LINFO( "mpegparser", "Enable CA=%d", enableCA );
	if (enableCA) {
		return new ca::Conditional( filter );
	}
	else {
		delete filter;
		return NULL;
	}
}

Conditional::Conditional( FilterDelegate *filter )
	: _filter(filter), _started(false)
{
	DTV_ASSERT(filter);
}

Conditional::~Conditional()
{
	stop();
	DTV_ASSERT(_systems.empty());
	CLEAN_ALL( Delegate *, _delegates );
	if (!_decrypters.empty()) {
		LWARN( "ca", "Decrypters already in use" );
		CLEAN_ALL( Decrypter *, _decrypters );
	}
	delete _filter;
}

//	Setup
void Conditional::addDelegate( Delegate *d ) {
	DTV_ASSERT(d);
	_delegates.push_back(d);
}

//	API
Decrypter *Conditional::allocDecrypter( ID srvID ) {
	int priority = -1;
	Delegate *d = NULL;
	BOOST_FOREACH( Delegate *del, _delegates ) {
		int prio = del->canDecrypt( srvID );
		if (prio > priority) {
			priority = prio;
			d = del;
		}
	}
	Decrypter *tmp = NULL;
	if (d) {
		tmp = d->createDecrypter(srvID);
		DTV_ASSERT(tmp);
		_decrypters.push_back( tmp );
	}
	return tmp;
}

void Conditional::freeDecrypter( Decrypter *csa ) {
	Decrypters::iterator it=std::find(
		_decrypters.begin(),
		_decrypters.end(),
		csa );
	if (it != _decrypters.end()) {
		delete (*it);
		_decrypters.erase(it);
	}
}

void Conditional::getAvailables( std::vector<ID> &systems ) const {
	getAvailables( systems, ID(-1) );
}

void Conditional::getAvailables( std::vector<ID> &systems, ID srvID ) const {
	systems.clear();
	BOOST_FOREACH( System *sys, _systems ) {
		if (sys->isAvailable( srvID )) {
			systems.push_back( sys->systemID() );
		}
	}
}

void Conditional::start() {
	if (_started) {
		return;
	}

	LINFO("ca", "Start CAS" );

	//	Start CAT filter
	filter()->startFilter(
		createDemuxer<CATDemuxer, Cat>(
			TS_PID_CAT,
			boost::bind( &Conditional::onCat, this, _1 )
		)
	);

	//	Start delegates
	BOOST_FOREACH( Delegate *del, _delegates ) {
		del->onKeys( boost::bind(&Conditional::onKeys,this,_1,_2) );
		del->start();
	}

	_started = true;
}

void Conditional::stop() {
	if (!_started) {
		return;
	}

	LINFO("ca", "Stop CAS");

	//	Stop delegates
	BOOST_FOREACH( Delegate *del, _delegates ) {
		del->stop();
	}

	//	Stop CAT filter
	filter()->stopFilter( TS_PID_CAT );

	//	Clean cards
	clearSystems();

	_started = false;
}

void Conditional::onStart( Service *srv ) {
	//	Add ECM information
	const desc::Descriptors &descs = srv->descriptors(service::desc::pmt);
	updateECM( srv->id(), descs, true );

	//	Notify a service is started
	BOOST_FOREACH( Delegate *del, _delegates ) {
		del->onStart( srv );
	}
}

void Conditional::onStop( Service *srv ) {
	//	Notify a service stopped
	BOOST_FOREACH( Delegate *del, _delegates ) {
		del->onStop( srv );
	}

	//	Remove ECM information
	const desc::Descriptors &descs = srv->descriptors(service::desc::pmt);
	updateECM( srv->id(), descs, false );
}


//	ECM handling
void Conditional::updateECM( ID srvID, const desc::Descriptors &descs, bool start ) {
	//	Start ECM
	desc::ca::Descriptor desc;
	if (DESC_PARSE( descs, conditional_access, desc )) {
		System *card = getSystem( desc.systemID );
		if (card) {
			card->updateECM( srvID, desc, start );
		}
	}
}

//	Keys
void Conditional::onKeys( ID srvID, Keys *keys ) {
	LDEBUG( "ca", "On keys received: srvID=%04x", srvID );
	BOOST_FOREACH( Decrypter *d, _decrypters ) {
		d->updateKeys( srvID, *keys );
	}
}

//	Notifications
void Conditional::onCat( const boost::shared_ptr<Cat> &cat ) {
	cat->show();

	clearSystems();

	//	Update all systems
	BOOST_FOREACH( const desc::ca::Descriptor &desc, cat->systems() ) {
		System *sys = getSystem( desc.systemID );
		if (!sys) {
			_systems.push_back( new System( filter(), desc ) );
		}
	}

	onCatChanged();
}

void Conditional::onCatChanged() {
	//	Notify to delegates
	BOOST_FOREACH( Delegate *del, _delegates ) {
		del->onCatChanged();
	}
}

System *Conditional::getSystem( ID caID ) const {
	Systems::const_iterator it = std::find_if(
		_systems.begin(),
		_systems.end(),
		boost::bind(&System::systemID,_1) == caID
	);
	return (it != _systems.end()) ? (*it) : NULL;
}

void Conditional::clearSystems() {
	CLEAN_ALL( System *, _systems );
}

//	Getters
FilterDelegate *Conditional::filter() const {
	return _filter;
}

}
}

