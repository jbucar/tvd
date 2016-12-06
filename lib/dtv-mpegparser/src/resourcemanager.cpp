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
#include "resourcemanager.h"
#include <util/log.h>
#include <util/fs.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_REPEAT    5

namespace tuner {

namespace fs = boost::filesystem;

ResourceManager::ResourceManager( const std::string &root, int memoryBlocks, int maxModules, DWORD maxModuleSize )
{
	_root          = root;
	_memoryBlocks  = memoryBlocks;
	_maxModules    = maxModules;
	_maxModuleSize = maxModuleSize;

	LDEBUG("ResourceManager", "Initializing: root=%s, memBlocks=%d, maxModules=%d, maxModuleSize=%d",
		_root.c_str(), _memoryBlocks, _maxModules, _maxModuleSize);

	//	Clean temporary files
	clean();
}

ResourceManager::~ResourceManager( void )
{
}

//	Pathset
const std::string &ResourceManager::rootPath( void ) const {
	return _root;
}

std::string ResourceManager::applicationsPath( void ) const {
	fs::path root = _root;
	root /= "applications";
	return root.string();
}

std::string ResourceManager::downloadPath( void ) const {
	fs::path root = _root;
	root /= "download";
	return root.string();	
}

std::string ResourceManager::temporaryPath( void ) const {
	fs::path root = _root;
	root /= "tmp";
	return root.string();	
}

//	Utils
std::string ResourceManager::mkTempFileName( const std::string &templateFileName, bool useTempPath/*=true*/ ) const {
	std::string name;
	if (!templateFileName.empty()) {
		fs::path tmp;
		if (useTempPath) {
			tmp = temporaryPath();
		}
		tmp /= templateFileName;
		name = fs::unique_path(tmp).string();
	}
	return name;
}

FILE *ResourceManager::openTempFileName( const std::string &templateFileName, std::string &filename ) const {
	FILE *file = NULL;
	struct stat st;
	int iter=0;

	if (!templateFileName.empty()) {
		std::string name;
		fs::path tmp = temporaryPath();
		tmp /= templateFileName;
		while (!file && iter < MAX_REPEAT) {
			name = fs::unique_path(tmp).string();
			LDEBUG("ResourceManager", "openTempFileName: template=%s, result=%s", tmp.string().c_str(), name.c_str());
			if (stat(name.c_str(),&st) < 0) {
				//  If file not exist, open file
				file = fopen(name.c_str(),"w+b");
			}
			iter++;
		}
		if (file) {
			filename = name;
			LDEBUG("ResourceManager", "openTempFileName: result=%s", name.c_str());
		}
	}

    return file;
}

void ResourceManager::clean() {
	try {
		//	Clean root directory
		util::cleanDirectory( _root );
	
		//	Clean applications path
		util::cleanDirectory( applicationsPath() );

		//	Clean download path
		util::cleanDirectory( downloadPath() );

		//	Clean temporary path
		util::cleanDirectory( temporaryPath() );
	} catch ( ... ) {
		LWARN("ResourceManager", "cannot clean resources on %s", _root.c_str());
	}
}

//	Resources
int ResourceManager::memoryblocks() const {
	return _memoryBlocks;
}

int ResourceManager::maxModules() const {
	return _maxModules;
}

DWORD ResourceManager::maxModuleSize() const {
	return _maxModuleSize;
}

}
