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
#include "tuner.h"
#include "scanextension.h"
#include "../provider/provider.h"
#include "../service/serviceprovider.h"
#include "../service/extension/extension.h"
#include "../provider/provider.h"
#include "../resourcemanager.h"
#include <util/task/dispatcher.h>
#include <util/assert.h>
#include <util/log.h>

namespace tuner {

Tuner::Tuner( Provider *provider, ResourceManager *resMgr )
{
	_scanIt = NULL;

	//	Setup provider
	DTV_ASSERT( provider );
	_provider = provider;
	_provider->dispatcher()->registerTarget( this, "tuner::Tuner" );
	_provider->onLockChanged( boost::bind(&Tuner::onLockChanged,this) );
	_provider->onLockTimeout( boost::bind(&Tuner::onLockTimeout,this) );

	//	Setup service manager
	_srvMgr = new ServiceProvider( provider, resMgr );
	attach( new ScanExtension( boost::bind(&Tuner::serviceReady,this,_1) ) );
}

Tuner::~Tuner()
{
	_provider->dispatcher()->unregisterTarget( this );
	DTV_ASSERT(!_scanIt);
	delete _provider;
	delete _srvMgr;
}

//	Setup
void Tuner::attach( Extension *ext ) {
	_srvMgr->attach( ext );
}

void Tuner::detach( Extension *ext ) {
	_srvMgr->detach( ext );
}

void Tuner::onServiceReady( const OnServiceReadyCallback &callback ) {
	_onServiceReady = callback;
}

//	Initialize/Finalize
bool Tuner::initialize() {
	if (!_provider->start()) {
		LERROR( "Tuner", "Cannot start provider" );
		return false;
	}
	_srvMgr->initialize();

	return true;

}

void Tuner::finalize() {
	_provider->stop();
	_srvMgr->finalize();
}

//	Start/Stop
void Tuner::start() {
}

void Tuner::stop() {
	//	Stop any service running
	stopAllServices();

	//	Stop service provider
	_srvMgr->stop();
}

//	Tune service
bool Tuner::startService( size_t networkID, ID srvID ) {
	DTV_ASSERT(!_scanIt);
	if (startNetwork( networkID )) {
		_srvMgr->startService( srvID );
		return true;
	}
	return false;
}

void Tuner::stopService( ID srvID ) {
	DTV_ASSERT(!_scanIt);
	_srvMgr->stopService( srvID );
}

void Tuner::stopAllServices() {
	DTV_ASSERT(!_scanIt);
	_srvMgr->stopServices();
}

//	Scan
bool Tuner::isScanning() const {
	return _scanIt ? true : false;
}

void Tuner::startScan( ScanIterator *it ) {
	DTV_ASSERT(it);

	if (!_scanIt) {
		LDEBUG( "Tuner", "Start scan begin" );

		//	Notify iter to begin scan
		it->onBeginScan( _provider->getNetworksCount() );
		it->current( -1 );
		_scanIt = it;

		//	Stop current network
		_provider->stopCurrent();

		//	Stop service provider
		_srvMgr->stop();

		//	Attach to end network scan callback
		_srvMgr->onEndScan( boost::bind(&Tuner::endNetScan,this) );

		//	Begin scan of networks
		nextScan();
	}
}

void Tuner::stopScan() {
	LDEBUG( "Tuner", "Stop scan" );
	if (_scanIt) {
		ScanIterator *tmp = _scanIt;
		_scanIt = NULL;

		_provider->stopCurrent();

		_srvMgr->stop();
		_srvMgr->onEndScan( NULL );

		tmp->onEndScan();
	}
}

void Tuner::status( status::Type &st ) const {
	return _provider->status( st );
}

size_t Tuner::currentNetwork() const {
	return _provider->currentNetwork();
}

std::string Tuner::currentNetworkName() const {
	return _provider->getNetworkName( _provider->currentNetwork() );
}

size_t Tuner::networks() const {
	return _provider->getNetworksCount();
}

//	Aux scan extension notification
void Tuner::serviceReady( Service *srv ) {
	if (!_onServiceReady.empty()) {
		_onServiceReady( srv, isScanning() );
	}
}

//	Aux tune/scan
void Tuner::onLockTimeout() {
	endNetScan();
}

void Tuner::onLockChanged() {
	if (_provider->isLocked() && !_srvMgr->isRunning()) {
		_srvMgr->start( _scanIt ? true : false );
	}
}

bool Tuner::startNetwork( size_t net ) {
	bool result=true;

	LDEBUG( "Tuner", "Start network: net=%d", net );
	if (net != _provider->currentNetwork()) {
		//	Stop service provider
		_srvMgr->stop();

		//	Try set new network
		result=_provider->setNetwork( net );
	}
	return result;
}

void Tuner::nextScan() {
	if (_scanIt) {
		size_t networkToScan = _scanIt->current()+1;
		LDEBUG( "Tuner", "Next network scan: network=%d", networkToScan );

		if (networkToScan < _provider->getNetworksCount()) {
			//	Notify network to scan
			_scanIt->current( networkToScan );
			_scanIt->onNetwork( _provider->getNetworkName(networkToScan) );

			//	Try scan
			if (!_provider->setNetwork( networkToScan )) {
				//	Enqueue task, so we can execute a cancelScan task
				_provider->dispatcher()->post( this, boost::bind(&Tuner::nextScan,this) );
			}
		}
		else {
			stopScan();
		}
	}
}

void Tuner::endNetScan() {
	if (_scanIt) {
		LDEBUG( "Tuner", "End network scan!" );

		//	Stop service scan
		_srvMgr->stop();

		//	Iterate to next network
		nextScan();
	}
}

}
