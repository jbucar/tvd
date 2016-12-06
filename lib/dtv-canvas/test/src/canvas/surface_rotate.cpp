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

class SurfaceRotate : public Surface {
protected:
    std::string getSubDirectory() const { return "Rotate"; }
};

// Surface rotated 0 degrees
TEST_F( SurfaceRotate, rotated_0_degrees ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(0);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_360_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(500,375) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 90 degrees
TEST_F( SurfaceRotate, rotated_90_degrees ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(90);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_90_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(375,500) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 180 degrees
TEST_F( SurfaceRotate, rotated_180_degrees ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(180);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_180_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(500,375) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 270 degrees
TEST_F( SurfaceRotate, rotated_270_degrees ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(270);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_270_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(375,500) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 360 degrees
TEST_F( SurfaceRotate, rotated_360_degrees ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(360);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_360_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(500,375) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 90 degrees negative
TEST_F( SurfaceRotate, rotated_90_degrees_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(-90);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_270_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(375,500) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 180 degrees negative
TEST_F( SurfaceRotate, rotated_180_degrees_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(-180);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_180_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(500,375) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 270 degrees negative
TEST_F( SurfaceRotate, rotated_270_degrees_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(-270);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_90_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(375,500) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated 360 degrees negative
TEST_F( SurfaceRotate, rotated_360_degrees_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(-360);
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rotated_360_degrees" ) ) );

	canvas::Size size = s2->getSize();
	ASSERT_TRUE( size == canvas::Size(500,375) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface rotated not multiple of 90
TEST_F( SurfaceRotate, rotated_not_multiple_of_90 ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(45);
	ASSERT_TRUE( s2 == NULL );

	getCanvas()->destroy( s1 );
}

// Surface rotated not multiple of 90 negative
TEST_F( SurfaceRotate, rotated_not_multiple_of_90_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( s1 != NULL );
	s1->setLocation(canvas::Point(100,100));

	canvas::Surface* s2 = s1->rotate(-45);
	ASSERT_TRUE( s2 == NULL );

	getCanvas()->destroy( s1 );
}
