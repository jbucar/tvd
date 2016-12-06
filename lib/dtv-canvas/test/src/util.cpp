/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "util.h"
#include "../../src/canvas.h"
#include <stdio.h>
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
	rootPath /= "dtv-canvas";
	rootPath /= "test";
	return rootPath.string();
}

std::string getFontRoot() {
	fs::path rootPath(getDepot());
	rootPath /= "lib";
	rootPath /= "dtv-canvas";
	rootPath /= "test";
	rootPath /= "fonts";
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

bool compareImages( canvas::Canvas *c, const std::string &file ) {
	std::string tmp1 = getExpectedPath( file, c->name() ) + ".png";
	if (!fs::exists( tmp1 )) {
		std::string tmp2 = getExpectedPath( file ) + ".png";
		if (fs::exists( tmp2 )) {
			tmp1 = tmp2;
		} else {
			printf("[util] Saving image %s\n", tmp1.c_str());

			// Create directory where the file will be written
			boost::filesystem::path p(tmp1);
			boost::filesystem::path dir = p.parent_path();
			if ( !boost::filesystem::exists( dir ) ) {
				boost::filesystem::create_directories(dir);
			}

			if (!c->saveAsImage( tmp1 ) ) {
				printf("[util] Error saving image %s\n", tmp1.c_str());
			}
			return false;
		}
	}
	//	Compare
	return c->equalsImage( tmp1 );
}

}
