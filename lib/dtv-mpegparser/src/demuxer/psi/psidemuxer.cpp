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
#include "psidemuxer.h"
#include "tableinfo.h"
#include "psifilter.h"
#include "cache.h"
#include "psi.h"
#include "generated/config.h"
#include <util/buffer.h>
#include <util/functions.h>
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>


REGISTER_INIT_CONFIG( tuner_demuxer ) {
	root().addNode("demuxer").addNode("psi")
		.addValue( "timeout", "PSI timeout base", DEMUXER_PSI_TIMEOUT )
		.addValue( "filterTID", "Filter tID in PSI sections", false );
}

namespace tuner {

/**
 * Constructor base.
 * @param pid Identificador de la tabla.
 */
PSIDemuxer::PSIDemuxer( ID pid )
{
	_pid = pid;
	_error = false;
	_buffer = NULL;
	_stuffingByteFlag = false;
	_enableCRC = true;
	_psiTimeoutBase = util::cfg::getValue<int>("tuner.demuxer.psi.timeout");
	_filterTID = util::cfg::getValue<bool>("tuner.demuxer.psi.filterTID");
	_cache = NULL;
}

/**
 * Destructor base.
 */
PSIDemuxer::~PSIDemuxer()
{
	CLEAN_ALL( PSIFilter *, _filters );
	delete _buffer;
}

/**
 * @return El pid de la tabla.
 */
ID PSIDemuxer::pid() const {
	return _pid;
}

/**
 * @return El identificador de la tabla.
 */
ID PSIDemuxer::tableID() const {
	return PSI_TID_FORBIDDEN;
}

bool PSIDemuxer::singleTableID() const {
	return true;
}

bool PSIDemuxer::checkTableID( ID tID ) const {
	//	Only filter tableID if that is not forbidden
	if (singleTableID()) {
		return tableID() == tID;
	}
	return false;
}

bool PSIDemuxer::filterTableID( util::BYTE &tID, util::BYTE &mask ) const {
	if (_filterTID && singleTableID()) {
		tID = tableID();
		mask = 0xFF;
		return true;
	}
	tID = 0;
	mask = 0;
	return false;
}

/**
 * @return La cantidad máxima de bytes por sector.
 */
WORD PSIDemuxer::maxSectionBytes() const {
	return TSS_ISO_MAX_BYTES;
}

/**
 * Tiempo límite para que llegue la tabla.
 * @return El timeout del filtro.
 */
DWORD PSIDemuxer::timeout() const {
	return frequency() * _psiTimeoutBase;
}

/**
 * @return La frecuencia en la cual tiene que aparecer la tabla.
 */
DWORD PSIDemuxer::frequency() const {
	//	ABNT NBR 15603-2:2007; Table 6 in One o more every n in miliseconds
	return 0;	//	Default 0 seconds
}

/**
 * @param enableCheck Indica si se quiere o no chequear el crc.
 */
void PSIDemuxer::checkCRC( bool enableCheck ) {
	_enableCRC = enableCheck;
}

/**
 * Agrega el filtro pasado por parámetro.
 * @param filter Instancia de @c PSIFilter a agregar.
 */
void PSIDemuxer::addFilter( PSIFilter *filter ) {
	_filters.push_back( filter );
}

//	Cache methods
void PSIDemuxer::setCache( psi::Cache *cache ) {
	_cache = cache;
}

/**
 * @return true si soporta el uso de cache, false en caso contrario.
 */
bool PSIDemuxer::supportCache() const {
	return false;
}

void PSIDemuxer::cacheSection( BYTE *section, size_t len ) {
	if (_cache && supportCache()) {
		_cache->put( pid(), section, len+4 );
	}
}

/**
 * Setea el dispatcher a ser llamado cuando expira el timeout de la tabla.
 * @param callback El callback a setear.
 */
void PSIDemuxer::setDispatcher( const DispatcherCallback &callback ) {
	_dispatcher = callback;
}

/**
 * Procesa el buffer realizando el push o descartandolo en caso de haber expirado el tiempo.
 * @param buf Buffer con datos a procesar.
 */
void PSIDemuxer::process( util::Buffer *buf ) {
	if (buf) {
		push( buf->bytes(), buf->length(), true );
	}
	else {
		timeoutExpired();
	}
}

/**
 * Realiza el demultiplexado de datos.
 * @param data Datos a demultiplexar.
 * @param len Indica la longitud de los datos.
 * @param start Indica si se quiere realizar ahora o si se deben guardar en un buffer.
 */
void PSIDemuxer::push( util::BYTE *data, size_t len, bool start ) {
	if (start) {
		//	Loop for payload parsing psi sections or stuffing bit
		size_t offset=0;
		while (offset < len && *(data+offset) != PSI_STUFFING_BYTE) {
			//	Parse section header
			offset += startHeader( data+offset, len-offset );
		}
	}
	else {
		Buffer *buf = getBuffer();

		//	Section is open?
		if (buf->length() > 0) {
			//	Copy payload to buffer
			buf->append( data, len );

			//	Check if section was finished ...
			if (endSection( buf->bytes(), buf->length() ) > 0) {
				buf->resize( 0 );
			}
		}
		else if (_error) {
			LWARN( "PSIDemuxer", "Section start with error, ignored" );
		}
		else if (!_stuffingByteFlag) {
			Buffer other( data, len, false );
			LWARN( "PSIDemuxer", "Section not opened pid=%04x, len=%ld, buf=%s",
				pid(), len, other.asHexa().c_str() );
		}
	}
}

size_t PSIDemuxer::startHeader( BYTE *payload, size_t len ) {
	size_t bytesUsed=0;

	//	Mark not error
	_error = false;

	//	Mark stuffing bytes flags
	_stuffingByteFlag = false;

	//	Check if a complete section is present in payload
	bytesUsed = endSection( payload, len );

	//LDEBUG( "PSIDemuxer", "StartHeader: pid=%x, len=%ld, bytesUsed=%ld", pid(), len, bytesUsed );

	//	Not complete?
	if (!bytesUsed) {
		Buffer *buf = getBuffer();

		//	Check if section was already opened
		if (buf->length() > 0) {
			//	Close section
			buf->resize( 0 );
			LWARN( "PSIDemuxer", "section already opened: pid=%04x", pid() );
		}
		//	add data to new section
		buf->append( payload, len );
		bytesUsed = len;
	}

	return bytesUsed;
}

size_t PSIDemuxer::endSection( BYTE *payload, size_t len ) {
	size_t bytesUsed=0;

	//	Generic header present (len included)
	if (len > PSI_GENERIC_HEADER) {
		//	Check section len
		size_t sLen = PSI_SECTION_LENGTH(payload);
		if (sLen > TSS_PRI_MAX_BYTES) { //	Can be implemented via virtual method ... its not worth
			LWARN( "PSIDemuxer", "Section len exceded. stuffingFlag=%d, len=%ld, sLen=%ld",
				_stuffingByteFlag, len, sLen );

			_error = true;
			return sLen;
		}

		//	Check tableID
		ID tID = PSI_TABLE(payload);
		if (tID == PSI_TID_FORBIDDEN) {
			LWARN( "PSIDemuxer", "Table ID forbidden: pid=%x, tid=%x", pid(), tID );
			_error = true;
			return sLen;
		}
		else if (tID > ABNT_TID_FORBIDDEN) {
			LWARN( "PSIDemuxer", "Table ID forbidden in ABNT: pid=%x, tid=%x", pid(), tID );
		}

		//	Check TableID
		if (!checkTableID( tID )) {
			_error = true;
			LTRACE( "PSIDemuxer", "Table ID ignored by filter: pid=%x, tid=%x", pid(), tID );
			return sLen;
		}

		//	Check syntax of section
		bool packetSyntax = PSI_SYNTAX(payload) ? true : false;
		if (checkSyntax() && packetSyntax != syntax()) {
			LWARN( "PSIDemuxer", "Syntax not valid for this psi section: pid=%x, syntax=%d, tid=%x, len=%ld, sectionLen=%ld",
				pid(), packetSyntax, tID, len, sLen );
			_error = true;
			return sLen;
		}

		//	Is section complete?
		if (len >= sLen) {
			int packetLen = sLen - PSI_CRC_SIZE;

			//	If generic syntax
			if (packetSyntax) {
				//	Is CRC correct?
				bool crcOK = _enableCRC ? checkCRC( payload, packetLen ) : true;
				if (crcOK) {
					if (useStandardVersion()) {
						//	Section is applicable? (current_next_indicator)
						bool isApplicable=PSI_NEXT(payload);

						//	Get table info
						TableInfo *info = getTable( payload );
						DTV_ASSERT(info);

						//	Version changed?
						Version newVersion = PSI_VERSION(payload);
						if (info->versionChanged(newVersion)) {
							//	Expire if necesary
							if (!_onExpired.empty() && info->expire( pid() )) {
								notify( boost::bind(_onExpired,pid()) );
							}

							//	If section is applicable
							if (isApplicable) {
								//	Update version
								info->update(newVersion);
								onVersionChanged();

								//	Process section
								if (supportMultipleSections()) {
									parseSection( info, payload, packetLen );
								}
								else if (!PSI_NUMBER(payload) && !PSI_LAST(payload)) {
									onSection( payload, packetLen );
									cacheSection( payload, packetLen );
								}
								else {
									LWARN( "PSIDemuxer", "demuxer not support multiple sections, but cur y last is not 0: pid=%04x, cur=%02x, last=%02x",
										pid(), PSI_NUMBER(payload), PSI_LAST(payload) );
								}
							}
						}
						else if (isApplicable && supportMultipleSections() && !info->wasParsed( PSI_NUMBER(payload) )) {
							parseSection( info, payload, packetLen );
						}
					}
					else {
						//	Call custom parser!
						onSection( payload, packetLen );
					}
				}
				else {
					LWARN( "PSIDemuxer", "CRC error!" );
				}
			}
			else {
				//	Private table, continue parsing (No CRC check!)
				onSection( payload, packetLen );
			}

			//	Section finished
			bytesUsed = sLen;
			size_t resto = len - bytesUsed;
			_stuffingByteFlag =	 (resto > 0 && payload[bytesUsed] == PSI_STUFFING_BYTE);
			//LDEBUG( "PSIDemuxer", "End section=%x, len=%ld, sectionLen=%ld, resto=%ld, stuffing=%d", pid(), len, sLen, resto, _stuffingByteFlag );
		}
	}

	// int x = PSI_SECTION_LENGTH(payload);
	// LDEBUG( "PSIDemuxer", "End section=%x, len=%ld, secLen=%d, used=%ld, syntax=%d", pid(), len, x, bytesUsed, PSI_SYNTAX(payload) );

	return bytesUsed;
}

bool PSIDemuxer::checkCRC( util::BYTE *payload, int packetLen ) {
	DWORD crc  = GET_CRC( payload + packetLen );
	DWORD calc = util::crc_calc( util::DWORD(-1), payload, packetLen );
	//	LDEBUG( "PSIDemuxer", "CRC values: crc=%08x, calc=%08x", crc, calc );
	return (crc == calc);
}

bool PSIDemuxer::useStandardVersion() const {
	return true;
}

void PSIDemuxer::onVersionChanged() {
	//	Do nothing
}

TableInfo *PSIDemuxer::getTable( BYTE * /*section*/ ) {
	return NULL;
}

bool PSIDemuxer::supportMultipleSections() const {
	return syntax() && useStandardVersion();
}

void PSIDemuxer::parseSection( TableInfo *ver, BYTE *section, size_t len ) {
	if (ver->checkSection( section )) {
		//	Parse section
		onSection( ver, section, len );
		cacheSection( section, len );
	}

	//	If section complete?
	if (ver->isComplete()) {
		onComplete( ver, section, len );
	}
}

void PSIDemuxer::onSection( util::BYTE * /*section*/, size_t /*len*/ ) {
	//	Used when:
	//		Table haven't standard version
	//		Table have syntax=0
}

void PSIDemuxer::onSection( TableInfo * /*ver*/, util::BYTE * /*section*/, size_t /*len*/ ) {
}

void PSIDemuxer::onComplete( TableInfo * /*ver*/, util::BYTE * /*section*/, size_t /*len*/ ) {
}

bool PSIDemuxer::syntax() const {
	return true;
}

bool PSIDemuxer::checkSyntax() const {
	return true;
}

Buffer *PSIDemuxer::getBuffer() {
	if (!_buffer)  {
		_buffer = new Buffer( TSS_PRI_MAX_BYTES );
	}
	return _buffer;
}

bool PSIDemuxer::directDispatch() const {
	return !_filters.empty() || _dispatcher.empty();
}

void PSIDemuxer::notify( const boost::function<void (void)> &callback ) {
	notify( callback, directDispatch() );
}

	//	Notification (Implementation)
void PSIDemuxer::dispatchNotify( const boost::function<void (void)> &callback ) {
	notify( callback, _dispatcher.empty() );
}

void PSIDemuxer::notify( const boost::function<void (void)> &callback, bool direct ) {
	DTV_ASSERT( !callback.empty() );

	if (direct) {
		callback();
	}
	else if (!_dispatcher.empty()) {
		_dispatcher(pid(),callback);
	}
	else {
		LWARN( "PSIDemuxer", "Callback dispatcher empty!" );
	}
}

/**
 * Setea el callback a ser llamado cuando expira el timeout de la tabla.
 * @param callback El callback a setear.
 */
void PSIDemuxer::onTimeout( const TimeoutCallback &callback ) {
	_onTimeout = callback;
}

void PSIDemuxer::timeoutExpired() {
	//LWARN( "PSIDemuxer", "timeout expired: pid=%04x", pid() );
	if (!_onTimeout.empty()) {
		notify( boost::bind(_onTimeout, pid()) );
	}
}

/**
 * Setea el callback a ser llamado cuando expira la tabla debido a que llega una nueva versión.
 * @param callback El callback a setear.
 */
void PSIDemuxer::onExpired( const ExpiredCallback &callback ) {
	_onExpired = callback;
}

bool PSIDemuxer::checkPID( ID pid, ID ignore/*=0xFFFF*/ ) {
	bool result;

	if (ignore != 0xFFFF && pid == ignore) {
		result=true;
	}
	else {
		switch (pid) {
			case TS_PID_NULL:
			case TS_PID_PAT:
			case TS_PID_CAT:
			case TS_PID_SDT:
				result=false;
				break;
			default:
				result=true;
				break;
		};
	}
	return result;
}

}
