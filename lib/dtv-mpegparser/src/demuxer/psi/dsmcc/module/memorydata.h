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
#pragma once

#include "moduledata.h"
#include <vector>

namespace util {
	class Buffer;
}

namespace tuner {
namespace dsmcc {

class ResourcePool;

class MemoryData : public ModuleData {
public:
	explicit MemoryData( DWORD size );
	virtual ~MemoryData( void );

	static void initialize( DWORD maxBlocks, DWORD blockSize );
	static void cleanup();
	static bool canDownload( DWORD moduleSize );

	//	Getters
	virtual bool canStart( ResourceManager *resMgr );	

	//	Operations
	virtual bool checkCRC32( DWORD crc );
	virtual bool saveAs( const std::string &name );	
	virtual size_t read( size_t off, BYTE *buf, size_t len );
	virtual size_t sendFile( FILE *of, size_t o_off, size_t i_off, size_t len );	
	virtual size_t write( size_t off, BYTE *buf, size_t len );
	
protected:
	DWORD blockSize() const;

	//  Function to process the buffers
	template<typename T>
		size_t process( size_t off, size_t len, T param, void (MemoryData::*fnc)( size_t block, size_t offset, size_t len, size_t processed, T param ) );

	//  Aux to buffer methods
	void doSendfile( size_t block, size_t offset, size_t len, size_t processed, FILE *f );
	void doRead( size_t block, size_t offset, size_t len, size_t processed, BYTE *buf );
	void doWrite( size_t block, size_t offset, size_t len, size_t processed, BYTE *buf );
	
private:
	bool _resourcesLocked;	
	std::vector<util::Buffer *> _blocks;
	DWORD _blockSize;
	util::BYTE *_crcBuf;
	static ResourcePool *_pool;
};

template<typename T>
inline size_t MemoryData::process(
	size_t off,
	size_t len,
	T param,
	void (MemoryData::*fnc)( size_t block, size_t offset, size_t len, size_t processed, T param ) )
{
    size_t processed=0;
	DWORD s = size();
	
    //  Check overflow
	if (off < s) {
		size_t bytes = (len > s-off) ? s-off : len;

		//  Calculate the first block, offset
		size_t block = off / _blockSize;
		size_t offset = off % _blockSize;

		size_t copy;
		while (bytes) {
			//  Calculate the bytes to process
			copy = bytes > _blockSize-offset ? _blockSize-offset : bytes;

			(this->*fnc)( block, offset, copy, processed, param );

			bytes -= copy;
			processed += copy;
			block++;
			offset = 0;
		}
	}
	return processed;
}

}
}
