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

class SurfaceResize : public Surface {
protected:
	std::string getSubDirectory() const { return "Resize"; }
};

// Create surface from an image and then resize
TEST_F( SurfaceResize, basic_smaller_not_scaled ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size newSize(100,100);
	ASSERT_TRUE( s->resize( newSize ) );
	ASSERT_TRUE( s->getSize() == newSize );
	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize scaleing surface content
TEST_F( SurfaceResize, basic_smaller_scaled ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size newSize(100,100);
	ASSERT_TRUE( s->resize( newSize, true ) );
	ASSERT_TRUE( s->getSize() == newSize );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_smaller_scaled" ) ) );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize
TEST_F( SurfaceResize, basic_bigger_not_scaled ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size newSize(600,500);
	ASSERT_TRUE( s->resize( newSize ) );
	ASSERT_TRUE( s->getSize() == newSize );
	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing") );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize scaleing surface content
TEST_F( SurfaceResize, basic_bigger_scaled ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size newSize(600,500);
	ASSERT_TRUE( s->resize( newSize, true ) );
	ASSERT_TRUE( s->getSize() == newSize );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "basic_bigger_scaled" ) ) );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize to 0
TEST_F( SurfaceResize, basic_0_0 ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size oldSize = s->getSize();
	canvas::Size newSize(0,0);
	ASSERT_FALSE( s->resize( newSize ) );
	ASSERT_TRUE( s->getSize() == oldSize );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize to 0
TEST_F( SurfaceResize, basic_0_10 ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size oldSize = s->getSize();
	canvas::Size newSize(0,10);
	ASSERT_FALSE( s->resize( newSize ) );
	ASSERT_TRUE( s->getSize() == oldSize );

	getCanvas()->destroy( s );
}

// Create surface from an image and then resize to 0
TEST_F( SurfaceResize, basic_10_0 ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath(util::getImageName( "image.png"));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	canvas::Size oldSize = s->getSize();
	canvas::Size newSize(10,0);
	ASSERT_FALSE( s->resize( newSize ) );
	ASSERT_TRUE( s->getSize() == oldSize );

	getCanvas()->destroy( s );
}