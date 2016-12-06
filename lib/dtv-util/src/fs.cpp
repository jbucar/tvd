/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV implementation.

    DTV is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV.

    DTV es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "fs.h"
#include "log.h"
#include "mcr.h"
#include "assert.h"
#include "main.h"
#include <boost/filesystem.hpp>
#include <string.h>
#include <stdlib.h>

namespace util {

namespace bfs = boost::filesystem;

static void createDirectory( const bfs::path &path ) {
	if (!bfs::create_directories( path )) {
		std::string err = "[util::fs] Cannot create directory: ";
		err += path.string();
		throw std::runtime_error( err );
	}
}

void removeDirectory( const std::string &dir ) {
	//	If directory exist ...
	if (bfs::exists( dir )) {
		//	Remove all content
		bfs::remove_all( dir );
	}
}

bool safeRemoveDirectory( const std::string &dir ) {
	try {
		removeDirectory( dir );
		return true;
	} catch (...) {
		LWARN("fs", "Cannot remove directory: %s", dir.c_str());
		return false;
	}
}

void cleanDirectory( const std::string &dir ) {
	//	Remove directory if exist ..
	removeDirectory( dir );

	//	create directory
	createDirectory( dir );
}

bool safeCleanDirectory( const std::string &dir ) {
	try {
		cleanDirectory( dir );
		return true;
	} catch (...) {
		LWARN("fs", "Cannot clean directory: %s", dir.c_str());
		return false;
	}
}

namespace fs {

namespace impl {

const std::string home();
const std::string ro_root();
const std::string rw_root( const std::string &prefix );

}

std::string make( const std::string &prefix, const std::string &name ) {
	bfs::path tmp(prefix);
	tmp /= name;
	return tmp.string();
}

std::string make( const std::string &p1, const std::string &p2, const std::string &p3 ) {
	bfs::path tmp(p1);
	tmp /= p2;
	tmp /= p3;
	return tmp.string();
}

const std::string home() {
	return impl::home();
}

const std::string installRootDir() {
	return impl::ro_root();
}

const std::string installDataDir() {
	DTV_ASSERT( !main::name().empty() );
	return make( installRootDir(), "share", util::main::name() );
}

const std::string sysConfDir() {
	return make( installRootDir(), "etc" );
}

const std::string stateDir() {
	return impl::rw_root( "lib" );
}

const std::string logDir() {
	return impl::rw_root( "log" );
}

const std::string tmpDir() {
	return impl::rw_root( "tmp" );
}

}	//	namespace fs
}

