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
TEST_F( Surface, canvas_flush ) {
	getCanvas()->flush();

	ASSERT_TRUE( util::compareImages(getCanvas(), "nothing" ) );
}

// Test flush between begin and end draw
TEST_F( Surface, canvas_begin_end_draw ) {
	canvas::Surface *s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	getCanvas()->beginDraw();

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	getCanvas()->flush();
	ASSERT_TRUE( util::compareImages(getCanvas(), "nothing" ) );

	getCanvas()->endDraw();
	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );

	getCanvas()->destroy( s );
}

// Test flush between begin and end draw
TEST_F( Surface, canvas_begin_end_draw_2 ) {
	canvas::Surface *s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	getCanvas()->beginDraw();
	getCanvas()->beginDraw();

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	getCanvas()->flush();
	ASSERT_TRUE( util::compareImages(getCanvas(), "nothing" ) );

	getCanvas()->endDraw();
	ASSERT_TRUE( util::compareImages(getCanvas(), "nothing" ) );

	getCanvas()->endDraw();
	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );

	getCanvas()->destroy( s );
}

// Test flush with one surface visible
TEST_F( Surface, canvas_flush_visible ) {
	canvas::Surface *s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/flush_with_surface_visible" ) );

	getCanvas()->destroy( s );
}

// Test flush with 17 surfaces with same zIndex. Some sorting algorithms (such as std::sort) does not guarantee the relative order with equal elements
TEST_F( Surface, canvas_flush_with_same_ZIndex ) {
	canvas::Surface *s1 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s2 = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );
	canvas::Surface *s3 = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );
	canvas::Surface *s4 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s5 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s6 = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );
	canvas::Surface *s7 = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );
	canvas::Surface *s8 = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );
	canvas::Surface *s9 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s10 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s11 = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );
	canvas::Surface *s12 = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );
	canvas::Surface *s13 = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );
	canvas::Surface *s14 = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );
	canvas::Surface *s15 = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );
	canvas::Surface *s16 = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );
	canvas::Surface *s17 = getCanvas()->createSurfaceFromPath( util::getImageName("green.jpg") );

	s1->autoFlush(true);
	s2->autoFlush(true);
	s3->autoFlush(true);
	s4->autoFlush(true);
	s5->autoFlush(true);
	s6->autoFlush(true);
	s7->autoFlush(true);
	s8->autoFlush(true);
	s9->autoFlush(true);
	s10->autoFlush(true);
	s11->autoFlush(true);
	s12->autoFlush(true);
	s13->autoFlush(true);
	s14->autoFlush(true);
	s15->autoFlush(true);
	s16->autoFlush(true);
	s17->autoFlush(true);

	getCanvas()->flush();

	ASSERT_TRUE( util::compareImages( getCanvas(), "canvas_flush/canvas_flush_with_same_ZIndex" ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
	getCanvas()->destroy( s4 );
	getCanvas()->destroy( s5 );
	getCanvas()->destroy( s6 );
	getCanvas()->destroy( s7 );
	getCanvas()->destroy( s8 );
	getCanvas()->destroy( s9 );
	getCanvas()->destroy( s10 );
	getCanvas()->destroy( s11 );
	getCanvas()->destroy( s12 );
	getCanvas()->destroy( s13 );
	getCanvas()->destroy( s14 );
	getCanvas()->destroy( s15 );
	getCanvas()->destroy( s16 );
	getCanvas()->destroy( s17 );
}

// Test flush with one surface invisible
TEST_F( Surface, canvas_flush_invisible ) {
	canvas::Surface *s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setVisible(false);

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

