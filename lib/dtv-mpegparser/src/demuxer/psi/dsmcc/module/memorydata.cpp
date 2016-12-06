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
#include "memorydata.h"
#include "resourcepool.h"
#include <util/buffer.h>
#include <util/functions.h>
#include <util/assert.h>
#include <boost/filesystem.hpp>
#include <memory.h>
#include <stdio.h>

namespace tuner {
namespace dsmcc {

namespace fs = boost::filesystem;

ResourcePool *MemoryData::_pool = NULL;

void MemoryData::initialize( DWORD maxBlocks, DWORD blockSize ) {
	_pool = new ResourcePool( "dsmcc", maxBlocks, blockSize );
}

void MemoryData::cleanup() {
	delete _pool;
	_pool = NULL;
}

bool MemoryData::canDownload( DWORD moduleSize ) {
	return _pool->canAlloc( moduleSize );
}

MemoryData::MemoryData( DWORD size )
	: ModuleData( size )
{
	_resourcesLocked = false;
	_blockSize = _pool->blockSize();
	_blocks.resize( _pool->blocks( size ), NULL );
	_crcBuf = (util::BYTE *)malloc(_blockSize);
	DTV_ASSERT(_crcBuf);
}

MemoryData::~MemoryData( void )
{
    //  Collect free blocks
	std::vector<util::Buffer *> toFree;
    for (size_t i=0; i<_blocks.size(); i++) {
		if (_blocks[i]) {
			toFree.push_back( _blocks[i] );
		}
    }
	//	Free blocks
	if (toFree.size()) {
		_pool->free( toFree, true );
	}

	//	Free lock from resources
	if (_resourcesLocked) {
		_pool->unlockBlocks( size() );
	}

	//	Clean structrure
    _blocks.clear();

	free(_crcBuf);
}

//	Getters
bool MemoryData::canStart( ResourceManager * /*resMgr*/ ) {
	DTV_ASSERT(!_resourcesLocked);
	_resourcesLocked = _pool->lockBlocks( size() );
	return _resourcesLocked;
}

DWORD MemoryData::blockSize() const {
    return _blockSize;
}

//  Buffer operations
size_t MemoryData::read( size_t off, BYTE *buf, size_t len ) {
    return process( off, len, buf, &MemoryData::doRead );
}

size_t MemoryData::sendFile( FILE *of, size_t o_off, size_t i_off, size_t len ) {
	::fseek( of, o_off, SEEK_SET );
    return process( i_off, len, of, &MemoryData::doSendfile );
}

size_t MemoryData::write( size_t off, BYTE *buf, size_t len ) {
    return process( off, len, buf, &MemoryData::doWrite );
}

bool MemoryData::checkCRC32( DWORD crc ) {
	size_t off = 0;
	DWORD calc = util::DWORD(-1);
	size_t bytes;
	while (off < size()) {
		bytes = process( off, _blockSize, _crcBuf, &MemoryData::doRead );
		if (bytes) {
			//	Check CRC
			calc = util::crc_calc( calc, _crcBuf, bytes );
		}
		off += bytes;
	}
	return calc == crc;
}

bool MemoryData::saveAs( const std::string &name ) {
	bool result=false;

	//	If output file exist, remove ...
	if (fs::exists( name )) {
		fs::remove( name );
	}

	FILE *file = fopen( name.c_str(), "wb" );
	if (file) {
		//  Write content to disk
		result = sendFile( file, 0, 0, size() ) == size();
		fclose( file );
	}
	return result;
}

void MemoryData::doRead( size_t block, size_t offset, size_t len, size_t processed, BYTE *buf ) {
    //  Get block
    util::Buffer *b = _blocks[block];
    memcpy( buf+processed, b->bytes()+offset, len );
}

void MemoryData::doWrite( size_t block, size_t offset, size_t len, size_t processed, BYTE *buf ) {
    //  Get first block
    Buffer *b = _blocks[block];
    if (!b) {
        //  Alloc buffer
        b = _pool->alloc();
        _blocks[block] = b;
    }
    b->copy( offset, buf+processed, len );
}

void MemoryData::doSendfile( size_t block, size_t offset, size_t len, size_t /*processed*/, FILE *f ) {
    //  Get block
    Buffer *b = _blocks[block];
    //  Write to file
    fwrite( b->bytes()+offset, 1, len, f );
}

}
}
