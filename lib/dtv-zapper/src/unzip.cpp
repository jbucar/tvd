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

#include "generated/config.h"
#if ZAPPER_LOGOS_USE_NETWORK
#	include <minizip/unzip.h>
#endif
#include <util/log.h>
#include <boost/filesystem.hpp>

#define BUFFER_SIZE 4096
#define FILENAME_SIZE 1024

namespace zapper {

namespace bfs = boost::filesystem;

#if ZAPPER_LOGOS_USE_NETWORK

bool unzipFile( const std::string &parent, const std::string &recvZipFile ) {
	//	Open zip file
	unzFile file = unzOpen( recvZipFile.c_str() );
	if (!file) {
		LWARN( "unzip", "Cannot open downloaded file: file=%s", recvZipFile.c_str() );
		return false;
	}
	LDEBUG( "unzip", "Zip file opened: file=%s", recvZipFile.c_str() );

	//	Go to first file inside ZIP container.
	int unzResult = unzGoToFirstFile(file);
	if(unzResult != UNZ_OK) {
		LWARN( "unzip", "Could not go to the first file inside ZIP containers" );
		unzClose( file );
		return false;
	}

	//	Extract all files inside the ZIP container.
	bool exit=false;
	while (!exit) {
		//	Get info about current file.
		unz_file_info fileInfo;
		char filename[ FILENAME_SIZE ];
		if (unzGetCurrentFileInfo(
				file,
				&fileInfo,
				filename,
				FILENAME_SIZE,
				NULL, 0, NULL, 0 ) != UNZ_OK )
		{
			LWARN( "unzip", "Could not read file info: file=%s", recvZipFile.c_str() );
			unzClose( file );
			return false;
		}
		LTRACE( "unzip", "File name=%s, flags=%x, method=%d, cSize=%d, size=%d, fnSize=%d",
			filename, fileInfo.flag, fileInfo.compression_method, fileInfo.compressed_size, fileInfo.uncompressed_size, fileInfo.size_filename );

		//	Setup filename
		bfs::path absName(parent);
		absName /= filename;

		if (!fileInfo.compressed_size && !fileInfo.uncompressed_size) {
			//	Entry is a directory, so create it.
			bool result;
			try {
				bfs::create_directories( absName );
				result=true;
			} catch (...) {
				result=false;
			}
			if (!result) {
				LWARN( "unzip", "Could not create directory: dir=%s", absName.string().c_str() );
				unzClose( file );
				return false;
			}
		}
		else {
			//	Entry is a file, so extract it.
			if (unzOpenCurrentFile( file ) != UNZ_OK) {
				LWARN( "unzip", "Could not open current zip file: file=%s", filename );
				unzClose( file );
				return false;
			}

			//	Open a file to write out the data.
			FILE *out = fopen( absName.string().c_str(), "wb" );
			if (!out) {
				LWARN( "unzip", "Could not open current zip file: file=%s", filename );
				unzCloseCurrentFile( file );
				unzClose( file );
				return false;
			}

			int bytes = UNZ_OK;
			char buf[BUFFER_SIZE];
			do {
				bytes = unzReadCurrentFile( file, buf, BUFFER_SIZE );
				if (bytes < 0) {
					LWARN( "unzip", "Could not read current zip file: file=%s", filename );
					unzCloseCurrentFile( file );
					unzClose( file );
					fclose(out);
					return false;
				}

				//	Write data to file.
				if (bytes > 0) {
					int bytesWritten = fwrite( buf, 1, bytes, out );
					if(bytesWritten != bytes) {
						LWARN( "unzip", "Could not write current zip file: file=%s", filename );
						unzCloseCurrentFile( file );
						unzClose( file );
						fclose(out);
						return false;
					}
				}
			} while (bytes > 0);

			//	Close current file
			fclose( out );
			unzCloseCurrentFile( file );
		}

		//	Go to the next file inside ZIP container.
		unzResult = unzGoToNextFile(file);
		if(unzResult == UNZ_END_OF_LIST_OF_FILE) {
			exit=true;
		}
		else if(unzResult != UNZ_OK) {
			LWARN( "unzip", "Failed to go to the next file inside ZIP containter" );
			unzClose( file );
			return false;
		}
	}

	unzClose( file );
	return true;
}

#else	// #if ZAPPER_LOGOS_USE_NETWORK

bool unzipFile( const std::string & /*parent*/, const std::string & /*recvZipFile*/ ) {
	LERROR( "unzip", "No implementation available" );
	return false;
}

#endif	// #if ZAPPER_LOGOS_USE_NETWORK

}
