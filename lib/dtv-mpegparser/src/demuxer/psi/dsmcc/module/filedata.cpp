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
#include "filedata.h"
#include "../../../../resourcemanager.h"
#include <util/functions.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/filesystem.hpp>

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#ifdef _WIN32
#	include <windows.h>
#elif defined(__ANDROID__) 
#	include <sys/vfs.h> 
#else
#	include <sys/statvfs.h>
#endif

#define BUFFER_SIZE 4096

namespace tuner {
namespace dsmcc {

namespace fs = boost::filesystem;

FileData::FileData( util::DWORD size )
	: ModuleData( size )
{
	_file = NULL;
}

FileData::~FileData( void )
{	
	if (_file) {
		fclose( _file );
		fs::remove( _name );
	}
}

//	Getters
bool FileData::checkDiskFree( const std::string &res ) {
	bool result=false;
#ifdef _WIN32
	ULARGE_INTEGER freeBytes;
	std::string path = res + "/";	//	Add trailing backslash
	result = GetDiskFreeSpaceEx( res.c_str(), &freeBytes, NULL, NULL ) == TRUE;
#elif defined(__ANDROID__)
	struct statfs st;
	if (::statfs( res.c_str(), &st ) == 0) {
		result=st.f_bfree*st.f_bsize > size();		
	}
#else
	struct statvfs st;
	if (::statvfs( res.c_str(), &st ) == 0) {
		result=st.f_bfree*st.f_bsize > size();
	}
#endif
	return result;
}

bool FileData::canStart( ResourceManager *resMgr ) {
	bool result=false;

	//	Check disk free
	if (checkDiskFree( resMgr->rootPath() )) {
		//	Try open temporary file
		DTV_ASSERT(!_file);
		_file = resMgr->openTempFileName( "module_%%%%%%%%", _name );
		result = _file ? true : false;
	}
	return result;
}

//	Operations
bool FileData::checkCRC32( util::DWORD crc ) {
	util::BYTE buf[BUFFER_SIZE];
	size_t r;
	util::DWORD calc = util::DWORD(-1);

	DTV_ASSERT(_file);
	::fseek( _file, 0, SEEK_SET );
	r=::fread( buf, 1, BUFFER_SIZE, _file );
	while (r > 0) {
		//	Check CRC
		calc = util::crc_calc( calc, buf, r );
		r=::fread( buf, 1, BUFFER_SIZE, _file );		
	}
	return calc == crc;
}

bool FileData::saveAs( const std::string &name ) {
	if (_file) {
		//	Close file ...
		fclose( _file );
		_file = NULL;

		try {
			//	If output file exist, remove ...
			if (fs::exists( name )) {
				fs::remove( name );
			}

			//	Rename temporary file to new name
			fs::rename( _name, name );
		} catch (...) {
			return false;
		}
		return true;
	}
	return false;
}

size_t FileData::read( size_t off, util::BYTE *buf, size_t len ) {
	DTV_ASSERT(_file);
	size_t bytes = 0;

	if (::fseek( _file, off, SEEK_SET ) == 0) {
		bytes = ::fread( buf, 1, len, _file );
		if (!bytes) {
			LERROR( "FileData", "read error: errno=%d, str=%s", errno, strerror(errno) );
		}
	}
	else {
		LERROR( "FileData", "fseek error: errno=%d, str=%s", errno, strerror(errno) );
		
	}
	return bytes;
}

size_t FileData::sendFile( FILE *of, size_t o_off, size_t i_off, size_t /*len*/ ) {
	char buf[BUFFER_SIZE];
	DTV_ASSERT(_file);

	::fseek( _file, i_off, SEEK_SET );
	::fseek( of, o_off, SEEK_SET );
	size_t r, bytes=0;
	r=::fread( buf, 1, BUFFER_SIZE, _file );
	while (r > 0) {
		bytes += ::fwrite( buf, 1, r, of );
		r=::fread( buf, 1, BUFFER_SIZE, _file );		
	}
	return bytes;
}

size_t FileData::write( size_t off, util::BYTE *buf, size_t len ) {
	DTV_ASSERT(_file);
	::fseek( _file, off, SEEK_SET );
	return ::fwrite( buf, 1, len, _file );
}

}
}

