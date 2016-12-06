/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif

#include "rtpsink.h"
#include <util/buffer.h>
#include <util/log.h>
#include <util/assert.h>
#include <UsageEnvironment.hh>

namespace tuner {
namespace ts {

RtpSink::RtpSink( demuxer::ts::DemuxerImpl *demux, UsageEnvironment *env )
	: MediaSink(*env) {

	_fBuffer = NULL;
	_demux = demux;
}

RtpSink::~RtpSink() {
}

Boolean RtpSink::continuePlaying() {
	bool result=false;

	if (fSource != NULL) {
		if (!_fBuffer) {
			_fBuffer = _demux->allocBuffer();
			DTV_ASSERT(_fBuffer);
		}

		fSource->getNextFrame(_fBuffer->bytes(), _fBuffer->capacity(), afterGettingFrame, this, onSourceClosure, this);
		result=true;
	}
	return result;
}

void RtpSink::afterGettingFrame( void* clientData,
                                 unsigned frameSize,
                                 unsigned /*numTruncatedBytes*/,
                                 struct timeval /*presentationTime*/,
                                 unsigned /*durationInMicroseconds*/) {
	RtpSink *sink = (RtpSink*)clientData;
	sink->afterGettingFrame1(frameSize);
}

void RtpSink::afterGettingFrame1(unsigned frameSize) {
	if (_fBuffer && frameSize) {
		_fBuffer->resize( frameSize );
		_demux->pushData( _fBuffer );
		_fBuffer=NULL;
		continuePlaying();
	}
}

void RtpSink::fin() {
	if (_fBuffer!=NULL) {
		_demux->freeBuffer( _fBuffer );
		_fBuffer=NULL;
	}
}

}
}
