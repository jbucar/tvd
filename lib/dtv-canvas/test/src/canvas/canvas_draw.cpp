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

#include "surface.h"
#include "../util.h"
#include "../../../src/canvas.h"
#include "../../../src/surface.h"

// Test flush when no surface was created
TEST_F( Surface, canvas_draw_basic_empty ) {
	getCanvas()->beginDraw();
	getCanvas()->endDraw();	
	ASSERT_TRUE( util::compareImages(getCanvas(), "nothing" ) );
}

// Test flush with one surface visible
TEST_F( Surface, canvas_draw_basic_ok ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	getCanvas()->beginDraw();
	ASSERT_FALSE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );
	getCanvas()->endDraw();
	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );	

	getCanvas()->destroy( s );
}

TEST_F( Surface, canvas_draw_recursive_ok ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	getCanvas()->beginDraw();
	getCanvas()->beginDraw();
	getCanvas()->beginDraw();
	ASSERT_FALSE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );
	getCanvas()->endDraw();
	ASSERT_FALSE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );
	getCanvas()->endDraw();
	ASSERT_FALSE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );
	getCanvas()->endDraw();	
	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );	

	getCanvas()->destroy( s );
}

