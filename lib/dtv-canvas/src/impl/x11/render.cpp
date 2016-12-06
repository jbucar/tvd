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

#include "render.h"
#include "window.h"
#include <util/log.h>

namespace canvas {
namespace x11 {

Render::Render( Window *win )
{
	_window = win;
	_renderInitialized = false;
}

Render::~Render()
{
}

//	Initialization
bool Render::initialize() {
	//	Initialize x11 extensions
	if (supportVideoOverlay()) {
		if (!_window->initXCompositeExtension()) {
			LERROR( "x11::render", "Cannot initialize window XComposite extension" );
			return false;
		}
		if (!_window->initXDamageExtension()) {
			LERROR( "x11::render", "Cannot initialize window XDamage extension" );
			return false;
		}
	}

	//	Initialize generic implementation
	if (!init()) {
		LERROR( "x11::render", "Cannot initialize render system" );
		return false;
	}

	//	Initialize render
	if (!initRender()) {
		LERROR( "x11::render", "Cannot initialize render" );
		return false;
	}

	return true;
}

void Render::finalize() {
	finRender();
	fin();
}

void Render::restartRender() {
	if (_renderInitialized) {
		finRender();
		initRender();
	}
}

bool Render::init() {
	return true;
}

void Render::fin() {
}

bool Render::initRender() {
	_renderInitialized = true;
	return true;
}

void Render::finRender() {
	_renderInitialized = false;
}

bool Render::supportVideoOverlay() const {
	return false;
}

//	Video methods
int Render::getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines ) {
	const Size &size = _window->size();	
	strcpy( chroma, "ARGB" );
	*width = size.w;
	*height = size.h;
	pitches[0] = size.w * 4;
	pitches[1] = pitches[2] = 0;
	lines  [0] = size.h;
	lines  [1] = lines  [2] = 0;
	return 1;
}

void Render::cleanup() {
}

void *Render::allocFrame( void ** /*pixels*/ ) {
	return NULL;
}

void Render::freeFrame( void * ) {
}

void Render::renderFrame( void * ) {
}

//	Getters
Window *Render::window() const {
	return _window;
}

::Display *Render::display() const {
	return _window->dpy();
}

::Window Render::win() const {
	return _window->win();
}

bool Render::isFullScreen() const {
	return _window->isFullScreen();
}

const Size &Render::size() const {
	return _window->size();
}

const Rect &Render::videoBounds() const {
	return _window->getVideoBounds();
}

Size Render::canvasSize() {
	return _window->canvasSize();
}

void *Render::visualInfo( Display * /*dpy*/ ) {
	return NULL;
}

}
}
