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

// Invalidates all the surface
TEST_F( Surface, invalidateRegion_invalidates_all_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	s->invalidateRegion(rect);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (middle)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(10,10,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (top_left)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-5,-5,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (top)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(355,-5,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (top_right)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(715,-5,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (right)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(715,283,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (bottom_right)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(715,571,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (bottom)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(355,571,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (bottom_left)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-5,571,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates part of the surface (left)
TEST_F( Surface, invalidateRegion_invalidates_part_of_the_surface_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-5,283,10,10);
	s->invalidateRegion(r);

	ASSERT_TRUE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (top-left)
TEST_F( Surface, invalidateRegion_outside_the_surface_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-10,-10,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (top)
TEST_F( Surface, invalidateRegion_outside_the_surface_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(355,-10,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (top-right)
TEST_F( Surface, invalidateRegion_outside_the_surface_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(720,-10,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (right)
TEST_F( Surface, invalidateRegion_outside_the_surface_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(720,283,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (bottom-right)
TEST_F( Surface, invalidateRegion_outside_the_surface_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(720,576,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (bottom)
TEST_F( Surface, invalidateRegion_outside_the_surface_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(355,576,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (bottom-left)
TEST_F( Surface, invalidateRegion_outside_the_surface_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-10,576,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

// Invalidates a rect outside the surface (left)
TEST_F( Surface, invalidateRegion_outside_the_surface_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r(-10,283,10,10);
	s->invalidateRegion(r);

	ASSERT_FALSE( s->isDirty() );

	getCanvas()->destroy( s );
}

TEST_F( Surface, adjacents_test ) {
	canvas::Rect rect1(0,0,360,288);
	canvas::Rect rect2(0,288,360,288);
	canvas::Rect rect3(360,0,360,288);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);

	ASSERT_FALSE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

TEST_F( Surface, adjacents_test2 ) {
	canvas::Rect rect1(0,0,360,288);
	canvas::Rect rect2(0,288,360,288);
	canvas::Rect rect3(360,260,360,288);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);

	ASSERT_TRUE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

TEST_F( Surface, adjacents_test3 ) {
	canvas::Rect rect1(0,0,360,288);
	canvas::Rect rect2(0,288,360,288);
	canvas::Rect rect3(361,260,359,288);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);

	ASSERT_TRUE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

TEST_F( Surface, adjacents_test4 ) {
	canvas::Rect rect1(0,0,10,10);
	canvas::Rect rect2(0,10,10,10);
	canvas::Rect rect3(10,0,10,10);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);

	ASSERT_FALSE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

TEST_F( Surface, adjacents_test5 ) {
	canvas::Rect rect1(0,0,10,10);
	canvas::Rect rect2(0,10,10,10);
	canvas::Rect rect3(8,0,10,10);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);

	ASSERT_TRUE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

TEST_F( Surface, adjacents_test6 ) {
	canvas::Rect rect1(180,0,10,10);
	canvas::Rect rect2(0,10,100,10);
	canvas::Rect rect3(100,10,100,10);
	canvas::Rect rect4(60,0,100,10);

	canvas::Surface* s1 = getCanvas()->createSurface( rect1 );

	ASSERT_TRUE( s1 != NULL );

	s1->autoFlush(true);
	getCanvas()->flush();

	canvas::Surface* s2 = getCanvas()->createSurface( rect2 );
	canvas::Surface* s3 = getCanvas()->createSurface( rect3 );
	canvas::Surface* s4 = getCanvas()->createSurface( rect4 );

	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );
	ASSERT_TRUE( s4 != NULL );

	s2->autoFlush(true);
	s3->autoFlush(true);
	s4->autoFlush(true);

	ASSERT_FALSE( s1->isDirty() );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
	getCanvas()->destroy( s4 );
}
