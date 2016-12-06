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

class SurfaceDrawLine : public Surface {
protected:
    std::string getSubDirectory() const { return "DrawLine"; }
};

// Draw multiples lines
TEST_F( SurfaceDrawLine, multiple ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));

	ASSERT_TRUE( s->drawLine( 10, 10, 30, 10) ); // Horizontal hacia derecha
	ASSERT_TRUE( s->drawLine( 30, 20, 10, 20) ); // Horizontal hacia izquierda

	ASSERT_TRUE( s->drawLine( 50, 10, 50, 20) ); // Vertical hacia abajo
	ASSERT_TRUE( s->drawLine( 70, 20, 70, 10) ); // Vertical hacia arriba

	ASSERT_TRUE( s->drawLine( 10, 30, 30, 50) ); // Diagonal hacia derecha/abajo
	ASSERT_TRUE( s->drawLine( 10, 90, 30, 70) ); // Diagonal hacia derecha/arriba

	ASSERT_TRUE( s->drawLine( 70, 30, 50, 50) ); // Diagonal hacia izquierda/abajo
	ASSERT_TRUE( s->drawLine( 70, 90, 50, 70) ); // Diagonal hacia izquierda/arriba

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("multiple" ) ) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line in the surface
TEST_F( SurfaceDrawLine, horizontal ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 310, 288, 410, 288 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("horizontal" ) ) );

	getCanvas()->destroy( s );
}

// Draw a vertical line in the surface
TEST_F( SurfaceDrawLine, vertical ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 360, 238, 360, 338 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("vertical" ) ) );

	getCanvas()->destroy( s );
}


// Draw a diagonal line in the surface
TEST_F( SurfaceDrawLine, diagonal ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 330, 260, 390, 320 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal" ) ) );

	getCanvas()->destroy( s );
}

// Draw a line from equal points in the surface. Must be a point.
TEST_F( SurfaceDrawLine, from_equal_points ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 330, 260, 330, 260 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("from_equal_points" ) ) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from the top left corner of the surface
TEST_F( SurfaceDrawLine, diagonal_top_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 0, 0, 85, 85) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal_top_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from the top right corner of the surface
TEST_F( SurfaceDrawLine, diagonal_top_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 719, 0, 634, 85) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal_top_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from the bottom right corner of the surface
TEST_F( SurfaceDrawLine, diagonal_bottom_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 719, 575, 634, 490) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal_bottom_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from the bottom left corner of the surface
TEST_F( SurfaceDrawLine, diagonal_bottom_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 0, 575, 85, 490) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal_bottom_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a vertical line on the left side of the surface
TEST_F( SurfaceDrawLine, vertical_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 0, 238, 0, 338) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("vertical_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a vertical line on the right side of the surface
TEST_F( SurfaceDrawLine, vertical_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 719, 238, 719, 338) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("vertical_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line on the top of the surface
TEST_F( SurfaceDrawLine, horizontal_top ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 310, 0, 410, 0) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("horizontal_top" ) ) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line on the bottom of the surface
TEST_F( SurfaceDrawLine, horizontal_bottom ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 310, 575, 410, 575) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("horizontal_bottom" ) ) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line from one point out of the surface to another point inside the surface (left)
TEST_F( SurfaceDrawLine, horizontal_one_point_out_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, 288, 100, 288) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line from one point out of the surface to another point inside the surface (right)
TEST_F( SurfaceDrawLine, horizontal_one_point_out_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 620, 288, 720, 288) );

	getCanvas()->destroy( s );
}

// Draw a vertical line from one point out of the surface to another point inside the surface (top)
TEST_F( SurfaceDrawLine, vertical_one_point_out_top ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 360, -1, 360, 100) );

	getCanvas()->destroy( s );
}

// Draw a vertical line from one point out of the surface to another point inside the surface (bottom)
TEST_F( SurfaceDrawLine, vertical_one_point_out_bottom ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 360, 476, 360, 576) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line from two points out of the surface that crosses the surface
TEST_F( SurfaceDrawLine, horizontal_two_points_out_crossing_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, 288, 720, 288) );

	getCanvas()->destroy( s );
}

// Draw a vertical line from two points out of the surface that crosses the surface
TEST_F( SurfaceDrawLine, vertical_two_points_out_crossing_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 360, -1, 360, 576) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from two points out of the surface that crosses the surface
TEST_F( SurfaceDrawLine, diagonal_two_points_out_crossing_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, -1, 720, 576) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line that is out of the surface (top)
TEST_F( SurfaceDrawLine, horizontal_completely_out_top ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 310, -1, 410, -1) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line that is out of the surface (bottom)
TEST_F( SurfaceDrawLine, horizontal_completely_out_bottom ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 310, 576, 410, 576) );

	getCanvas()->destroy( s );
}

// Draw a vertical line that is out of the surface (left)
TEST_F( SurfaceDrawLine, vertical_completely_out_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, 238, -1, 338) );

	getCanvas()->destroy( s );
}

// Draw a vertical line that is out of the surface (right)
TEST_F( SurfaceDrawLine, vertical_completely_out_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, 720, -1, 720) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from one point out of the surface (top-left)
TEST_F( SurfaceDrawLine, diagonal_one_point_out_top_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, -1, 85, 85) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from one point out of the surface (top-right)
TEST_F( SurfaceDrawLine, diagonal_one_point_out_top_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 720, -1, 635, 85) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from one point out of the surface (bottom-left)
TEST_F( SurfaceDrawLine, diagonal_one_point_out_bottom_left ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( -1, 576, 85, 491) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from one point out of the surface (bottom-right)
TEST_F( SurfaceDrawLine, diagonal_one_point_out_bottom_right ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->drawLine( 720, 576, 635, 491) );

	getCanvas()->destroy( s );
}

// Draw an horizontal line of the same width of the surface
TEST_F( SurfaceDrawLine, horizontal_same_width_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 0, 288, 719, 288) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("horizontal_same_width_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw a vertical line of the same height of the surface
TEST_F( SurfaceDrawLine, vertical_same_height_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 360, 0, 360, 575) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("vertical_same_height_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw a diagonal line from the top-left corner to the bottom-right corner
TEST_F( SurfaceDrawLine, diagonal_same_width_surface ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->drawLine( 0, 0, 719, 575) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("diagonal_same_size_surface" ) ) );

	getCanvas()->destroy( s );
}



// Draw a line with alpha on the middle of the surface .
TEST_F( SurfaceDrawLine, line_with_alpha_and_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    s->fillRect( rect );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,288,719,288 );
    s->setColor( canvas::Color(0, 255, 0, 128) );
    s->drawLine( 360,0,360,575 );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "line_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a line with alpha on the middle of the surface .
TEST_F( SurfaceDrawLine, line_with_alpha_and_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0) );
    s->drawLine( 360,0,360,575 );
    s->setColor( canvas::Color(255, 0, 0, 128) );
    s->drawLine( 0,288,719,288 );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "line_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent line over a solid background.
TEST_F( SurfaceDrawLine, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    s->fillRect( rect );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    ASSERT_TRUE( s->drawLine( 0, 288, 719, 288 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent line over a transparent background.
TEST_F( SurfaceDrawLine, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    ASSERT_TRUE( s->drawLine( 0, 288, 719, 288 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}

// Test markDirty after drawing a line
TEST_F( SurfaceDrawLine, markDirty_drawLine ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();

	s->drawLine(0,0,9,9);	   // Diagonal line
	s->drawLine(20,20,20,29);  // Vertical line
	s->drawLine(40,40,49,40);  // Horizontal line
	s->drawLine(60,60,60,60);  // Point

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 4 );

	canvas::Rect r1(0,0,10,10),
				 r2(20,20,1,10),
				 r3(40,40,10,1),
				 r4(60,60,1,1);

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );
	ASSERT_TRUE( dirtyRegions.at(1) == r2 );
	ASSERT_TRUE( dirtyRegions.at(2) == r3 );
	ASSERT_TRUE( dirtyRegions.at(3) == r4 );

	getCanvas()->destroy( s );
}
