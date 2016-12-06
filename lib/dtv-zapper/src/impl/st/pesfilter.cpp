/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "pesfilter.h"
#include "provider.h"
#include <mpegparser/stream/pipe.h>
#include <mpegparser/demuxer/psi/psi.h>
#include <mpegparser/demuxer/psi/psidemuxer.h>
#include <util/buffer.h>
#include <util/log.h>

extern "C" {
#include "stapp_main.h"
}

#define BUFFER_SIZE (256*1024) //256K

namespace st {

PESFilter::PESFilter( Provider *prov, tuner::ID pid, tuner::stream::Pipe *pipe )
	: Filter( prov, pid, 0 )
{
	_pipe = pipe;
	_ptiBufferBase = 0;
	_ptiBufferBaseKernel = 0;
	_ptiBufferConsumer = 0;	
}

PESFilter::~PESFilter()
{
}

void PESFilter::read() {
	U32	ptiBufferProducer;
	U32 bytes;

	//	Get producter pointer
	ST_ErrorCode_t err = STPTI_BufferGetWritePointer(
		_ptiBuffer,
		(void **)&ptiBufferProducer
	);
	if(err != ST_NO_ERROR || (ptiBufferProducer - _ptiBufferBaseKernel) > BUFFER_SIZE) {
		LWARN( "st", "Cannot get buffer pointer, or is invalid: base=%08x, pointer=%08x",
			_ptiBufferBaseKernel, ptiBufferProducer );
		return;
	}

	ptiBufferProducer = ptiBufferProducer - _ptiBufferBaseKernel + _ptiBufferBase;

	//	Get available data
	if (ptiBufferProducer >= _ptiBufferConsumer) {
		bytes = ptiBufferProducer - _ptiBufferConsumer;
	}
	else {
		bytes = (_ptiBufferBase + BUFFER_SIZE) - _ptiBufferConsumer;
	}

	if (bytes != 0) {
		//	Push into pipe
		util::Buffer *buf = _pipe->alloc();
		buf->copy( (const char *)_ptiBufferConsumer, bytes );
		_pipe->push( buf );

		//	Update consumer pointer
		_ptiBufferConsumer += bytes;
		if (_ptiBufferConsumer >= (_ptiBufferBase + BUFFER_SIZE)) {
			_ptiBufferConsumer = _ptiBufferBase;
		}
	}
			
	//	Update productor pointer
	err = STPTI_BufferSetReadPointer(
		_ptiBuffer,
		(void *)(_ptiBufferConsumer - _ptiBufferBase + _ptiBufferBaseKernel)
	);
	if(err != ST_NO_ERROR) {
		LWARN( "st", "Cannot set buffer pointer" );
		return;
	}
}

void PESFilter::deinitialize() {
	Filter::deinitialize();
	if (_ptiBufferBase) {
		SYS_Memory_Deallocate(
			SYS_MEMORY_NCACHED | SYS_MEMORY_CONTIGUOUS | SYS_MEMORY_SECURED,
			(U32)_ptiBufferBase
		);
		_ptiBufferBase = 0;
	}
}

bool PESFilter::allocBuffer( STPTI_Handle_t pti, STPTI_Buffer_t *buffer ) {
	ST_ErrorCode_t err;

	_ptiBufferBase = (U32)SYS_Memory_Allocate(
		SYS_MEMORY_NCACHED | SYS_MEMORY_CONTIGUOUS | SYS_MEMORY_SECURED,
		BUFFER_SIZE,
		256
	);
	
	_ptiBufferBaseKernel = (U32)SYS_Memory_UserToKernel(
		SYS_MEMORY_NCACHED | SYS_MEMORY_CONTIGUOUS | SYS_MEMORY_SECURED,
		_ptiBufferBase
	);
	
	err = STPTI_BufferAllocateManual(
		pti,
		(U8 *)_ptiBufferBaseKernel,
		BUFFER_SIZE,
		1,
		buffer
	);
	if (err != ST_NO_ERROR) {
		LWARN("st", "Unable to allocate a PTI buffer");
		deinitialize();
		return false;		
	}

	_ptiBufferConsumer = _ptiBufferBase;

	return true;
}

void PESFilter::setupSlot( STPTI_SlotData_t *slot ) {
	slot->SlotType = STPTI_SLOT_TYPE_PES;
	slot->SlotFlags.SoftwareCDFifo = TRUE;
}

bool PESFilter::setCCContrl() const {
	return false;
}

}

