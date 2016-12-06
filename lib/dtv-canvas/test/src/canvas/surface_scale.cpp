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

class SurfaceScale : public Surface {
protected:
    std::string getSubDirectory() const { return "Scale"; }
};

// Surface scaled double
TEST_F( SurfaceScale, factor_double ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,720,576);
	canvas::Rect r2(0,0,360,288);
	ASSERT_TRUE( s2->scale(r1, s1, r2) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_double" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled half
TEST_F( SurfaceScale, factor_half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r(0,0,250,188);
	ASSERT_TRUE( s2->scale(r, s1) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_half" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled half
TEST_F( SurfaceScale, factor_half_2 ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,100,100);
	canvas::Rect r2(100,100,200,200);
	ASSERT_TRUE( s2->scale(r1, s1, r2) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_half_2" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled equal
TEST_F( SurfaceScale, factor_equal ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r(0,0,500,375);
	ASSERT_TRUE( s2->scale(r, s1) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_equal" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled height half
TEST_F( SurfaceScale, factor_height_half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r(0,0,500,188);
	ASSERT_TRUE( s2->scale(r, s1) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_height_half" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled width half
TEST_F( SurfaceScale, factor_width_half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	ASSERT_TRUE( s2->scale( canvas::Rect(0,0,250,375), s1 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_width_half" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled height double
TEST_F( SurfaceScale, factor_height_double ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	ASSERT_TRUE( s2->scale( canvas::Rect(0,0,500,576), s1, canvas::Rect(0,0,500,288) ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_height_double" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled width double
TEST_F( SurfaceScale, factor_width_double ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	ASSERT_TRUE( s2->scale( canvas::Rect(0,0,720,375), s1, canvas::Rect(0,0,360,375) ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_width_double" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled double negative
TEST_F( SurfaceScale, factor_double_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,720,576);
	canvas::Rect r2(0,0,360,288);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_double_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled height negative
TEST_F( SurfaceScale, factor_half_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,250,188);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_half_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled equal negative
TEST_F( SurfaceScale, factor_equal_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,375);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_equal_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}


// Surface scaled height half negative
TEST_F( SurfaceScale, factor_negative_height_half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,188);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_negative_height_half" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled width half negative
TEST_F( SurfaceScale, factor_negative_width_half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,250,375);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_negative_width_half" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}


// Surface scaled height double negative
TEST_F( SurfaceScale, factor_negative_height_double ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,576);
	canvas::Rect r2(0,0,500,288);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_negative_height_double" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}


// Surface scaled width double negative
TEST_F( SurfaceScale, factor_negative_width_double ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,720,375);
	canvas::Rect r2(0,0,360,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_negative_width_double" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled double height negative
TEST_F( SurfaceScale, factor_double_height_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,720,576);
	canvas::Rect r2(0,0,360,288);
	ASSERT_TRUE( s2->scale(r1, s1, r2, false, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_double_height_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}


// Surface scaled double width negative
TEST_F( SurfaceScale, factor_double_width_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,720,576);
	canvas::Rect r2(0,0,360,288);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_double_width_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled half height negative
TEST_F( SurfaceScale, factor_half_hegiht_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,250,188);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, false, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_half_hegiht_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled half width negative
TEST_F( SurfaceScale, factor_half_width_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,250,188);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_half_width_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}


// Surface height mirrored
TEST_F( SurfaceScale, factor_equal_hegiht_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,375);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, false, true) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_equal_hegiht_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface width mirrored
TEST_F( SurfaceScale, factor_equal_width_negative ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,375);
	canvas::Rect r2(0,0,500,375);
	ASSERT_TRUE( s2->scale(r1, s1, r2, true, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "factor_equal_width_negative" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled width 0 and height 0 
TEST_F( SurfaceScale, factor_zero ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,0,0);
	canvas::Rect r2(0,0,500,375);
	ASSERT_FALSE( s2->scale(r1, s1, r2) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled width 0
TEST_F( SurfaceScale, factor_width_zero ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,0,375);
	canvas::Rect r2(0,0,500,375);
	ASSERT_FALSE( s2->scale(r1, s1, r2) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Surface scaled height 0
TEST_F( SurfaceScale, factor_height_zero ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );

	s2->autoFlush(true);
	canvas::Rect r1(0,0,500,0);
	canvas::Rect r2(0,0,500,375);
	ASSERT_FALSE( s2->scale(r1, s1, r2) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}
