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

#include "dsmccfilter.h"
#include "dsmccdemuxer.h"
#include "module.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {

DSMCCFilter::DSMCCFilter( DSMCCDemuxer *demux )
	: _demux(demux)
{
	DTV_ASSERT(_demux);

	_totalSize = -1;
	_progress = -1;

	//	Add this as filter
	_demux->addFilter( this );
}

DSMCCFilter::~DSMCCFilter()
{
}

//	Getters
ResourceManager *DSMCCFilter::resourceMgr() {
	return _demux->resourceMgr();
}

DSMCCDemuxer *DSMCCFilter::demux() const {
	return _demux;
}

//	Notifications
void DSMCCFilter::notify( int step ) {
	int oldPass = (_progress * 100) / _totalSize;
	_progress += step;
	DTV_ASSERT( _progress <= _totalSize );
	int newPass = (_progress * 100) / _totalSize;
	if (!_onProgress.empty() && oldPass != newPass) {
		LDEBUG( "DSMCCFilter", "Notify: step=%d, progress=%d, total=%d", step, _progress, _totalSize );
		_demux->dispatchNotify( boost::bind(_onProgress,_progress,_totalSize) );
	}
}

void DSMCCFilter::onProgressChanged( const OnProgressChanged &callback ) {
	_onProgress = callback;
}

//	DSI methods
void DSMCCFilter::filterDSI() {
	//	Setup DSI filter
	dsmcc::DSMCCDemuxer::DSICallback onParsed = boost::bind( &DSMCCFilter::onDSI,this,_1 );
	_demux->onDSI( onParsed );
}

void DSMCCFilter::onDSI( const boost::shared_ptr<DSI> &dsi ) {
	dsi->show();
	processDSI( dsi );
}

void DSMCCFilter::processDSI( const boost::shared_ptr<DSI> & /*dsi*/ ) {
}

//	DII methods
void DSMCCFilter::filterDII() {
	//	Setup DII filter
	dsmcc::DSMCCDemuxer::DIICallback onParsed = boost::bind( &DSMCCFilter::onDII,this,_1 );
	_demux->onDII( onParsed );
}

void DSMCCFilter::onDII( const boost::shared_ptr<DII> &dii ) {
	dii->show();
	processDII( dii );
}

void DSMCCFilter::processDII( const boost::shared_ptr<DII> &/*dii*/ ) {
}

//	Modules
void DSMCCFilter::filterModules( const boost::shared_ptr<DII> &dii ) {
	//	Setup module callback
	dsmcc::DSMCCDemuxer::ModuleCallback onModule = boost::bind( &DSMCCFilter::onModule,this,_1 );
	_demux->onModule( onModule );

	//	Calculate total download size
	_totalSize = 0;
	_progress = 0;
	BOOST_FOREACH( const module::Type &mod, dii->modules() ) {
		_totalSize += mod.size;
	}
	LDEBUG( "DSMCCFilter", "Total size to download: %d", _totalSize );

	//	Notify begin download
	notify( 0 );

	//	Filter all modules in DII
	_demux->onBlockDownloaded( boost::bind( &DSMCCFilter::onBlock,this,_1 ) );
	_demux->filterModules( dii->downloadID(), dii->modules(), dii->blockSize() );
}

void DSMCCFilter::onModule( const boost::shared_ptr<Module> &module ) {
	module->show();
	processModule( module );
}

void DSMCCFilter::onBlock( size_t bytes ) {
	notify( bytes );
}

void DSMCCFilter::processModule( const boost::shared_ptr<Module> &/*module*/ ) {
}

}
}

