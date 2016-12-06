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

class SurfaceBlit : public Surface {
protected:
    std::string getSubDirectory() const { return "Blit"; }
};

// Surface created from image is blitted into an empty surface in pos(0,0)
TEST_F( SurfaceBlit, complete_source_in_empty_surface_pos_0_0 ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	ASSERT_TRUE( s1->blit( target, s2 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complete_source_in_empty_surface_pos_0_0" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface created from image is blitted into an empty surface in pos(100,100)
TEST_F( SurfaceBlit, complete_source_in_empty_surface_pos_100_100 ) {
    canvas::Rect rect(0,0,720,576);
    canvas::Surface* s1 = getCanvas()->createSurface( rect );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    ASSERT_TRUE( s1->blit( target, s2 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complete_source_in_empty_surface_pos_100_100" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Surface created from image is blitted into another surface created from an image in pos(0,0)
TEST_F( SurfaceBlit, complete_source_in_surface_with_image_pos_0_0 ) {
    canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia_withoutAlpha.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(0,0);
    ASSERT_TRUE( s1->blit( target, s2 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complete_source_in_surface_with_image_pos_0_0" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Surface created from image is blitted into another surface created from an image in pos(100,100)
TEST_F( SurfaceBlit, complete_source_in_surface_with_image_pos_100_100 ) {
    canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia_withoutAlpha.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    ASSERT_TRUE( s1->blit( target, s2 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complete_source_in_surface_with_image_pos_100_100" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Rect from surface created from image is blitted into an empty surface in pos(0,0)
TEST_F( SurfaceBlit, rect_from_source_in_empty_surface_pos_0_0 ) {
    canvas::Rect rect(0,0,720,576);
    canvas::Surface* s1 = getCanvas()->createSurface( rect );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(0,0);
    canvas::Rect source(0,0,200,200);
    ASSERT_TRUE( s1->blit( target, s2, source ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_from_source_in_empty_surface_pos_0_0" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Rect from surface created from image is blitted into an empty surface in pos(100,100)
TEST_F( SurfaceBlit, rect_from_source_in_empty_surface_pos_100_100 ) {
    canvas::Rect rect(0,0,720,576);
    canvas::Surface* s1 = getCanvas()->createSurface( rect );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    canvas::Rect source(0,0,200,200);
    ASSERT_TRUE( s1->blit( target, s2, source ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_from_source_in_empty_surface_pos_100_100" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Rect from surface created from image is blitted into another surface created from an image in pos(0,0)
TEST_F( SurfaceBlit, rect_from_source_in_surface_with_image_pos_0_0 ) {
    canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia_withoutAlpha.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(0,0);
    canvas::Rect source(0,0,100,25);
    ASSERT_TRUE( s1->blit( target, s2, source ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_from_source_in_surface_with_image_pos_0_0" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Rect from surface created from image is blitted into another surface created from an image in pos(100,100)
TEST_F( SurfaceBlit, rect_from_source_in_surface_with_image_pos_100_100 ) {
    canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia_withoutAlpha.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    canvas::Rect source(0,0,100,25);
    ASSERT_TRUE( s1->blit( target, s2, source ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_from_source_in_surface_with_image_pos_100_100" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Blit a rect from the middle of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_middle_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(200,187,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_middle_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the top left corner of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_top_left_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(0,0,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_top_left_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the top of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_top_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(200,0,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_top_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the top right corner of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_top_right_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(400,0,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_top_right_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the right side of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_right_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(400,137,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_right_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the bottom right corner of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_bottom_right_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(400,275,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_bottom_right_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the bottom of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_bottom_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(200,275,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_bottom_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the bottom left corner of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_bottom_left_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(0,275,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_bottom_left_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect from the left side of a surface into another surface.
TEST_F( SurfaceBlit, rect_from_left_of_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(false);

	canvas::Point target(310,238);
	canvas::Rect source(0,137,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("rect_from_left_of_surface" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a surface into another surface of the same size.
TEST_F( SurfaceBlit, surface_eq_size ) {
	canvas::Rect rect(0,0,500,375);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	ASSERT_TRUE( s1->blit( target, s2 ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "surface_eq_image" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a surface into another surface of the same width.
TEST_F( SurfaceBlit, surface_eq_width ) {
	canvas::Rect rect(0,0,500,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s1->setColor( canvas::Color(255,0,0) );
	s1->fillRect( canvas::Rect(0,0, s1->getSize().w, s1->getSize().h));

	canvas::Point target(0,0);
	ASSERT_TRUE( s1->blit( target, s2 ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("surface_eq_width" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a surface into another surface of the same height.
TEST_F( SurfaceBlit, surface_eq_height ) {
	canvas::Rect rect(0,0,720,375);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s1->setColor( canvas::Color(255,0,0) );
    s1->setColor( canvas::Color(255,0,0) );
    s1->fillRect( canvas::Rect(0,0, s1->getSize().w, s1->getSize().h));

	canvas::Point target(0,0);
	ASSERT_TRUE( s1->blit( target, s2 ) );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("surface_eq_height" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect of size 0 from a surface into another surface.
TEST_F( SurfaceBlit, surface_from_rect_size_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(0,0,0,0);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect of width 0 from a surface into another surface.
TEST_F( SurfaceBlit, surface_from_rect_width_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(0,0,0,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit a rect of height 0 from a surface into another surface.
TEST_F( SurfaceBlit, surface_from_rect_height_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(0,0,100,0);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (top-left)
TEST_F( SurfaceBlit, part_outside_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-1,-1);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_top_left" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (top)
TEST_F( SurfaceBlit, part_outside_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,-1);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_top" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (top-right)
TEST_F( SurfaceBlit, part_outside_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(621,-1);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_top_right" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (right)
TEST_F( SurfaceBlit, part_outside_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(621,238);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_right" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (bottom-right)
TEST_F( SurfaceBlit, part_outside_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(621,477);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_bottom_right" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (bottom)
TEST_F( SurfaceBlit, part_outside_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,477);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_bottom" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (bottom-left)
TEST_F( SurfaceBlit, part_outside_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-1,477);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_bottom_left" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect part outside the surface. (left)
TEST_F( SurfaceBlit, part_outside_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-1,238);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("part_outside_left" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (top-left)
TEST_F( SurfaceBlit, completely_outside_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-100,-100);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (top)
TEST_F( SurfaceBlit, completely_outside_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,-100);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (top-right)
TEST_F( SurfaceBlit, completely_outside_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(720,-100);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (right)
TEST_F( SurfaceBlit, completely_outside_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(720,238);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (bottom-right)
TEST_F( SurfaceBlit, completely_outside_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(720,576);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (bottom)
TEST_F( SurfaceBlit, completely_outside_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,576);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (bottom-left)
TEST_F( SurfaceBlit, completely_outside_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-100,576);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with target rect completely outside the surface. (left)
TEST_F( SurfaceBlit, completely_outside_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(-100,238);
	canvas::Rect source(200,200,100,100);
	ASSERT_TRUE( s1->blit( target, s2, source ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect of width lower than zero.
TEST_F( SurfaceBlit, source_rect_with_lt_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(200,200,-1,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect of height lower than zero.
TEST_F( SurfaceBlit, source_rect_hegiht_lt_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(200,200,100,-1);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect of width and height lower than zero.
TEST_F( SurfaceBlit, source_rect_width_and_height_lt_zero ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(0,0);
	canvas::Rect source(200,200,-1,-1);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (top-left).
TEST_F( SurfaceBlit, source_rect_part_outside_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-1,-1,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (top).
TEST_F( SurfaceBlit, source_rect_part_outside_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(200,-1,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (top-right).
TEST_F( SurfaceBlit, source_rect_part_outside_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(401,-1,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (right).
TEST_F( SurfaceBlit, source_rect_part_outside_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(401,137,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (bottom-right).
TEST_F( SurfaceBlit, source_rect_part_outside_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(401,276,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (bottom).
TEST_F( SurfaceBlit, source_rect_part_outside_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(200,276,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (bottom-left).
TEST_F( SurfaceBlit, source_rect_part_outside_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-1,276,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect part outside the surface (left).
TEST_F( SurfaceBlit, source_rect_part_outside_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-1,137,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (top-left).
TEST_F( SurfaceBlit, source_rect_completely_outside_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-100,-100,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (top).
TEST_F( SurfaceBlit, source_rect_completely_outside_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(200,-100,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (top-right).
TEST_F( SurfaceBlit, source_rect_completely_outside_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(500,-1,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (right).
TEST_F( SurfaceBlit, source_rect_completely_outside_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(500,137,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (bottom-right).
TEST_F( SurfaceBlit, source_rect_completely_outside_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(500,375,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (bottom).
TEST_F( SurfaceBlit, source_rect_completely_outside_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(200,375,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (bottom-left).
TEST_F( SurfaceBlit, source_rect_completely_outside_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-100,375,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Blit with source rect completely outside the surface (left).
TEST_F( SurfaceBlit, source_rect_completely_outside_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	canvas::Point target(310,238);
	canvas::Rect source(-100,137,100,100);
	ASSERT_FALSE( s1->blit( target, s2, source ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface created from image with alpha is blitted into an empty surface
TEST_F( SurfaceBlit, surface_from_image_with_alpa_into_empty_surface ) {
    canvas::Rect rect(0,0,720,576);
    canvas::Surface* s1 = getCanvas()->createSurface( rect );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    ASSERT_TRUE( s1->blit( target, s2 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "surface_from_image_with_alpa_into_empty_surface" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Surface created from image with alpha is blitted into a surface with an image
TEST_F( SurfaceBlit, surface_from_image_with_alpa_into_surface_with_image ) {
    canvas::Rect rect(0,0,720,576);
    canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
    canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia.png" ) );

    ASSERT_TRUE(s1 && s2);
    s1->autoFlush(true);

    canvas::Point target(100,100);
    ASSERT_TRUE( s1->blit( target, s2 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "surface_from_image_with_alpa_into_surface_with_image" ) ) );

    getCanvas()->destroy( s1 );
    getCanvas()->destroy( s2 );
}

// Surface created from image is blitted into an empty surface in pos(0,0)
TEST_F( SurfaceBlit, markDirty_Blit_target_equal_0) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	getCanvas()->flush();

	canvas::Rect r1(0,0,500,375);
	canvas::Point target(0,0);
	ASSERT_TRUE( s1->blit( target, s2 ) );


	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();
	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );


	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface created from image is blitted into an empty surface in pos(0,0)
TEST_F( SurfaceBlit, markDirty_Blit_target_not_equal_0) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s1 = getCanvas()->createSurface( rect );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);

	getCanvas()->flush();

	canvas::Rect r1(100,100,500,375);
	canvas::Point target(100,100);
	ASSERT_TRUE( s1->blit( target, s2 ) );


	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();
	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );


	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}
