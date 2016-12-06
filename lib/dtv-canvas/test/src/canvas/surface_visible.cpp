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

class SurfaceVisible : public Surface {
protected:
    std::string getSubDirectory() const { return "Visible"; }
};

/// Surface rendered from an image and then setVisible to false.
TEST_F( SurfaceVisible, nothing_compare ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setVisible(false);

	ASSERT_FALSE( s->isVisible() );
	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

/// Two surfaces rendered from a rectangle with color property.
/// The top surface setVisible to false.
TEST_F( SurfaceVisible, one_of_two_compare ) {
	canvas::Surface* s1 = getCanvas()->createSurface( canvas::Rect(100, 100, 100, 100) );
	canvas::Surface* s2 = getCanvas()->createSurface( canvas::Rect(150, 150, 100, 100) );

	ASSERT_TRUE( s1 && s2 );
	s1->autoFlush(true);
	s2->autoFlush(true);
	s1->setColor(canvas::Color(0, 155, 255));
	s2->setColor(canvas::Color(255, 155, 255));
	s1->fillRect(canvas::Rect(0,0,100,100));
	s2->fillRect(canvas::Rect(0,0,100,100));
	s2->setVisible(false);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("one_of_two") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Set visible to false and then to true
TEST_F( SurfaceVisible, image_true_dynamic){
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "red.jpg" ) );

	ASSERT_TRUE( s != NULL );
	s->setVisible(false);
	ASSERT_FALSE(s->isVisible());
	s->autoFlush(true);
	s->setVisible(true);
	ASSERT_TRUE(s->isVisible());

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("true_dynamic")));

	getCanvas()->destroy( s );
}

// Two surfaces with different zindex intersected. The surface with higher zindex is setted invisible
TEST_F( SurfaceVisible, images_with_zindex ) {
	canvas::Surface* s1 = getCanvas()->createSurface( canvas::Rect(100, 100, 100, 100) );
	canvas::Surface* s2 = getCanvas()->createSurface( canvas::Rect(150, 150, 100, 100) );

	ASSERT_TRUE( s1 && s2 );
	s1->autoFlush(true);
	s2->autoFlush(true);
	s1->setColor(canvas::Color(0, 0, 255));
	s2->setColor(canvas::Color(255, 0, 0));
	s1->setZIndex(2);
	s1->fillRect(canvas::Rect(0,0,100,100));
	s2->fillRect(canvas::Rect(0,0,100,100));
	s1->setVisible(false);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "with_zindex" ) ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Three surfaces with different zindex intersected. The surface in the middle is setted invisible
// and the one with highest zindex has alpha setted
TEST_F( SurfaceVisible, images_with_zindex_and_alpha ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );
	canvas::Surface* s2 = getCanvas()->createSurface( canvas::Rect(100, 100, 200, 200) );
	canvas::Surface* s3 = getCanvas()->createSurface( canvas::Rect(100, 100, 200, 200) );

	ASSERT_TRUE( s1 && s2 && s3 );

	s1->autoFlush(true);

	s2->autoFlush(true);
	s2->setColor(canvas::Color(0, 0, 255));
	s2->setZIndex(2);
	s2->fillRect(canvas::Rect(0,0,200,200));
	s2->setVisible(false);

	s3->autoFlush(true);
	s3->setColor(canvas::Color(255, 0, 0, 127) );
	s3->setZIndex(3);
	s3->fillRect(canvas::Rect(0,0,200,200));

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "images_with_zindex_and_alpha") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

// Test markDirty after setting visibile to false
TEST_F( SurfaceVisible, markDirty_visible_false ) {
	canvas::Rect rect(0,0,200,200);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	s->setVisible(false);

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	ASSERT_TRUE( dirtyRegions.at(0) == rect );

	getCanvas()->destroy( s );
}

// Test markDirty after setting visibile to true
TEST_F( SurfaceVisible, markDirty_visible_true ) {
	canvas::Rect rect(0,0,200,200);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setVisible(false);
	getCanvas()->flush();

	s->setVisible(true);

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	ASSERT_TRUE( dirtyRegions.at(0) == rect );

	getCanvas()->destroy( s );
}
