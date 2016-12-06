/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "util.h"
#include "../../src/device.h"
#include "../../src/system.h"
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>

namespace util {

namespace fs = boost::filesystem;

std::string getDepot() {
		const char *root=getenv( "DEPOT" );
		return root ? root : "/";
	}

std::string getTestRoot() {
	fs::path rootPath(getDepot());
	rootPath /= "lib";
	rootPath /= "dtv-gingaplayer";
	rootPath /= "test";
	return rootPath.string();
}

std::string getImageName( const std::string &name ) {
	fs::path imagesPath( util::getTestRoot() );
	imagesPath /= "images";
	imagesPath /= name;
	return imagesPath.string();
}

std::string getExpectedPath( const std::string &name, const std::string &canvas="" ) {
	fs::path expectedPath( util::getTestRoot() );
	expectedPath /= "images";
	expectedPath /= "expected";
	if (!canvas.empty()) {
		expectedPath /= canvas;
	}
	expectedPath /= name;
	return expectedPath.string();
}

bool compareImagesFromPath( canvas::Canvas *c, const std::string &file ) {
	if (!fs::exists( file )) {
 		printf( "[util] Creating image file: %s\n", file.c_str() );
 		c->saveAsImage( file );
		return false;
	}
	return c->equalsImage( file );
}

bool compareImages( canvas::Canvas *c, const std::string &file ) {
	std::string tmp = getExpectedPath( file, c->name() ) + ".png";
	if (!fs::exists( tmp )) {
		tmp = getExpectedPath( file ) + ".png" ;
	}
 	return compareImagesFromPath( c, tmp );
}

}

