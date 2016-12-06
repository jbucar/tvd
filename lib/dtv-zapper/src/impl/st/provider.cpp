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

#include "provider.h"
#include "frontend.h"
#include "psifilter.h"
#include "pesfilter.h"
#include <mpegparser/types.h>
#include <mpegparser/service/service.h>
#include <mpegparser/demuxer/psi/psi.h>
#include <util/log.h>
#include <util/buffer.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>

extern "C" {
#include "stapp_main.h"
}

// #define MAX_REINTENT 5
#define MAX_LINE_LEN 512

namespace st {

//	Aux static functions
static S32 getPTI( void ) {
	ST_ErrorCode_t       ErrCode;
	S32                  ID;
	U8                   ConnectionName[MAX_LINE_LEN];
	PLAYREC_Connection_t PLAYREC_Connection;
	
	//	Try to get a connection name
	strcpy( (char *)ConnectionName, "0" );
	
	//	Try to get virtual pti number connected to this connection
	ErrCode=PLAYREC_GetConnection((char *)ConnectionName,&PLAYREC_Connection);
	if (ErrCode!=ST_NO_ERROR) {
	#if defined(OSPLUS)||defined(ST_OSLINUX)
		//	It could be a file to parse to get the sections
		//	ID parameter is now a pointer to the filename to be analyzed
		ID=(S32)ConnectionName;
	#else
		LWARN("st", "Unable to get info of this connection name");
		return -1;
	#endif
	}
	else {
		//	If the connection type is not a demux, go out
		if (PLAYREC_Connection.Type!=PLAYREC_CONNECTION_SRC_DEMUX) {
			LWARN("st", "The connection name is not a valid demux source");
			return -1;
		}

		//	 Look for pti id
		for (ID=0;ID<PTI_MAX_NUMBER;ID++) {
			if (PTI_Handle[ID]==PLAYREC_Connection.Description.SrcDemux.PTIHandle) {
				break;
			}
		}
		
		//	If pti id, not found, go out
		if (ID==PTI_MAX_NUMBER) {
			LWARN("st", "There is no PTI for this connection");
			return -1;
		}
	}
	return ID;
}

Provider::Provider( void )
{
	_ptiHandle = NULL;
	_ptiSignal = NULL;
}

Provider::~Provider()
{
}

STPTI_Handle_t Provider::pti() const {
	return _ptiHandle;
}

STPTI_Signal_t Provider::signal() const {
	return _ptiSignal;
}

void Provider::addPID( STPTI_Buffer_t buf, Filter *filter ) {
	_mutex.lock();
	_pids[buf] = filter;
	_mutex.unlock();
}

void Provider::rmPID( STPTI_Buffer_t buf ) {
	_mutex.lock();
	Filters::iterator it=_pids.find( buf );
	if (it != _pids.end()) {
		_pids.erase( it );
	}
	_mutex.unlock();
}

//	Tuner methods
tuner::Frontend *Provider::createFrontend() const {
	return new Frontend();
}

//	Aux filters
tuner::Filter *Provider::createFilter( tuner::PSIDemuxer *sectionDemux ) {
	return new PSIFilter( this, sectionDemux );
}

tuner::Filter *Provider::createFilter( tuner::pes::FilterParams *params ) {
	if (params->type == tuner::pes::type::subtitle) {
		return new PESFilter( this, params->pid, params->pipe );
	}
	else {
		return new tuner::Filter( params->pid );
	}
}

bool Provider::startEngineFilter( void ) {
	ST_ErrorCode_t err;	

	//	Get PTI ID
	S32 ptiID = getPTI();
	if (ptiID < 0) {
		LWARN("st", "Cannot get PTI ID");
		return false;
	}

	//	Initialize PTI handle
	_ptiHandle = PTI_Handle[ptiID];

	//	Allocate a signal
	err=STPTI_SignalAllocate( _ptiHandle, &_ptiSignal );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to allocate a PTI signal");
		_ptiHandle = NULL;		
		return false;		
	}

	//  Start ST read thread
	_thread = boost::thread( boost::bind( &Provider::reader, this ) );
	return true;
}

void Provider::stopEngineFilter( void ) {
	//	Send signal abort and wait for thread finish
	STPTI_SignalAbort( _ptiSignal );
	_thread.join();

	//	Deallocate signal
	STPTI_SignalDeallocate( _ptiSignal );
	_ptiSignal = NULL;

	LINFO("st", "stop filters ok");
}

int Provider::maxFilters() const {
	return 32;	//	TODO: Check this value!!!
}

bool Provider::haveTimers() const {
	return false;
}

void Provider::reader() {
	bool exit=false;
	ST_ErrorCode_t err;
	STPTI_Buffer_t buffer;
	U32            timeout=STPTI_TIMEOUT_INFINITY;

	LINFO("st", "Thread started");
	
	while (!exit) {
		//	Wait for signal reception		
		err=STPTI_SignalWaitBuffer( _ptiSignal, &buffer, timeout );
		switch (err) {
			case ST_ERROR_TIMEOUT: {
				LDEBUG("st", "Signal timeout");
				break;
			}
			case STPTI_ERROR_SIGNAL_ABORTED: {
				LDEBUG("st", "Signal aborted");
				exit=true;
				break;
			}
			case ST_NO_ERROR: {
				Filter *filter = NULL;	
				_mutex.lock();
				Filters::const_iterator it=_pids.find( buffer );
				if (it != _pids.end()) {
					filter = (*it).second;
					filter->read();
				}
				_mutex.unlock();				

				if (!filter) {
					LWARN( "st", "Cannot found filter for buffer" );
				}
				break;
			}
			default:
				LERROR("st", "Unknown error");
		};
	}
	
	LINFO("st", "Thread stopped");
}

}


