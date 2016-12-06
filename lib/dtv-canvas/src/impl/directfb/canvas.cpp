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
#include "dfb.h"
#include "font.h"

namespace canvas {
namespace directfb {

Canvas::Canvas( IDirectFB *dfb )
	: _dfb(dfb)
{
}

Canvas::~Canvas()
{
	_dfb = NULL;
}

std::string Canvas::name() {
	return "directfb";
}

canvas::Surface *Canvas::createSurfaceImpl( ImageData *img ) {
	return new Surface(this, img);
}

canvas::Surface *Canvas::createSurfaceImpl( const Rect &rect ) {
	return new Surface(this, rect, false);
}

canvas::Surface *Canvas::createSurfaceImpl( const std::string &file ) {
	return new Surface(this, file);
}

canvas::Font *Canvas::createFont( const std::string &filename, size_t size ) {
	return new Font(filename, size, _dfb);
}

IDirectFB *Canvas::getDFB() const {
	return _dfb;
}

}
}
