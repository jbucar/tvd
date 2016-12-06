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
#include "module.h"
#include "module/memorydata.h"
#include "module/filedata.h"
#include <util/assert.h>
#include <util/log.h>
#include <zlib.h>

#include <memory.h>

namespace tuner {
namespace dsmcc {

Module::Module( util::DWORD downloadID, const module::Type &moduleInfo, util::WORD blockSize )
	: _module( moduleInfo )
{
	_downloadID = downloadID;
	_blockSize  = blockSize;
	_data       = createData( _module.size );
	_downloadedSize = 0;
	_downloaded.resize( (_module.size/blockSize)+1, false );
}

Module::~Module()
{
	freeResources();
}

//  Getters
util::DWORD Module::downloadID() const {
	return _downloadID;
}

ID Module::id() const {
	return _module.id;
}

util::DWORD Module::size() const {
	return _module.size;
}

util::BYTE Module::version() const {
	return _module.version;
}

const Buffer &Module::info() const {
	return _module.info;
}

void Module::show() const {
	LDEBUG( "dsmcc::Module", "Show: downloadID=%08x, moduleID=%04d, ver=%02x, size=%08d, blockSize=%04d, blocks=%d, info=%d",
		_downloadID, _module.id, _module.version, _module.size, _blockSize, _downloaded.size(), _module.info.length() );
}

bool Module::isComplete() const {
	return _module.size == _downloadedSize;
}

//	Basic read operations
bool Module::readDW( size_t &off, util::DWORD &var ) {
	util::BYTE tmp[sizeof(var)];
	if (read( off, tmp, sizeof(var) ) != sizeof(var)) {
		return false;
	}
	var = GET_DWORD(tmp);
	off += sizeof(var);
	return true;
}

bool Module::readW( size_t &off, util::WORD &var ) {
	util::BYTE tmp[sizeof(var)];	
	if (read( off, tmp, sizeof(var) ) != sizeof(var)) {
		return false;
	}
	var = GET_WORD(tmp);
	off += sizeof(var);
	return true;
}

//  Buffer operations
size_t Module::read( size_t off, util::BYTE *buf, size_t len ) {
	return _data->read( off, buf, len );
}

size_t Module::sendFile( FILE *of, size_t o_off, size_t i_off, size_t len ) {
	return _data->sendFile( of, o_off, i_off, len );
}

size_t Module::write( size_t off, util::BYTE *buf, size_t len ) {
	return _data->write( off, buf, len );
}

bool Module::saveAs( const std::string &name ) {
	return _data->saveAs( name );
}

bool Module::checkCRC32( util::DWORD crc ) {
	return _data->checkCRC32( crc );
}

int Module::pushData( util::WORD blockNumber, util::BYTE *buf, size_t len ) {
	int ret=0;
	//  Check blockNumber valid
	if (blockNumber < _downloaded.size()) {
		//  Check that blockNumber not already downloaded
		if (!_downloaded[blockNumber]) {
			//LDEBUG( "dsmcc::Module", "push data: downloadID=%08lx, moduleID=%04x, block=%04x, len=%ld", _downloadID, _module.id, blockNumber, len );
			size_t bytes = write( blockNumber*_blockSize, buf, len );
			DTV_ASSERT( bytes == len );
			_downloadedSize += bytes;
			_downloaded[blockNumber] = true;
			ret = len;
		}
	}
	return ret;
}

#define CHUNK 4096
void Module::inflate( util::BYTE /*type*/, util::DWORD originalSize ) {
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	//	Allocate inflate state
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = 0;
	strm.next_in  = Z_NULL;
	ret = inflateInit(&strm);
	if (ret == Z_OK) {
		//	Alloc a new module data for the outout
		ModuleData *infData = createData( originalSize );
	
		//	Decompress until deflate stream ends or end of module
		bool err=false;
		size_t r_off, w_off;
		r_off = w_off = 0;
		strm.avail_in = read( r_off, in, CHUNK );
		while (!err && strm.avail_in) {
			strm.next_in = in;
			
			//	run inflate() on input until output buffer not full
			do {
				strm.avail_out = CHUNK;
				strm.next_out  = out;
				ret = ::inflate(&strm, Z_NO_FLUSH);
				DTV_ASSERT(ret != Z_STREAM_ERROR);  //	state not clobbered
				switch (ret) {
					case Z_NEED_DICT:
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						LWARN( "dsmcc::Module", "inflate error: ret=%x", ret );
						err=true;
						break;
				}

				//	Write inflated data
				have = CHUNK - strm.avail_out;
				if (infData->write( w_off, out, have ) != have) {
					LWARN( "dsmcc::Module", "write error: ret=%x", ret );
					err=true;
					break;
				}
				w_off += have;
				
			} while (strm.avail_out == 0);
			//	done when inflate() says it's done

			if (!err) {
				r_off += CHUNK;
				strm.avail_in = read( r_off, in, CHUNK );
			}
		}

		//	clean up
		(void)inflateEnd(&strm);
		if (!err) {
			freeResources();
			_data = infData;
			_module.size = originalSize;
		}
		else {
			delete infData;
			_module.size = 0;
		}
	}	
}

//	Create moduleData 
ModuleData *Module::createData( util::DWORD size ) {
	ModuleData *data;
	
	//	Create module data (depends if can be downloaded in memory)
	if (MemoryData::canDownload( size )) {
		LDEBUG( "dsmcc::Module", "Using memory for data: downloadID=%08x, moduleID=%04d", _downloadID, _module.id );
		data = new MemoryData( size );
	}
	else {
		LDEBUG( "dsmcc::Module", "Using file for data: downloadID=%08x, moduleID=%04d", _downloadID, _module.id );
		data = new FileData( _module.size );
	}

	return data;
}

//  Resources operations
bool Module::canStartDownloading( ResourceManager *resMgr ) const {
	//  If free blocks availables, lock blocks
	return _data->canStart( resMgr );
}

void Module::freeResources( void ) {
	if (_data) {
		LDEBUG( "dsmcc::Module", "Free resources: downloadID=%08x, moduleID=%04x", _downloadID, _module.id );
		delete _data;
		_data = NULL;
		_downloadedSize = 0;
		_downloaded.clear();
	}
}

}
}

