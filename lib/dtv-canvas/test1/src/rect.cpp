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

#include <gtest/gtest.h>
#include "../../src/rect.h"
#include "../../src/point.h"
#include "../../src/size.h"

TEST( rect, default_constructor ) {
    canvas::Rect rect;

    ASSERT_TRUE( rect.x == 0 );
    ASSERT_TRUE( rect.y == 0 );
    ASSERT_TRUE( rect.w == 0 );
    ASSERT_TRUE( rect.h == 0 );
}

TEST( rect, constructor_coord ) {
    canvas::Rect rect(15, 10, 28, 24);

    ASSERT_TRUE( rect.x == 15 );
    ASSERT_TRUE( rect.y == 10 );
    ASSERT_TRUE( rect.w == 28 );
    ASSERT_TRUE( rect.h == 24 );
}

TEST( rect, constructor_point_size ) {
    canvas::Point point(25, 4);
    canvas::Size size(10, 12);
    canvas::Rect rect(point, size);

    ASSERT_TRUE( rect.x == 25 );
    ASSERT_TRUE( rect.y == 4 );
    ASSERT_TRUE( rect.w == 10 );
    ASSERT_TRUE( rect.h == 12 );
}

TEST( rect, constructor_point_coord ) {
    canvas::Point point(25, 4);
    canvas::Rect rect(point, 10, 12);

    ASSERT_TRUE( rect.x == 25 );
    ASSERT_TRUE( rect.y == 4 );
    ASSERT_TRUE( rect.w == 10 );
    ASSERT_TRUE( rect.h == 12 );
}

TEST( rect, constructor_coord_size ) {
    canvas::Size size(10, 12);
    canvas::Rect rect(25,4,size);

    ASSERT_TRUE( rect.x == 25 );
    ASSERT_TRUE( rect.y == 4 );
    ASSERT_TRUE( rect.w == 10 );
    ASSERT_TRUE( rect.h == 12 );
}


// Two rects not initialized have to be equals
TEST( rect, equal_compare_default){
    canvas::Rect r1,r2;

    ASSERT_TRUE( r1 == r2);
    ASSERT_FALSE( r1 != r2);
}

//Two rects initialized with same values have to be equals
TEST( rect, equal_compare_with_value){
    canvas::Rect r1(50,50,100,100),r2(50,50,100,100);

    ASSERT_TRUE( r1 == r2 );
    ASSERT_FALSE( r1 != r2);
}

//Two rects initialized with differents value have not to be equals
TEST( rect, not_equal_compare){
    canvas::Rect r1(50,50,100,100), r2(55,55,100,110);

    ASSERT_TRUE( r1 != r2);
    ASSERT_FALSE( r1 == r2);
}

// Tests empty from a not empty rectangle
TEST( rect, empty) {
	canvas::Rect r(0,0,1,1);

	ASSERT_FALSE( r.isEmpty());
}

// Tests empty from a rectangle of width 0
TEST( rect, empty_width_0) {
	canvas::Rect r(0,0,0,1);

	ASSERT_TRUE( r.isEmpty());
}

// Tests empty from a rectangle of height 0
TEST( rect, empty_height_0) {
	canvas::Rect r(0,0,1,0);

	ASSERT_TRUE( r.isEmpty());
}

// Tests empty from a rectangle of height 0 and width 0
TEST( rect, empty_height_and_width_0) {
	canvas::Rect r(0,0,0,0);

	ASSERT_TRUE( r.isEmpty());
}

// Tests includes from equal rectangles
TEST( rect, includes_equal_rects ) {
	canvas::Rect r(10,10,10,10);

	ASSERT_TRUE(r.includes(r));
}

// Tests includes from different rectangles
TEST( rect, includes_different_rects_1 ) {
	canvas::Rect r1(10,10,10,10);
	canvas::Rect r2(10,10,9,9);

	ASSERT_TRUE(r1.includes(r2));
	ASSERT_FALSE(r2.includes(r1));
}

// Tests includes from different rectangles
TEST( rect, includes_different_rects_2 ) {
	canvas::Rect r1(10,10,10,10);
	canvas::Rect r2(11,11,9,9);

	ASSERT_TRUE(r1.includes(r2));
	ASSERT_FALSE(r2.includes(r1));
}

// Tests includes from different rectangles
TEST( rect, includes_different_rects_3 ) {
	canvas::Rect r1(10,10,10,10);
	canvas::Rect r2(10,11,9,9);

	ASSERT_TRUE(r1.includes(r2));
	ASSERT_FALSE(r2.includes(r1));
}

// Tests includes from different rectangles
TEST( rect, includes_different_rects_4 ) {
	canvas::Rect r1(10,10,10,10);
	canvas::Rect r2(11,10,9,9);

	ASSERT_TRUE(r1.includes(r2));
	ASSERT_FALSE(r2.includes(r1));
}

// Tests includes from different rectangles
TEST( rect, includes_different_rects_5 ) {
	canvas::Rect r1(10,10,10,10);
	canvas::Rect r2(12,12,6,6);

	ASSERT_TRUE(r1.includes(r2));
	ASSERT_FALSE(r2.includes(r1));
}

// Tests intersects from equal rectangles
TEST( rect, intersects_equal_rects ) {
    canvas::Rect r( 0,  0, 100, 100);

    ASSERT_TRUE( r.intersects( r ) );
}

// Tests intersects from different rectangles
TEST( rect, intersects_different_rects ) {
    canvas::Rect r1(100, 100, 100, 100);
    canvas::Rect r2(50, 50,  100,  100);

    ASSERT_TRUE( r1.intersects( r2 ) );
    ASSERT_TRUE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (top-bottom)
TEST( rect, not_intersects_top_bottom ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(0, 100,  100,  100);

    ASSERT_FALSE( r1.intersects( r2 ) );
    ASSERT_FALSE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (left-right)
TEST( rect, not_intersects_left_right ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(100, 0, 100, 100);

    ASSERT_FALSE( r1.intersects( r2 ) );
    ASSERT_FALSE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (diagonal-down)
TEST( rect, not_intersects_diagonal_down ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(100, 100, 100, 100);

    ASSERT_FALSE( r1.intersects( r2 ) );
    ASSERT_FALSE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (diagonal-up)
TEST( rect, not_intersects_diagonal_up ) {
    canvas::Rect r1(100, 0, 100, 100);
    canvas::Rect r2(0, 100, 100, 100);

    ASSERT_FALSE( r1.intersects( r2 ) );
    ASSERT_FALSE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (top-bottom)
TEST( rect, intersects_top_bottom ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(0, 99,  100,  100);

    ASSERT_TRUE( r1.intersects( r2 ) );
    ASSERT_TRUE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (left-right)
TEST( rect, intersects_left_right ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(99, 0, 100, 100);

    ASSERT_TRUE( r1.intersects( r2 ) );
    ASSERT_TRUE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (diagonal-down)
TEST( rect, intersects_diagonal_down ) {
    canvas::Rect r1(0, 0, 100, 100);
    canvas::Rect r2(99, 99, 100, 100);

    ASSERT_TRUE( r1.intersects( r2 ) );
    ASSERT_TRUE( r2.intersects( r1 ) );
}

// Tests intersects from different rectangles (diagonal-up)
TEST( rect, intersects_diagonal_up ) {
    canvas::Rect r1(99, 0, 100, 100);
    canvas::Rect r2(0, 99, 100, 100);

    ASSERT_TRUE( r1.intersects( r2 ) );
    ASSERT_TRUE( r2.intersects( r1 ) );
}

// Tests intersection from equal rects
TEST( rect, intersection_equal_rects ) {
    canvas::Rect r( 0,  0, 100, 100);
    canvas::Rect result;

    ASSERT_TRUE( r.intersection( r, result) );

    ASSERT_TRUE( result == r );
}

// Tests intersection from different rects
TEST( rect, intersection_different_rects ) {
    canvas::Rect r1( 0,  0, 100, 100);
    canvas::Rect r2( 50, 50, 100, 100);
    canvas::Rect expected(50,50,50,50);
    canvas::Rect result;

    ASSERT_TRUE( r1.intersection( r2, result ));
    ASSERT_TRUE( result == expected );
    ASSERT_TRUE( r2.intersection( r1, result ));
    ASSERT_TRUE( result == expected );
}

// Tests intersection from different rects (top-bottom)
TEST( rect, intersection_different_rects_top_bottom ) {
    canvas::Rect r1( 0,  0, 100, 100);
    canvas::Rect r2( 0, 99, 100, 100);
    canvas::Rect expected(0,99,100,1);
    canvas::Rect result;

    r1.intersection( r2, result );
    ASSERT_TRUE( result == expected );
    r2.intersection( r1, result );
    ASSERT_TRUE( result == expected );
}

// Tests intersection from different rects (left-right)
TEST( rect, intersection_different_rects_left_right ) {
    canvas::Rect r1( 0,  0, 100, 100);
    canvas::Rect r2( 99, 0, 100, 100);
    canvas::Rect expected(99,0,1,100);
    canvas::Rect result;

    r1.intersection( r2, result );
    ASSERT_TRUE( result == expected );
    r2.intersection( r1, result );
    ASSERT_TRUE( result == expected );
}

// Tests intersection from different rects (diagonal-down)
TEST( rect, intersection_different_rects_diagonal_down ) {
    canvas::Rect r1( 0,  0, 100, 100);
    canvas::Rect r2( 99, 99, 100, 100);
    canvas::Rect expected(99,99,1,1);
    canvas::Rect result;

    r1.intersection( r2, result );
    ASSERT_TRUE( result == expected );
    r2.intersection( r1, result );
    ASSERT_TRUE( result == expected );
}

// Tests intersection from different rects (diagonal-up)
TEST( rect, intersection_different_rects_diagonal_up ) {
    canvas::Rect r1( 99,  0, 100, 100);
    canvas::Rect r2( 0, 99, 100, 100);
    canvas::Rect expected(99,99,1,1);
    canvas::Rect result;

    r1.intersection( r2, result );
    ASSERT_TRUE( result == expected );
    r2.intersection( r1, result );
    ASSERT_TRUE( result == expected );
}

TEST( rect, abuts_squares_vertically ) {
	canvas::Rect r1(0,  0, 10, 10);
	canvas::Rect r2(0, 10, 10, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_squares_horizontally ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(10, 0, 10, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_rectangles_vertically ) {
	canvas::Rect r1( 0,  0, 100, 10);
	canvas::Rect r2( 0, 10, 100, 100);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}


TEST( rect, abuts_rectangles_horizontally ) {
	canvas::Rect r1( 0, 0,  10, 10);
	canvas::Rect r2(10, 0, 100, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_squares_vertically_in_the_middle ) {
	canvas::Rect r1(100, 100, 10, 10);
	canvas::Rect r2(100, 110, 10, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_squares_horizontally_center_in_the_middle ) {
	canvas::Rect r1( 100, 100, 10, 10);
	canvas::Rect r2( 110, 100, 10, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_intersects ) {
	canvas::Rect r1(  0,  0, 100, 100);
	canvas::Rect r2( 75, 75, 100, 100);

	ASSERT_FALSE( r1.abuts( r2 ) );
	ASSERT_FALSE( r2.abuts( r1 ) );
}

TEST( rect, abuts_not_intersects ) {
	canvas::Rect r1(  0,   0, 100, 100);
	canvas::Rect r2(200, 200, 100, 100);

	ASSERT_FALSE( r1.abuts( r2 ) );
	ASSERT_FALSE( r2.abuts( r1 ) );
}

TEST( rect, abuts_includes ) {
	canvas::Rect r1(  0,  0, 100, 100);
	canvas::Rect r2( 25, 25,  50,  50);

	ASSERT_FALSE( r1.abuts( r2 ) );
	ASSERT_FALSE( r2.abuts( r1 ) );
}

TEST( rect, abuts_vertically_different_width ) {
	canvas::Rect r1(0,  0, 10, 10);
	canvas::Rect r2(0, 10, 50, 10);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_different_height ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(10, 0, 10, 50);

	ASSERT_TRUE( r1.abuts( r2 ) );
	ASSERT_TRUE( r2.abuts( r1 ) );
}

TEST( rect, abuts_full_adjacency_level ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(10, 0, 10, 10);

	ASSERT_TRUE( r1.abuts( r2, 1 ) );
	ASSERT_TRUE( r2.abuts( r1, 1 ) );
}

TEST( rect, abuts_full_adjacency_level_2 ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(20, 0, 10, 10);

	ASSERT_FALSE( r1.abuts( r2, 1 ) );
	ASSERT_FALSE( r2.abuts( r1, 1 ) );
}

TEST( rect, abuts_half_adjacency_level ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(10, 0, 10, 10);

	ASSERT_TRUE( r1.abuts( r2, 0.5 ) );
	ASSERT_TRUE( r2.abuts( r1, 0.5 ) );
}

TEST( rect, abuts_mix_adjacency_level ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(5, 10, 5, 10);

	ASSERT_TRUE( r1.abuts( r2, 0.5 ) );
	ASSERT_FALSE( r2.abuts( r1, 0.7f ) );
}

TEST( rect, encompass_horizontally ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2(10, 0, 10, 10);
	canvas::Rect r3;
	r3= r1.encompass( r2 );
	ASSERT_TRUE(r3.x == 0);
	ASSERT_TRUE(r3.y == 0);
	ASSERT_TRUE(r3.w == 20);
	ASSERT_TRUE(r3.h == 10);

}

TEST( rect, encompass_vertically ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2( 0, 10, 10, 10);
	canvas::Rect r3;
	r3= r1.encompass( r2 );
	ASSERT_TRUE(r3.x == 0);
	ASSERT_TRUE(r3.y == 0);
	ASSERT_TRUE(r3.w == 10);
	ASSERT_TRUE(r3.h == 20);

}

TEST( rect, encompass_vertically_not_near ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2( 0, 20, 10, 10);
	canvas::Rect r3;
	r3= r1.encompass( r2 );
	ASSERT_TRUE(r3.x == 0);
	ASSERT_TRUE(r3.y == 0);
	ASSERT_TRUE(r3.w == 10);
	ASSERT_TRUE(r3.h == 30);

}

TEST( rect, encompass_horizontally_not_near ) {
	canvas::Rect r1( 0, 0, 10, 10);
	canvas::Rect r2( 20, 0, 10, 10);
	canvas::Rect r3;
	r3= r1.encompass( r2 );
	ASSERT_TRUE(r3.x == 0);
	ASSERT_TRUE(r3.y == 0);
	ASSERT_TRUE(r3.w == 30);
	ASSERT_TRUE(r3.h == 10);

}

TEST( rect, aside ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(100,100,10,100);
	canvas::Rect r3(190,100,10,100);
	canvas::Rect r4(100,100,100,10);
	canvas::Rect r5(100,190,100,10);
	canvas::Rect r6(125,125,50,50);

	ASSERT_TRUE(r2.aside(r1));
	ASSERT_TRUE(r3.aside(r1));
	ASSERT_TRUE(r4.aside(r1));
	ASSERT_TRUE(r5.aside(r1));
	ASSERT_FALSE(r6.aside(r1));
	ASSERT_FALSE(r1.aside(r1));
	
	ASSERT_FALSE(r1.aside(r2));
	ASSERT_FALSE(r1.aside(r3));
	ASSERT_FALSE(r1.aside(r4));
	ASSERT_FALSE(r1.aside(r5));
	ASSERT_FALSE(r1.aside(r6));
}

TEST( rect, cut_left ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(100,100,50,100);

	canvas::Rect r3 = r1.cut(r2);

	ASSERT_TRUE(r3.x==150);
	ASSERT_TRUE(r3.y==100);
	ASSERT_TRUE(r3.w==50);
	ASSERT_TRUE(r3.h==100);
}

TEST( rect, cut_right ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(150,100,50,100);

	canvas::Rect r3 = r1.cut(r2);

	ASSERT_TRUE(r3.x==100);
	ASSERT_TRUE(r3.y==100);
	ASSERT_TRUE(r3.w==50);
	ASSERT_TRUE(r3.h==100);
}

TEST( rect, cut_top ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(100,100,100,50);

	canvas::Rect r3 = r1.cut(r2);

	ASSERT_TRUE(r3.x==100);
	ASSERT_TRUE(r3.y==150);
	ASSERT_TRUE(r3.w==100);
	ASSERT_TRUE(r3.h==50);
}

TEST( rect, cut_bottom ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(100,150,100,50);

	canvas::Rect r3 = r1.cut(r2);

	ASSERT_TRUE(r3.x==100);
	ASSERT_TRUE(r3.y==100);
	ASSERT_TRUE(r3.w==100);
	ASSERT_TRUE(r3.h==50);
}


TEST( rect, goesInto_left ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,120,100,60);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesInto_right ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(150,120,100,60);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesInto_top ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,50,60,100);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesInto_bottom ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,150,60,100);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesInto_same_height ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,100,100,100);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_TRUE(r1.goesInto(r2));
}

TEST( rect, goesInto_same_width ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(100,50,100,100);

	ASSERT_TRUE(r2.goesInto(r1));
	ASSERT_TRUE(r1.goesInto(r2));
}

TEST( rect, goesInto_larger1 ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,120,200,60);

	ASSERT_FALSE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesInto_larger2 ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,50,60,200);

	ASSERT_FALSE(r2.goesInto(r1));
	ASSERT_FALSE(r1.goesInto(r2));
}

TEST( rect, goesThrough_horizontaly ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,120,200,60);

	ASSERT_TRUE(r2.goesThrough(r1));
	ASSERT_TRUE(r1.goesThrough(r2));
}

TEST( rect, goesThrough_verticaly ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,50,60,200);

	ASSERT_TRUE(r2.goesThrough(r1));
	ASSERT_TRUE(r1.goesThrough(r2));
}

TEST( rect, divide ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,120,200,60);

	std::vector<canvas::Rect> result = r1.divide(r2);
	ASSERT_TRUE(result.size()==2);
	ASSERT_TRUE(result[0]==canvas::Rect(100,100,100,20));
	ASSERT_TRUE(result[1]==canvas::Rect(100,180,100,20));

	result = r2.divide(r1);
	ASSERT_TRUE(result.size()==2);
	ASSERT_TRUE(result[0]==canvas::Rect(50,120,50,60));
	ASSERT_TRUE(result[1]==canvas::Rect(200,120,50,60));
}
