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
#include "urlfrontend.h"
#include "../../../demuxer/ts/demuxer.h"
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/lexical_cast.hpp>

REGISTER_INIT_CONFIG( tuner_provider_tsdata_url ) {
	root().addNode( "url" )
		.addValue( "host", "Host ip address", std::string("239.255.0.1") )
		.addValue( "firstport", "First port to scan", (util::WORD)1234 )
		.addValue( "lastport", "Last port to scan", (util::WORD)1244 );
}

namespace tuner {
namespace ts {

URLFrontend::URLFrontend( demuxer::ts::DemuxerImpl *demux )
	: Frontend(demux)
{
	_host = util::cfg::getValue<std::string>("tuner.provider.tsdata.url.host");
	_firstPort = (util::WORD)util::cfg::getValue<util::WORD>("tuner.provider.tsdata.url.firstport");
	_lastPort = (util::WORD)util::cfg::getValue<util::WORD>("tuner.provider.tsdata.url.lastport");
}

URLFrontend::~URLFrontend()
{
}

size_t URLFrontend::getCount() const {
	return _lastPort-_firstPort+1;
}

std::string URLFrontend::getName( size_t nIndex ) const {
	DTV_ASSERT( nIndex < getCount() );
	std::string tmp = _host;
	tmp += ":";
	tmp += boost::lexical_cast<std::string>( _firstPort+nIndex );
	return tmp;
}

size_t URLFrontend::find( const std::string &name ) const {
	size_t pos = name.find_first_of( ":" );
	DTV_ASSERT( pos != std::string::npos);
	return boost::lexical_cast<size_t>( name.substr( pos+1 ) ) - _firstPort;
}

bool URLFrontend::parseUrl(const std::string &url, std::string* host, std::string* port) {
	//  TODO: parse url ok! hostname:port
	size_t pos = url.find_first_of( ":" );
	if (pos == std::string::npos) {
		LERROR( "ts::URLFrontend", "Invalid URL: %s", url.c_str() );
		return false;
	}
	*host = url.substr( 0, pos );
	*port = url.substr( pos+1 );

	return true;
}

std::string URLFrontend::startImpl( size_t nIndex ) {
	std::string net = getName( nIndex );
	if (!connect(net)) {
		net = "";
	}
	return net;
}

}
}
