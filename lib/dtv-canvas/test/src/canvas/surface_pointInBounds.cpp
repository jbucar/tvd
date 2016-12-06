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

// Tests pointInBounds with a point inside the surface
TEST_F( Surface, pointInBounds_point_inside_the_surface ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(10,10);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the top-left corner of the surface
TEST_F( Surface, pointInBounds_point_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(0,0);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the top of the surface
TEST_F( Surface, pointInBounds_point_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(360,0);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the top-right corner of the surface
TEST_F( Surface, pointInBounds_point_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(719,0);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the right of the surface
TEST_F( Surface, pointInBounds_point_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(719,288);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the bottom-right corner of the surface
TEST_F( Surface, pointInBounds_point_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(719,575);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the bottom of the surface
TEST_F( Surface, pointInBounds_point_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(360,575);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the bottom-left corner of the surface
TEST_F( Surface, pointInBounds_point_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(0,575);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point in the left of the surface
TEST_F( Surface, pointInBounds_point_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(0,288);
	ASSERT_TRUE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (top-left)
TEST_F( Surface, pointInBounds_point_outside_top_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(-1,-1);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (top)
TEST_F( Surface, pointInBounds_point_outside_top ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(360,-1);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (top-right)
TEST_F( Surface, pointInBounds_point_outside_top_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(720,-1);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (right)
TEST_F( Surface, pointInBounds_point_outside_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(720,288);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (bottom-right)
TEST_F( Surface, pointInBounds_point_outside_bottom_right ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(720,576);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (bottom)
TEST_F( Surface, pointInBounds_point_outside_bottom ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(360,576);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (bottom-left)
TEST_F( Surface, pointInBounds_point_outside_bottom_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(-1,576);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

// Tests pointInBounds with a point outside the surface (left)
TEST_F( Surface, pointInBounds_point_outside_left ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );

	canvas::Point p(-1,288);
	ASSERT_FALSE( s->pointInBounds(p) );

	getCanvas()->destroy( s );
}

