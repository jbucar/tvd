/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "serviceprovider.h"
#include "service.h"
#include "extension/extension.h"
#include "../provider/provider.h"
#include "../resourcemanager.h"
#include "../demuxer/ts.h"
#include "../demuxer/psi/psi.h"
#include "../demuxer/psi/patdemuxer.h"
#include "../demuxer/psi/pmtdemuxer.h"
#include "../demuxer/psi/sdtdemuxer.h"
#include "../demuxer/psi/nitdemuxer.h"
#include "../demuxer/psi/dsmcc/module/memorydata.h"
#include "../demuxer/pes/pesdemuxer.h"
#include <util/assert.h>
#include <util/log.h>
#include <set>

#define READY_STATE (sps::pat | sps::nit | sps::sdt)
#define IS_READY(mask) (mask == READY_STATE)

namespace tuner {

//	Service Provider State
namespace sps {

enum type {
       pat=0x01,
       nit=0x02,
       sdt=0x04
};

}

ServiceProvider::ServiceProvider( Provider *provider, ResourceManager *resMgr )
	: ServiceManager( resMgr )
{
	DTV_ASSERT(provider);

	_readyMask     = 0;
	_provider      = provider;
	_tsID          = INVALID_TS_ID;
	_nitID         = NIT_ID_RESERVED;
	_nitPID        = TS_PID_NULL;

	//	Initialize pool of memory data
	tuner::dsmcc::MemoryData::initialize( resMgr->memoryblocks(), 4096 );
}

ServiceProvider::~ServiceProvider()
{
	DTV_ASSERT(_readyMask == 0);

	//	Cleanup pool of memory data
	tuner::dsmcc::MemoryData::cleanup();
}

void ServiceProvider::start( bool isOnline/*=true*/ ) {
	LDEBUG("ServiceProvider", "Start: online=%d", isOnline);

	//	Start extensions
	ServiceManager::start( isOnline );

	//	Checks
	DTV_ASSERT(_tsID == INVALID_TS_ID);

	//	Filter the PAT
	startFilter(
		createDemuxer<PATDemuxer,Pat>(
			TS_PID_PAT,
			boost::bind(&ServiceProvider::onPat,this,_1),
			boost::bind(&ServiceProvider::onPatExpired,this),
			boost::bind(&ServiceProvider::onPatTimeout,this)
		)
	);
}

void ServiceProvider::stop() {
	if (isRunning()) {
		LDEBUG("ServiceProvider", "Stop");

		//	Kill services
		killServices();

		//	Stop PAT filter
		stopFilter( TS_PID_PAT );

		//	Stop extensions
		ServiceManager::stop();
	}
}

//	Filters
bool ServiceProvider::startFilter( PSIDemuxer *demux ) {
	return _provider->startFilter( demux );
}

bool ServiceProvider::startFilter( pes::FilterParams *params ) {
	return _provider->startFilter( params );
}

void ServiceProvider::stopFilter( ID pid ) {
	_provider->stopFilter( pid );
}

//	Getters
util::task::Dispatcher *ServiceProvider::dispatcher() {
	return _provider->dispatcher();
}

ID ServiceProvider::currentTS() const {
	return _tsID;
}

ID ServiceProvider::currentNit() const {
	return _nitID;
}

//	Services
void ServiceProvider::killServices() {
	LDEBUG("ServiceProvider", "Kill services");

	//	Update state ...
	setFlags( READY_STATE, false );

	{	//	Delete all services ...
		Services &sers = services();
		BOOST_FOREACH( Service *srv, sers ) {
			//	Stop filter
			stopFilter( srv->pid() );

			delete srv;
		}
		sers.clear();
	}

 	//	If NIT was available, stop filter
	stopNit();

	//	Stop SDT
	stopFilter( TS_PID_SDT );

	_tsID  = INVALID_TS_ID;
}

void ServiceProvider::setFlags( DWORD mask, bool set ) {
	//	Backup old state
	bool wasReady = isReady();

	LDEBUG("ServiceProvider", "Set flags begin: mask=%08x, state=%08x, set=%d, wasReady=%d", mask, _readyMask, set, wasReady);

	//	Set/Clean mask
	if (set) {
		_readyMask |= mask;
	}
	else {
		_readyMask &= ~mask;
	}

	//	State changed ...
	if (wasReady != IS_READY(_readyMask)) {
		if (wasReady) {
			//	Expire all services
			expire();
		}
		else {
			//	Set ready
			setReady();
		}
	}

	LDEBUG("ServiceProvider", "Set flags end: mask=%08x, state=%08x, set=%d, wasReady=%d, isReady=%d",
		mask, _readyMask, set, wasReady, isReady());
}

//	Tables callback
void ServiceProvider::onPat( const boost::shared_ptr<Pat> &pat ) {
	LDEBUG("ServiceProvider", "On PAT begin");

	pat->show();

	//	Setup TS ID
	DTV_ASSERT( _nitID == NIT_ID_RESERVED );
	DTV_ASSERT( _tsID == INVALID_TS_ID );
	_tsID = pat->tsID();

	//  Loop into PAT programs, create the services and filter each PMT
	Service *srv;
	Pat::Programs &programs = pat->programs();
	std::set<ID> pids;
	BOOST_FOREACH( const Pat::ProgramInfo &prog, programs ) {
		{	//	Create service
			ServiceID id(_nitID,(ID)_tsID,prog.program);
			srv = new Service( _provider->currentNetwork(), id, prog.pid );
		}

		//	Add service to list
		addService( srv );

		//	Only add pid if not present ...
		pids.insert( prog.pid );
	}

	//	set tsID
	setFlags( sps::pat, true );

	//	Start PMTs filters
	BOOST_FOREACH( ID pid, pids ) {
		//	Filter pmt for each service
		startFilter( createDemuxer<PMTDemuxer,Pmt>(
				pid,
				boost::bind(&ServiceProvider::onPmt,this,_1,pid),
				boost::bind(&ServiceProvider::onPmtExpired,this,pid),
				boost::bind(&ServiceProvider::onPmtTimeout,this,pid))
		);
	}

	{	//  Filter NIT (hack if not available)
		if (pat->nitPid() != TS_PID_NULL) {
			_nitPID = pat->nitPid();
		}
		else {
			_nitPID = TS_PID_NIT;
		}

		startFilter( createDemuxer<NITDemuxer,Nit>(
				_nitPID,
				boost::bind(&ServiceProvider::onNit,this,_1),
				boost::bind(&ServiceProvider::onExpired,this,sps::nit,service::desc::nit),
				boost::bind(&ServiceProvider::onTimeout,this,sps::nit))
		);
	}

	//	Filter SDT if available
	startFilter( createDemuxer<SDTDemuxer,Sdt>(
			TS_PID_SDT,
			boost::bind(&ServiceProvider::onSdt,this,_1),
			boost::bind(&ServiceProvider::onExpired,this,sps::sdt,service::desc::sdt),
			boost::bind(&ServiceProvider::onTimeout,this,sps::sdt))
	);

	LDEBUG("ServiceProvider", "On PAT end");
}

void ServiceProvider::onPatExpired() {
	LDEBUG("ServiceProvider", "PAT Expired");
	//	Kill all services
	killServices();
}

void ServiceProvider::onPatTimeout() {
	if (!(_readyMask & sps::pat)) {
		LDEBUG("ServiceProvider", "PAT timeout");

		//	Force all scanned ...
		notifyEndScan( true );

		//	Kill all services
		killServices();
	}
}

void ServiceProvider::onNit( const boost::shared_ptr<Nit> &nit ) {
	LDEBUG("ServiceProvider", "On NIT begin");

	if (nit->actual()) {
		//	Update NIT
		_nitID = nit->networkID();

		//	Check consistency
		ID oNit = nit->getNetwork( currentTS() );
		if (oNit != NIT_ID_RESERVED && oNit != nit->networkID()) {
			LWARN("ServiceProvider", "NIT with errors: oNit=%04x, network tsID=%04x, using=%4x", oNit, _nitID, oNit);
			_nitID = oNit;
		}

		//	Process NIT
		updateServices<Nit>( nit );

		//	Update status
		setFlags( sps::nit, true );
	}
	else {
		//	NIT is for other network or ts, ignore them
		nit->show();
	}

	LDEBUG("ServiceProvider", "On NIT end");
}

void ServiceProvider::stopNit() {
	if (_nitPID != TS_PID_NULL) {
		//	Stop NIT
		stopFilter( _nitPID );

		//	Initialize NIT state
		_nitID  = NIT_ID_RESERVED;
		_nitPID = TS_PID_NULL;
	}
}

void ServiceProvider::onSdt( const boost::shared_ptr<Sdt> &sdt ) {
	LDEBUG("ServiceProvider", "On SDT begin");

	if (sdt->actual() && sdt->tsID() == currentTS()) {
		updateServices<Sdt>( sdt );

		//	Mark SDT ready!
		setFlags( sps::sdt, true );
	}
	else {
		//	SDT is for other network or ts, ignore them
		sdt->show();
	}

	LDEBUG("ServiceProvider", "On SDT end");
}

void ServiceProvider::onTimeout( DWORD mask ) {
	if (!(_readyMask & mask)) {
		LDEBUG("ServiceProvider", "Timeout: nit=%d, sdt=%d",
			(mask & sps::nit) ? 1 : 0, (mask & sps::sdt) ? 1 : 0);

		setFlags( mask, true );

		if (isReady()) {
			//	Finish scan ...
			notifyEndScan( true );
		}
	}
}

void ServiceProvider::onExpired( DWORD mask, service::desc::type tableIndex ) {
	LDEBUG("ServiceProvider", "Expired: mask=%08x", mask);
	BOOST_FOREACH( Service *srv, services() ) {
		srv->expire( tableIndex );
	}
	setFlags( mask, false );
}

void ServiceProvider::onPmt( const boost::shared_ptr<Pmt> &pmt, ID pid ) {
	pmt->show();

	//	Check if there are a service in this pid
	Service *srv = findServiceByPID( pid );
	if (!srv) {
		LWARN( "ServiceProvider", "Cannot find service: pid=%04x", pid );
		return;
	}

	//	Check service have the same programID
	if (srv->id() != pmt->programID()) {
		LWARN( "ServiceProvider", "PMT is for another service: pid=%04x, srv=%04x, pmt=%04x",
			pid, srv->id(), pmt->programID() );
		return;
	}

	//	Update data
	updateService( srv, pmt );

	//	Notify end scan
	notifyEndScan();
}

void ServiceProvider::onPmtExpired( ID pid ) {
	Service *srv = findServiceByPID( pid );
	if (srv) {
		expireService( srv );
		srv->expire( service::desc::pmt );
	}
	else {
		LWARN( "ServiceProvider", "Cannot find service: pid=%04x", pid );
	}
}

void ServiceProvider::onPmtTimeout( ID pid ) {
	//	Find if pid changed is from a service (for PMT changes)
	Service *srv = findServiceByPID( pid );
	if (srv && srv->state() <= service::state::present) {
		LDEBUG("ServiceProvider", "On PMT timeout: pid=%04x", pid);
		srv->state( service::state::timeout );
		notifyEndScan();
	}
}

}
