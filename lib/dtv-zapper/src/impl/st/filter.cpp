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

#include "filter.h"
#include "provider.h"
#include <util/log.h>
#include <mpegparser/demuxer/psi/psi.h>
#include <mpegparser/demuxer/psi/psidemuxer.h>

extern "C" {
#include "stapp_main.h"
}

namespace st {

Filter::Filter( Provider *prov, tuner::ID pid, util::DWORD timeout )
	: tuner::Filter( pid, timeout )
{
	_prov      = prov;
	_ptiSlot   = NULL;
	_ptiBuffer = NULL;
}

Filter::~Filter()
{
}

bool Filter::initialized() const {
	return _ptiBuffer ? true : false;
}

bool Filter::initialize() {
	ST_ErrorCode_t err;
	STPTI_SlotData_t ptiSlotData;

	LDEBUG("st", "init filter");
	
	//	Allocate a buffer to receive the section
	if (!allocBuffer( _prov->pti(), &_ptiBuffer )) {
		return false;
	}

	//	Allocate a slot
	memset(&ptiSlotData,0,sizeof(STPTI_SlotData_t));
	ptiSlotData.SlotType                                      = STPTI_SLOT_TYPE_SECTION;
	ptiSlotData.SlotFlags.AlternateOutputInjectCarouselPacket = FALSE;
	ptiSlotData.SlotFlags.AppendSyncBytePrefixToRawData       = FALSE;
	ptiSlotData.SlotFlags.CollectForAlternateOutputOnly       = FALSE;
	ptiSlotData.SlotFlags.ForcePesLengthToZero                = FALSE;
	ptiSlotData.SlotFlags.InsertSequenceError                 = FALSE;
#ifdef ST_7108
	ptiSlotData.SlotFlags.OutPesWithoutMetadata               = FALSE;
#else
	ptiSlotData.SlotFlags.OutPesWithoutMetadata               = TRUE;
#endif
	ptiSlotData.SlotFlags.SignalOnEveryTransportPacket        = FALSE;
	ptiSlotData.SlotFlags.StoreLastTSHeader                   = FALSE;
	ptiSlotData.SlotFlags.SoftwareCDFifo                      = FALSE;

	setupSlot( &ptiSlotData );
	err=STPTI_SlotAllocate( _prov->pti(), &_ptiSlot, &ptiSlotData );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to allocat slot");
		deinitialize();
		return false;
	}

	err=STPTI_SlotClearPid( _ptiSlot );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to clear PID");
		deinitialize();
		return false;		
	}
	
	//	Active CC control
	err=STPTI_SlotSetCCControl( _ptiSlot, setCCContrl() ? TRUE : FALSE );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to set CC control to slot");
		deinitialize();
		return false;		
	}

	//	Associate slot to buffer
	err=STPTI_SlotLinkToBuffer( _ptiSlot, _ptiBuffer );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to associate a PTI slot with a PTI buffer" );
		deinitialize();
		return false;		
	}

	//	Associate buffer to provider pti signal
	err=STPTI_SignalAssociateBuffer( _prov->signal(), _ptiBuffer );
	if(err!=ST_NO_ERROR) {
		LWARN("st", "Unable to associate PTI buffer to PTI provider signal: err=%d", err );
		deinitialize();
		return false;		
	}

	return true;
}

void Filter::setupSlot( STPTI_SlotData_t * /*slot*/ ) {
}

void Filter::deinitialize() {
	LDEBUG("st", "fin filter");
	
	if (_ptiSlot) {
		STPTI_SlotDeallocate( _ptiSlot );
		_ptiSlot = NULL;
	}

	if (_ptiBuffer) {
		STPTI_SignalDisassociateBuffer( _prov->signal(), _ptiBuffer );
		
		STPTI_BufferDeallocate( _ptiBuffer );
		_ptiBuffer = NULL;
	}
}

bool Filter::start() {
	LDEBUG("st", "start filter");
	
	//	Set the pid and let's see
	if (STPTI_SlotSetPid( _ptiSlot, pid() ) != ST_NO_ERROR) {
		LWARN("st", "Unable to set the pid to a PTI slot");
		return false;
	}

	//	Add pid to list of valid
	_prov->addPID( _ptiBuffer, this );
	
	return true;
}

void Filter::stop() {
	ST_ErrorCode_t err;

	LDEBUG("st", "stop filter");
	
	//	Remove pid to list of valid
	_prov->rmPID( _ptiBuffer );
	
	//	Clear the pid
	err=STPTI_SlotClearPid( _ptiSlot );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "cannot clear slot");
	}
}

bool Filter::setCCContrl() const {
	return true;
}

}

