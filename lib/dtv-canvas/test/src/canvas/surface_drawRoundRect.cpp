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

class SurfaceDrawRoundRect : public Surface {
protected:
    std::string getSubDirectory() const { return "DrawRoundRect"; }
};

// Draw a rounded rectangle on the middle of the surface
TEST_F( SurfaceDrawRoundRect, middle ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 263, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle on the top left corner of the surface
TEST_F( SurfaceDrawRoundRect, top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(0, 0, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle on the top right corner of the surface
TEST_F( SurfaceDrawRoundRect, top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(620, 0, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_rigth" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle on the bottom right corner of the surface
TEST_F( SurfaceDrawRoundRect, bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(620, 526, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_rigth" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle on the bottom left corner of the surface
TEST_F( SurfaceDrawRoundRect, bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(0, 526, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with the same height of the surface
TEST_F( SurfaceDrawRoundRect, same_height_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 0, 100, 576), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_height_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with the same width of the surface
TEST_F( SurfaceDrawRoundRect, same_width_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(0, 238, 720, 100), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_width_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with the same size of the surface
TEST_F( SurfaceDrawRoundRect, same_size_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(0, 0, 720, 576), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_size_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW = 0
TEST_F( SurfaceDrawRoundRect, arcW_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 0, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcW_0" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcH = 0
TEST_F( SurfaceDrawRoundRect, arcH_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 10, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcH_0" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW = 0 and arcH = 0
TEST_F( SurfaceDrawRoundRect, arcW_and_arcH_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcW_and_arcH_0" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded square with arcW = w/2 and arcH = h/2. Must be a circle.
TEST_F( SurfaceDrawRoundRect, arcW_and_arcH_half_w_h ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 100), 50, 50 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcW_arcH_half_w_h" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW > w/2
TEST_F( SurfaceDrawRoundRect, arcW_gt_w_div_2 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 60, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcW_gt_w_div_2" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcH > h/2
TEST_F( SurfaceDrawRoundRect, arcH_gt_h_div_2 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 10, 35 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcH_gt_h_div_2" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW > w.
TEST_F( SurfaceDrawRoundRect, arcW_gt_w ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 200, 25 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcW_gt_w" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcH > h.
TEST_F( SurfaceDrawRoundRect, arcH_gt_h ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 50, 100 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcH_gt_h" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcH > h and arcW > w.
TEST_F( SurfaceDrawRoundRect, arcH_gt_h_and_arcW_gt_w ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 200, 100 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "arcH_gt_h_&_arcW_gt_w" ) ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with w = 0.
TEST_F( SurfaceDrawRoundRect, w_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 0, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with h = 0.
TEST_F( SurfaceDrawRoundRect, h_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 100, 0), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with h = 0 and w = 0.
TEST_F( SurfaceDrawRoundRect, h_0_and_w_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawRoundRect( canvas::Rect(310, 260, 0, 0), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle completely out of the surface (top)
TEST_F( SurfaceDrawRoundRect, completely_out_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, -50, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle completely out of the surface (bottom)
TEST_F( SurfaceDrawRoundRect, completely_out_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle completely out of the surface (left)
TEST_F( SurfaceDrawRoundRect, completely_out_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(-100, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle completely out of the surface (right)
TEST_F( SurfaceDrawRoundRect, completely_out_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(720, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with two vertices out of the surface (top)
TEST_F( SurfaceDrawRoundRect, two_vertices_out_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, -1, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with two vertices out of the surface (bottom)
TEST_F( SurfaceDrawRoundRect, two_vertices_out_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 527, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with two vertices out of the surface (left)
TEST_F( SurfaceDrawRoundRect, two_vertices_out_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(-1, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with two vertices out of the surface (right)
TEST_F( SurfaceDrawRoundRect, two_vertices_out ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(621, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with three vertices out of the surface (top-left)
TEST_F( SurfaceDrawRoundRect, three_vertices_out_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(-1, -1, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with three vertices out of the surface (top-right)
TEST_F( SurfaceDrawRoundRect, three_vertices_out_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(621, 0, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with three vertices out of the surface (bottom-left)
TEST_F( SurfaceDrawRoundRect, three_vertices_out_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(-1, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with three vertices out of the surface (bottom-right)
TEST_F( SurfaceDrawRoundRect, three_vertices_out_bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(621, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with four vertices out of the surface that crosses the surface horizontally
TEST_F( SurfaceDrawRoundRect, four_vertices_out_crossing_surface_horizontally ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(-1, 238, 722, 100), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with four vertices out of the surface that crosses the surface vertically
TEST_F( SurfaceDrawRoundRect, four_vertices_out_crossing_surface_vertically ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, -1, 100, 578), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with w < 0.
TEST_F( SurfaceDrawRoundRect, w_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, -1, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with h < 0.
TEST_F( SurfaceDrawRoundRect, h_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, 100, -1), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with h < 0 and w < 0.
TEST_F( SurfaceDrawRoundRect, h_lt_0_and_w_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, -1, -1), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW < 0
TEST_F( SurfaceDrawRoundRect, arcW_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), -1, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcH < 0
TEST_F( SurfaceDrawRoundRect, arcH_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), 10, -1 ) );

	getCanvas()->destroy( s );
}

// Draw a rounded rectangle with arcW < 0 and arcH < 0
TEST_F( SurfaceDrawRoundRect, arcW_and_arcH_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawRoundRect( canvas::Rect(310, 260, 100, 50), -1, -1 ) );

	getCanvas()->destroy( s );
}

// Draw a round rect with alpha on the middle of the surface .
TEST_F( SurfaceDrawRoundRect, round_rect_with_alpha_and_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    s->fillRect( rect );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 128) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 128) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "round_rect_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a round rect with alpha on the middle of the surface .
TEST_F( SurfaceDrawRoundRect, round_rect_with_alpha_and_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 128) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 128) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "round_rect_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent round rect over a solid background.
TEST_F( SurfaceDrawRoundRect, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    s->fillRect( rect );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent round rect over a transparent background.
TEST_F( SurfaceDrawRoundRect, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}

// Test markDirty after drawing a round rect
TEST_F( SurfaceDrawRoundRect, markDirty_drawRoundRect ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();

	canvas::Rect r1(0,0,1,1),
				 r2(10,10,10,5),
				 r3(30,30,5,10),
				 r4(50,50,10,1),
				 r5(70,70,1,10),
				 r6(90,10,10,5),
				 r7(110,30,5,10),
				 r8(130,50,10,1),
				 r9(150,70,1,10);

	s->drawRoundRect( r1, 0, 0 );   // Point
	s->drawRoundRect( r2, 0, 0 );   // Rect with w > h
	s->drawRoundRect( r3, 0, 0 );   // Rect with h > w
	s->drawRoundRect( r4, 0, 0 );   // Rect with h = 1
	s->drawRoundRect( r5, 0, 0 );   // Rect with w = 1
	s->drawRoundRect( r6, 10, 10 ); // Rect with w > h with arcW and arcH
	s->drawRoundRect( r7, 10, 10 ); // Rect with h > w with arcW and arcH
	s->drawRoundRect( r8, 10, 10 ); // Rect with h = 1 with arcW and arcH
	s->drawRoundRect( r9, 10, 10 ); // Rect with w = 1 with arcW and arcH

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 9 );

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );
	ASSERT_TRUE( dirtyRegions.at(1) == r2 );
	ASSERT_TRUE( dirtyRegions.at(2) == r3 );
	ASSERT_TRUE( dirtyRegions.at(3) == r4 );
	ASSERT_TRUE( dirtyRegions.at(4) == r5 );
	ASSERT_TRUE( dirtyRegions.at(5) == r6 );
	ASSERT_TRUE( dirtyRegions.at(6) == r7 );
	ASSERT_TRUE( dirtyRegions.at(7) == r8 );
	ASSERT_TRUE( dirtyRegions.at(8) == r9 );

	getCanvas()->destroy( s );
}
