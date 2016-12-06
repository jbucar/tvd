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

#include "window.h"
#include "../../src/window.h"
#include "../../src/system.h"
#include "../../src/canvas.h"
#include "../../src/screen.h"
#include "../../src/point.h"
#include "../../src/rect.h"
#include "../../src/types.h"
#include "../../src/impl/dummy/window.h"
#include <util/mcr.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <vector>

void Window::SetUp() {
	System::SetUp();
	win()->fullScreen(false);
	util::cfg::setValue<bool>("gui.window.fullscreen", false);
}

void Window::TearDown() {
	win()->fullScreen(false);
	System::TearDown();
}

canvas::Window *Window::win() {
	return System::getSystem()->window();
}

TEST_F( Window, constructor ) {
	ASSERT_FALSE( win()->isFullScreen() );
	ASSERT_FALSE( win()->isIconified() );

	canvas::Size size;
	size.h = util::cfg::getValue<int>("gui.window.size.height");
	size.w = util::cfg::getValue<int>("gui.window.size.width");
	ASSERT_TRUE( win()->size() == size );
}

TEST_F( Window, constructor_in_fullScreen ) {
	util::cfg::setValue<bool>("gui.window.fullscreen", true);
	canvas::dummy::Window *win = new canvas::dummy::Window();
	ASSERT_TRUE( win->initialize(System::getSystem()) );

	ASSERT_TRUE( win->isFullScreen() );
	ASSERT_FALSE( win->isIconified() );

	win->finalize();
	DEL( win );
}

TEST_F( Window, set_fullScreen_dinamicaly ) {
	ASSERT_FALSE( win()->isFullScreen() );
	ASSERT_FALSE( win()->isIconified() );

	canvas::Size size;
	size.h = util::cfg::getValue<int>("gui.window.size.height");
	size.w = util::cfg::getValue<int>("gui.window.size.width");
	ASSERT_TRUE( win()->size() == size );

	if (win()->supportFullScreen()) {
		win()->fullScreen( true );
		ASSERT_TRUE( win()->isFullScreen() );
	}
}

TEST_F( Window, set_unset_fullScreen_dinamicaly ) {
	ASSERT_FALSE( win()->isFullScreen() );
	ASSERT_FALSE( win()->isIconified() );

	canvas::Size size;
	size.h = util::cfg::getValue<int>("gui.window.size.height");
	size.w = util::cfg::getValue<int>("gui.window.size.width");
	ASSERT_TRUE( win()->size() == size );

	if (win()->supportFullScreen()) {
		win()->fullScreen( true );
		ASSERT_TRUE( win()->isFullScreen() );

		win()->fullScreen( false );
		ASSERT_FALSE( win()->isFullScreen() );
	}
}

TEST_F( Window, change_video_mode ) {
	canvas::Screen *scr = getSystem()->screen();
	canvas::Canvas *canvas = getSystem()->canvas();
	canvas::Rect canvasBounds( canvas::Point(0,0), canvas->size() );

	win()->fullScreen( true );
	canvas->flush();
	ASSERT_EQ( 0, canvas->dirtyRegions().size() );

	std::vector<canvas::connector::type> connectors = scr->supportedConnectors();
	BOOST_FOREACH( canvas::connector::type conn, connectors ) {
		std::vector<canvas::mode::type> modes = scr->supportedModes( conn );
		BOOST_FOREACH( canvas::mode::type mode, modes ) {
			canvas::mode::type currentMode = scr->mode();
			if (currentMode != mode) {
				const canvas::Size &winSize = win()->size();
				canvas::Mode nm = canvas::mode::get( mode );
				ASSERT_TRUE( scr->mode( conn, mode ) );

				const std::vector<canvas::Rect> &dirtyRects = canvas->dirtyRegions();
				if ((winSize.w!=nm.width) || (winSize.h!=nm.height)) {
					ASSERT_EQ( 1, dirtyRects.size() );
					ASSERT_EQ( canvasBounds.x, dirtyRects[0].x );
					ASSERT_EQ( canvasBounds.y, dirtyRects[0].y );
					ASSERT_EQ( canvasBounds.w, dirtyRects[0].w );
					ASSERT_EQ( canvasBounds.h, dirtyRects[0].h );
					canvas->flush();
				} else {
					ASSERT_EQ( 0, dirtyRects.size() );
				}
			}
		}
	}
}
