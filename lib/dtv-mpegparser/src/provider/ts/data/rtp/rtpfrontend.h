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

/*******************************************************************************
  To use rtptuner export ZAPPER_PROVIDER="rtp" before running lzapper,
  mark RTP option in deps.cmake and lunch dvblast this way:
    dvblast -d <ip_address>:<port> -f <frequency> -e -u
    
    ej: dvblast -d 127.0.0.1:1234 -f 503142857 -e -u
  
  NOTE:  port must be between 1234 and 1244
*******************************************************************************/
#pragma once

#include "../urlfrontend.h"
#include <util/buffer.h>
#include <string>

class UsageEnvironment;
class Groupsock;
class RTPSource;

namespace tuner {
namespace ts {

class RtpSink;

class RTPFrontend : public URLFrontend {
public:
	RTPFrontend( demuxer::ts::DemuxerImpl *demux );
	virtual ~RTPFrontend( void );

protected:
	virtual void stopImpl();
	virtual void reader( const std::string net );
	virtual bool connect( const std::string &host );

private:
	UsageEnvironment* _usageEnvironment;
	Groupsock* _rtpGroupsock;
	RtpSink* _rtpSink;
	RTPSource* _rtpSource;
	char _exit;
};

}
}
