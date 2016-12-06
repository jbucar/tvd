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

class SurfaceClip : public Surface {
protected:
    std::string getSubDirectory() const { return "Clip"; }
};

// Set a clipping area in the surface and then draw a filled rect (middle)
TEST_F( SurfaceClip, basic_middle ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(310, 238, 100, 100)));
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_middle" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface and then draw a filled rect (top_left)
TEST_F( SurfaceClip, basic_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(0, 0, 100, 100)));
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_top_left" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface and then draw a filled rect (top_right)
TEST_F( SurfaceClip, basic_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(620, 0, 100, 100)));
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_top_right" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface and then draw a filled rect (bottom_left)
TEST_F( SurfaceClip, basic_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(0, 476, 100, 100)));
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_bottom_left" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface and then draw a filled rect (bottom_right)
TEST_F( SurfaceClip, basic_bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush( true );
	ASSERT_TRUE( s->setClip(canvas::Rect(620, 476, 100, 100)) );
	s->setColor( canvas::Color(0, 255, 0) );
	s->fillRect( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_bottom_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw a filled rect in the surface and then set a clipping area
TEST_F( SurfaceClip, basic_draw_diagonal_line_before_clipping ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );
	ASSERT_TRUE(s->setClip(canvas::Rect(310, 238, 100, 100)));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "fillRect_before_clipping" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface with color red and then draw a diagonal line
TEST_F( SurfaceClip, set_color_after_clipping ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(310, 238, 100, 100)));
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRect(canvas::Rect(0,0,720,576)) );
	s->setColor(canvas::Color(0, 255, 0));
	s->drawLine( 0, 0, 719, 575 );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "set_color_after_clipping" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (bottom right)
TEST_F( SurfaceClip, outside_bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(720, 576, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (bottom left)
TEST_F( SurfaceClip, outside_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(-100, 576, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (top left)
TEST_F( SurfaceClip, outside_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(-100, -100, 100, 100)) );

	getCanvas()->destroy( s );
}
// Set a clipping area outside the surface. (top right)
TEST_F( SurfaceClip, outside_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(720, -100, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (right)
TEST_F( SurfaceClip, outside_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(720, 238, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (bottom)
TEST_F( SurfaceClip, outside_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(310, 576, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (left)
TEST_F( SurfaceClip, outside_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(-100, 238, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area outside the surface. (top)
TEST_F( SurfaceClip, outside_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(310, -100, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with some part outside the surface.
TEST_F( SurfaceClip, some_part_outside_bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(621, 477, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with some part outside the surface.
TEST_F( SurfaceClip, some_part_outside_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(-1, -1, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with some part outside the surface.
TEST_F( SurfaceClip, some_part_outside_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(-1, 477, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with some part outside the surface.
TEST_F( SurfaceClip, some_part_outside_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(621, -1, 100, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area in the surface and draw a filled rect. Then change the clipping area
// and draw a diagonal line
TEST_F( SurfaceClip, change ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE(s->setClip(canvas::Rect(310, 238, 100, 100)));
	s->setColor(canvas::Color(0, 255, 0));
	s->fillRect( canvas::Rect(0, 0, 720, 576) );
	ASSERT_TRUE(s->setClip(canvas::Rect(0, 0, 720, 576)));
	s->setColor(canvas::Color(255, 0, 0));
	s->drawLine(0,0,719,575);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "change" ) ) );

	getCanvas()->destroy( s );
}

// Set a clipping area greater than surface
TEST_F( SurfaceClip, area_gt_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(0, 0, 721, 577)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with width greater than surface
TEST_F( SurfaceClip, area_with_gt_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(0, 0, 721, 100)) );

	getCanvas()->destroy( s );
}

// Set a clipping area with height greater than surface
TEST_F( SurfaceClip, area_height_gt_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(0, 0, 100, 577)) );

	getCanvas()->destroy( s );
}

// Set a clipping area of width 0 and height 0
TEST_F( SurfaceClip, area_width_and_height_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(100, 100, 0, 0)) );

	getCanvas()->destroy( s );
}

// Set a clipping area of width 0
TEST_F( SurfaceClip, area_width_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(100, 100, 0, 10)) );

	getCanvas()->destroy( s );
}

// Set a clipping area of height 0
TEST_F( SurfaceClip, area_height_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_FALSE(s->setClip(canvas::Rect(100, 100, 10, 0)) );

	getCanvas()->destroy( s );
}

// Test getClip
TEST_F( SurfaceClip, get_clip ) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    canvas::Rect r1(310, 238, 100, 100);
    canvas::Rect r2;

    ASSERT_TRUE(s->setClip(r1));
    ASSERT_TRUE(s->getClip(r2));

    printf("[Surface Clip] x: %d y: %d w: %d h: %d\n", r2.x, r2.y, r2.w, r2.h);
    ASSERT_TRUE( r1 == r2 );

    getCanvas()->destroy( s );
}

// Test getClip
TEST_F( SurfaceClip, get_clip_after_reclipping_bigger ) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    canvas::Rect r1(310, 238, 100, 100);
    canvas::Rect r2(300, 200, 200, 200);
    canvas::Rect r3;

    ASSERT_TRUE(s->setClip(r1));
    ASSERT_TRUE(s->setClip(r2));
    ASSERT_TRUE(s->getClip(r3));

    printf("[Surface Clip] x: %d y: %d w: %d h: %d\n", r2.x, r2.y, r2.w, r2.h);
    ASSERT_TRUE( r2 == r3 );

    getCanvas()->destroy( s );
}

// Test getClip
TEST_F( SurfaceClip, get_clip_after_reclipping_smaller ) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    canvas::Rect r1(310, 238, 100, 100);
    canvas::Rect r2(320, 240, 60, 60);
    canvas::Rect r3;

    ASSERT_TRUE(s->setClip(r1));
    ASSERT_TRUE(s->setClip(r2));
    ASSERT_TRUE(s->getClip(r3));

    printf("[Surface Clip] x: %d y: %d w: %d h: %d\n", r2.x, r2.y, r2.w, r2.h);
    ASSERT_TRUE( r2 == r3 );

    getCanvas()->destroy( s );
}
