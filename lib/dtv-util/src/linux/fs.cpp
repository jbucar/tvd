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

#include "../assert.h"
#include "../main.h"
#include "../log.h"
#include "../fs.h"
#include "generated/config.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace util {

bool getMountedFilesystems( std::set<std::string> &paths ) {
	const char *sysMounts[] = {
		"/sys",
		"/dev",
		"/proc",
		NULL
	};

	FILE *f = fopen("/proc/mounts", "r");
	if (!f) {
		LWARN( "fs", "Cannot open /proc/mounts" );
		return false;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, f)) != -1) {
		std::vector<std::string> tokens;
		boost::split( tokens, line, boost::is_any_of( " " ) );
		if (tokens.size() > 2) {
			const std::string &mount = tokens[1];
			size_t i=0;
			while (sysMounts[i]) {
				if (mount.compare( 0, strlen(sysMounts[i]), sysMounts[i] ) == 0) {
					//	skip sys filesystem
					break;
				}
				i++;
			}
			if (i >= (sizeof(sysMounts)/sizeof(const char *)) - 1) {
				paths.insert( mount );
			}
		}
	}

	free(line);
	fclose(f);
	return true;
}

namespace fs {
namespace impl {

namespace bfs = boost::filesystem;

const std::string home() {
	const char *envHome = getenv( "HOME" );
	DTV_ASSERT(envHome);
	return std::string(envHome);
}

const std::string ro_root() {
	return LINUX_INSTALL_PREFIX;
}

const std::string rw_root( const std::string &prefix ) {
	if (main::is_service()) {
		//	LINUX_INSTALL_PREFIX/prefix/{TOOLNAME}
		bfs::path tmp(LINUX_INSTALL_PREFIX);
		tmp /= "var";
		tmp /= prefix;
		tmp /= main::name();
		return tmp.string();
	}
	else {
		//	HOME/.{TOOLNAME}/{prefix}
		bfs::path tmp( home() );
		tmp /= "." + main::name();
		tmp /= prefix;
		return tmp.string();
	}
}

}	//	namespace impl
}	//	namespace fs
}	//	namespace util

