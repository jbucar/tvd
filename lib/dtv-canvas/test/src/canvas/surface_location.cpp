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
#include "../../../src/surface.h"
#include "../../../src/canvas.h"

class SurfaceLocation: public Surface {
protected:
    std::string getSubDirectory() const { return "Location"; }
};

// Set the surface location to the top left corner of the canvas
TEST_F( SurfaceLocation, top_left ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point());

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_left") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the top right corner of the canvas
TEST_F( SurfaceLocation, top_right ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(620, 0));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_right") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the bottom left corner of the canvas
TEST_F( SurfaceLocation, bottom_left ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(0, 476));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_left") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the bottom right corner of the canvas
TEST_F( SurfaceLocation, bottom_right ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(620, 476));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_right") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the middle of the canvas
TEST_F( SurfaceLocation, middle ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(310, 238));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("middle") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the bottom right corner of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, bottom_right_part_outside ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(620, 476));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_right_part_outside") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the bottom left corner of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, bottom_left_part_outside ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s1 != NULL );
	s1->autoFlush(true);
	s1->setLocation(canvas::Point(-100, 476));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_left_part_outside") ) );

	getCanvas()->destroy( s1 );
}

// Set the surface location to the top left corner of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, top_left_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(-100, -100));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_left_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location to the top right corner of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, top_right_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(620, -100));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_right_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location to the bottom of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, bottom_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(110, 476));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location to the top of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, top_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(110, -150));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location to the left of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, left_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(-400, 80));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("left_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location to the right of the canvas with some part out of the canvas
TEST_F( SurfaceLocation, right_part_outside ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(620, 80));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("right_part_outside") ) );

	getCanvas()->destroy( s );
}

// Set the surface location completely outside the canvas (bottom)
TEST_F( SurfaceLocation, completely_outside_bottom ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(110, 576));

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Set the surface location completely outside the canvas (top)
TEST_F( SurfaceLocation, completely_outside_top ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(110, -375));

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Set the surface location completely outside the canvas (left)
TEST_F( SurfaceLocation, completely_outside_left) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(-500, 80));

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Set the surface location completely outside the canvas (right)
TEST_F( SurfaceLocation, completely_outside_right ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setLocation(canvas::Point(720, 80));

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Test markDirty after changing location
TEST_F( SurfaceLocation, markDirty_location ) {
	canvas::Rect rect(0,0,100,100);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Point p(300,300);
	s->setLocation(p);

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	canvas::Rect r(0,0,100,100);
	ASSERT_TRUE( dirtyRegions.at(0) == rect );

	getCanvas()->destroy( s );
}
