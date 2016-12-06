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
#include "provider.h"
#include "filter.h"
#include "../../stream/pipe.h"
#include "../../demuxer/ts/demuxerimpl.h"
#include "../../demuxer/ts/filter.h"
#include "../../demuxer/psi/psidemuxer.h"
#include "generated/config.h"
#include "data/frontend.h"
#include "data/filefrontend.h"
#include "data/networkfrontend.h"
#if RTP_USE_RTP
#include "data/rtp/rtpfrontend.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/buffer.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <string.h>
#include <stdio.h>

namespace tuner {
namespace ts {

REGISTER_INIT_CONFIG( tuner_provider_tsdata ) {
#if RTP_USE_RTP
	root().addNode( "tsdata" ).addValue( "use", "TS input to initialize", std::string("rtp") );
#else
	root().addNode( "tsdata" ).addValue( "use", "TS input to initialize", std::string("file") );
#endif
}

Provider::Provider()
{
	_tsDemuxer = new demuxer::ts::DemuxerImpl();
}

Provider::~Provider()
{
    delete _tsDemuxer;
}

//	Tuner methods
tuner::Frontend *Provider::createFrontend() const {
	Frontend *frontend=NULL;
		
	const std::string &use = util::cfg::getValue<std::string>("tuner.provider.tsdata.use");
	LINFO( "ts::Provider", "Using ts provider: %s", use.c_str() );
	if (use == "net") {
		frontend = new NetworkFrontend( _tsDemuxer );
	}
	else if (use == "file") {
		frontend = new FileFrontend( _tsDemuxer );
	}
#if RTP_USE_RTP
	else if (use == "rtp") {
		frontend = new RTPFrontend( _tsDemuxer );
	}
#endif

	return frontend;
}

//	Aux filters engine
bool Provider::startEngineFilter( void ) {
	LINFO( "ts::Provider", "Start engine filter" );
	_tsDemuxer->start();
	return true;
}

void Provider::stopEngineFilter( void ) {
	LINFO( "ts::Provider", "Stop engine filter" );
	_tsDemuxer->stop();
}

bool Provider::haveTimers() const {
	return false;
}

void Provider::onFilterCallback( demuxer::ts::SectionFilterData *data ) {
	enqueue( data->pid, data->buf, data->len );
}

//	Aux filters	
tuner::Filter *Provider::createFilter( PSIDemuxer *demux ) {
	//	Create section filter
	demuxer::ts::Filter *filter=new demuxer::ts::SectionFilter(
		demux->pid(),
		boost::bind(&Provider::onFilterCallback,this,_1)
	);

	//	Create provider filter
	return new Filter( _tsDemuxer, filter, demux->pid(), demux->timeout() );
}

tuner::Filter *Provider::createFilter( pes::FilterParams *params ) {
	demuxer::ts::Filter *filter = NULL;
	
	if (params->mode == pes::mode::pes ||
		params->mode == pes::mode::ts)
	{
		//	Create ts::Demuxer filter
		filter = new demuxer::ts::StreamFilter(
			params->mode == pes::mode::ts,
			params->pid,
			params->pipe
		);
	}
	else {
		filter = NULL;
	}

	if (filter) {
	 	//	Create provider filter
		return new Filter( _tsDemuxer, filter, params->pid );
	}
	return NULL;
}

}
}
