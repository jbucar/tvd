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
#include "../../size.h"
#include <util/log.h>
#include <SDL/SDL.h>


namespace canvas {
namespace sdl {

Canvas::Canvas()
	: canvas::Canvas()
{
	//	Setup screen
	SDL_Surface *videoMode = SDL_SetVideoMode(size.w, size.h, 0, SDL_SRCALPHA );
	DTV_ASSERT( videoMode );

	//	Change screen alpha
	_screen = new Surface( this, videoMode );
	SDL_Surface *tmp = SDL_DisplayFormatAlpha( videoMode );
	if (!tmp) {
		LERROR("sdl::Canvas", "Can't set display format alpha to canvas layer: %s", SDL_GetError());
		DTV_ASSERT( false );
	}

	//	Create layer
	_layer = new Surface( this, tmp );
	SDL_WM_SetCaption( "Ginga - Lifia", NULL );
}

Canvas::~Canvas( void )
{
	delete _layer;
	delete _screen;
}

std::string Canvas::name() {
	return "sdl";
}

canvas::Surface *Canvas::createSurfaceImpl( ImageData *img ) {
	// TODO:
	throw std::runtime_error( "Cannot create cairo sdl::Surface from image data" );
}

canvas::Surface *Canvas::createSurfaceImpl( const Rect &rect ) {
	return new Surface( this, rect );
}

canvas::Surface *Canvas::createSurfaceImpl( const std::string &file ) {
	return new Surface( this, file );
}

canvas::Surface *Canvas::getLayer() const {
	return _layer;
}

void Canvas::renderNow( void ) {
	_screen->::canvas::Surface::clear();
	const Size &size = _screen->getSize();
	Rect rect( 0, 0, size.w, size.h );
	_screen->blit( Point(0,0), _layer, rect );
	SDL_Flip( _screen->getContent() );
}

}
}
