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

#include "rtpfrontend.h"
#include "rtpsink.h"
#include <util/buffer.h>
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <liveMedia.hh>
#include <GroupsockHelper.hh>
#include <BasicUsageEnvironment.hh>

namespace tuner {
namespace ts {

RTPFrontend::RTPFrontend( demuxer::ts::DemuxerImpl *demux )
	: URLFrontend(demux)
{
	_exit=(char)0;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	_usageEnvironment = BasicUsageEnvironment::createNew(*scheduler);
}

RTPFrontend::~RTPFrontend()
{
}

void RTPFrontend::stopImpl() {
	//	Signal thread to finish
	_exit='1';
	_rtpSink->stopPlaying();
}

bool RTPFrontend::connect( const std::string &url ) {
	std::string host, port;

	bool result = parseUrl(url, &host, &port);
	if (result) {
		struct in_addr sessionAddress;
		sessionAddress.s_addr = our_inet_addr(host.c_str());
		const Port rtpPort((portNumBits)atoi(port.c_str()));

		_rtpGroupsock = new Groupsock(*_usageEnvironment, sessionAddress, rtpPort, 8);
		_rtpSource = SimpleRTPSource::createNew(*_usageEnvironment, _rtpGroupsock, 33, 90000, "video/MP2T", 0, False );
		_rtpSink = new RtpSink(demux(), _usageEnvironment);
	}

	return result;
}

//	Running in provider thread
void RTPFrontend::reader( const std::string net ) {
	LDEBUG( "ts::RTPFrontend", "network thread started: net=%s", net.c_str() );
	_rtpSink->startPlaying(*_rtpSource, NULL, NULL);
	_usageEnvironment->taskScheduler().doEventLoop(&_exit);
	_rtpSink->fin();
	_exit=(char)0;
	MediaSink::close(_rtpSink);
	SimpleRTPSource::close(_rtpSource);
	DEL(_rtpGroupsock);
	LDEBUG( "ts::RTPFrontend", "network thread terminated: net=%s", net.c_str());
}

}
}
