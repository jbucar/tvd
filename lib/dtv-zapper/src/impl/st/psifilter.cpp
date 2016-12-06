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

#include "psifilter.h"
#include "provider.h"
#include <mpegparser/demuxer/psi/psi.h>
#include <mpegparser/demuxer/psi/psidemuxer.h>
#include <util/log.h>
#include <util/buffer.h>

extern "C" {
#include "stapp_main.h"
}

namespace st {

PSIFilter::PSIFilter( Provider *prov, tuner::PSIDemuxer *demux )
	: Filter( prov, demux->pid(), demux->timeout() )
{
	util::DWORD freq = demux->frequency();
	if (freq && freq < 500 && demux->maxSectionBytes() == TSS_PRI_MAX_BYTES) {
		//	1MB for high bitrate
		_size = 1024*1024;
	}
	else {
		//	8K for PAT/PMT, etc
		_size = 8*1024;
	}
	_tableID   = demux->tableID();
	_ptiFilter = NULL;
}

PSIFilter::~PSIFilter()
{
}

void PSIFilter::read() {
	ST_ErrorCode_t err;
	STPTI_Filter_t filterList[1];
	U32            filterMatch;
	BOOL           filterCRC;
	U32            bytesCopied;
	util::Buffer  *buf;

	//	Get network buffer
	buf = _prov->getNetworkBuffer();

	//	Read data from buffer
	err=STPTI_BufferReadSection(
		_ptiBuffer,
		filterList,
		1,
		&filterMatch,
		&filterCRC,
		(U8 *)buf->data(),
		buf->capacity(),
		NULL,
		0,
		&bytesCopied,
#ifdef DVD_TRANSPORT_STPTI5
		STPTI_COPY_TRANSFER_BY_MEMCPY
#else
		STPTI_COPY_TRANSFER_BY_DMA
#endif
	);

	if (err != ST_NO_ERROR || !filterCRC) {
#ifndef ST_7109
		if (err != STPTI_ERROR_NO_PACKET) {
#endif
			LWARN("st", "during section read or CRC fail: buffer=%p, err=%08x, crc=%d", _ptiBuffer, err, filterCRC );
#ifndef ST_7109
		}
#endif
	}
	else if (bytesCopied) {
		//	Setup error code
		buf->resize( bytesCopied );
		_prov->enqueue( pid(), buf );
		buf = NULL;
	}

	if (buf) {
		_prov->freeNetworkBuffer( buf );
	}
}

bool PSIFilter::allocBuffer( STPTI_Handle_t pti, STPTI_Buffer_t *buffer ) {
	ST_ErrorCode_t err;

	//	Allocate buffer
	err=STPTI_BufferAllocate( pti, _size, 1, buffer );
	if (err != ST_NO_ERROR) {
		LWARN("st", "Unable to allocate a PTI buffer");
		deinitialize();
		return false;
	}

	//	Allocate a filter
	err=STPTI_FilterAllocate( pti, STPTI_FILTER_TYPE_SECTION_FILTER_SHORT_MODE, &_ptiFilter );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to allocate a PTI filter");
		deinitialize();
		return false;
	}

	return true;
}

void PSIFilter::deinitialize() {
	if (_ptiFilter) {
		STPTI_FilterDeallocate( _ptiFilter );
		_ptiFilter = NULL;
	}
	Filter::deinitialize();
}

bool PSIFilter::start() {
	ST_ErrorCode_t     err;
	STPTI_FilterData_t ptiFilterData;
	U8                 filterBytes[16];
	U8                 filterMask[16];
	U32 progNumber;    // Program Number to filter
	U32 verNumber;     // Version Number to filter
	U32 secNumber;     // Section Number to filter
	U32 curNotNext;    // Current or next to filter indicator
	BOOL notMatchMode; // Filter is negative mode

	//	Initialize params
	if (_tableID == PSI_TID_FORBIDDEN) {
		_tableID = STTABLE_DONT_CARE;
	}

	verNumber    = STTABLE_DONT_CARE;
	progNumber   = STTABLE_DONT_CARE;
	secNumber    = STTABLE_DONT_CARE;
	curNotNext   = STTABLE_DONT_CARE;
	notMatchMode = FALSE;

	//	Create filter mask and bytes
	memset(filterBytes,0,sizeof(filterBytes));
	memset(filterMask ,0,sizeof(filterMask));

	//	Table_id
	if ((_tableID & STTABLE_DONT_CARE)==0) {
		if (_tableID < 0x100) {
			filterMask[0] = 0xFF;
		}
		else {
			filterMask[0] = (U8)(_tableID >> 8);
		}
		filterBytes[0] = (U8)(_tableID & 0xFF);
	}

	//	Program_number
	if ((progNumber & STTABLE_DONT_CARE)==0) {
		filterMask[1]  = 0xFF;
		filterBytes[1] = (U8)(progNumber >> 8);
		filterMask[2]  = 0xFF;
		filterBytes[2] = (U8)(progNumber & 0xFF);
	}

	//	Version_number
	if ((verNumber&STTABLE_DONT_CARE)==0) {
		if (notMatchMode==TRUE) {
			filterMask[3]  = 0xC1;
		}
		else {
			filterMask[3] |= 0x3E;
		}
		filterBytes[3] |= (verNumber&0x1F)<<1;
	}

	//	Current_next_indicator
	if (((curNotNext&STTABLE_DONT_CARE)==0)&&(notMatchMode==FALSE)) {
		filterMask[3]  |= 0x01;
		filterBytes[3] |= (curNotNext&0x1);
	}

	//	Section_number
	if ((secNumber&STTABLE_DONT_CARE)==0) {
		filterMask[4]  = 0xFF;
		filterBytes[4] = (U8)secNumber;
	}

	//	Setup filter
	memset(&ptiFilterData,0,sizeof(STPTI_FilterData_t));
	ptiFilterData.FilterBytes_p                     = filterBytes;
	ptiFilterData.FilterMasks_p                     = filterMask;
	ptiFilterData.FilterType                        = STPTI_FILTER_TYPE_SECTION_FILTER_SHORT_MODE;
	ptiFilterData.FilterRepeatMode                  = STPTI_FILTER_REPEAT_MODE_STPTI_FILTER_REPEATED;
	ptiFilterData.InitialStateEnabled               = TRUE;
	ptiFilterData.u.SectionFilter.DiscardOnCrcError = TRUE;
	ptiFilterData.u.SectionFilter.ModePattern_p     = NULL;
	ptiFilterData.u.SectionFilter.NotMatchMode      = notMatchMode;
	ptiFilterData.u.SectionFilter.OverrideSSIBit    = FALSE;
	err=STPTI_FilterSet( _ptiFilter, &ptiFilterData );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to setup PTI filter");
		return false;
	}

	//	Associate the filter to slot
	err=STPTI_FilterAssociate( _ptiFilter, _ptiSlot );
	if (err!=ST_NO_ERROR) {
		LWARN("st", "Unable to associate a PTI filter");
		return false;
	}

	return Filter::start();
}

void PSIFilter::stop() {
	Filter::stop();

	//	Disassociate filter of slot
	if (STPTI_FilterDisassociate( _ptiFilter, _ptiSlot ) != ST_NO_ERROR) {
		LWARN("st", "cannot disassociate filter from slot");
	}
}

}

