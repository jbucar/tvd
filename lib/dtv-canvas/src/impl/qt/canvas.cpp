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

#include "canvas.h"
#include "surface.h"
#include "font.h"
#include "../../size.h"
#include <util/log.h>
#include <util/mcr.h>
#include <QFontDatabase>
#include <boost/filesystem.hpp>

namespace canvas {
namespace qt {

namespace bfs = boost::filesystem;

Canvas::Canvas()
{
	_fontsDB = new QFontDatabase();
}

Canvas::~Canvas()
{
	DEL(_fontsDB);
}

QFontDatabase *Canvas::fontsDB() const {
	return _fontsDB;
}

void Canvas::addImpl( const std::string &dir, int maxDepth ) {
	bfs::directory_iterator end_itr;
	for (bfs::directory_iterator itr( dir ); itr != end_itr; ++itr ) {
		if (bfs::is_directory( itr->status() ) && 0 < maxDepth) {
			addImpl( itr->path().string(), maxDepth - 1 );
		}
		else if (bfs::is_regular_file( itr->status() )) {
			_fontsDB->addApplicationFont( itr->path().string().c_str() );
		}
	}
}

void Canvas::addFontDirectory( const std::string &dir ) {
	LDEBUG( "qt", "Add font directory: %s", dir.c_str() );
	addImpl( dir, 5 );
}

void Canvas::clearFontDirectories() {
	_fontsDB->removeAllApplicationFonts();
}

std::string Canvas::name() {
	return "qt";
}

canvas::Surface *Canvas::createSurfaceImpl( ImageData *img ) {
	return new Surface( this, img );
}

canvas::Surface *Canvas::createSurfaceImpl( const Rect &rect ) {
	return new Surface(this,rect);
}

canvas::Surface *Canvas::createSurfaceImpl( const std::string &file ) {
	return new Surface(this,file);	
}

canvas::Font *Canvas::createFont( const std::string &filename, size_t size ) {
	return new Font( filename, size );
}

}
}

