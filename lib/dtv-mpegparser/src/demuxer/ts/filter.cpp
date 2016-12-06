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
#include "filter.h"
#include "psiwrapper.h"
#include "../../stream/pipe.h"
#include "../../demuxer/ts.h"
#include "../../demuxer/psi/psi.h"
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>

namespace tuner {
namespace demuxer {
namespace ts {

Filter::Filter( void )
{
	_continuityCounter = TS_INVALID_CONTINUITY;
}

Filter::~Filter( void )
{
}

bool Filter::checkContinuity( bool tsStart, util::BYTE actualCC ) {
	bool result=true;
	bool begin;

	// printf( "[ts::Filter] Check continuity begin: tsStart=%d, actualCC=%02x, counter=%02x\n",
	//         tsStart, actualCC, _continuityCounter );

	if (_continuityCounter != TS_INVALID_CONTINUITY) {
		begin = false;

		//	Get next continuity counter
		_continuityCounter++;
		_continuityCounter = (_continuityCounter % 0x10);
		if (_continuityCounter != actualCC) {
			//	Mark invalid
			_continuityCounter = TS_INVALID_CONTINUITY;
		}
	}
	else {
		//	Begin of a PID
		begin = true;
	}

	//	Recheck if continuity is invalid
	if (_continuityCounter == TS_INVALID_CONTINUITY && (tsStart || begin)) {
		_continuityCounter = actualCC;
	}

	//printf( "[ts::Filter] Check continuity: %d\n", result );

	return result;
}

void Filter::push( BYTE * /*tsPayload*/, size_t /*len*/, bool /*start*/ ) {
}

void Filter::push( BYTE * /*tsPayload*/, size_t /*len*/ ) {
}

bool Filter::processRawTS() const {
	return false;
}

//	Section filter
SectionFilter::SectionFilter( ID pid, const SectionFilterCallback &callback )
{
	_psi = new PSIWrapper( pid, callback );
}

SectionFilter::~SectionFilter()
{
	delete _psi;
}

ID SectionFilter::pid() const {
	return _psi->pid();
}

void SectionFilter::push( BYTE *tsPayload, size_t len, bool start ) {
	_psi->push( tsPayload, len, start );
}

//	Stream filter
StreamFilter::StreamFilter( bool processTS, ID pid, stream::Pipe *pipe )
{
	_processTS = processTS;
	_pid = pid;
	_pipe = pipe;
}

StreamFilter::~StreamFilter()
{
}

bool StreamFilter::processRawTS() const {
	return _processTS;
}

ID StreamFilter::pid() const {
	return _pid;
}

void StreamFilter::push( BYTE *tsPayload, size_t len, bool /*start*/ ) {
	util::Buffer *buf = _pipe->alloc();
	DTV_ASSERT( buf->capacity() > len );
	buf->copy( tsPayload, len );
	_pipe->push( buf );
}

void StreamFilter::push( BYTE *tsPayload, size_t len ) {
	util::Buffer *buf = _pipe->alloc();
	DTV_ASSERT( buf->capacity() > len );
	buf->copy( tsPayload, len );
	_pipe->push( buf );
}

}
}
}

