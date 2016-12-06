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

class SurfaceConstructor : public Surface {
protected:
    std::string getSubDirectory() const { return "Constructor"; }
};

// Test values of surface after creating it with a Rect as parameter
TEST_F( SurfaceConstructor, from_rect ) {
	canvas::Canvas *c = getCanvas();
	canvas::Surface* s = c->createSurface(canvas::Rect(10,10,100,100));

	ASSERT_FALSE ( s->autoFlush());
	ASSERT_FALSE ( s->isDirty() );
	ASSERT_TRUE ( s->getZIndex() == 1 );
	ASSERT_TRUE ( s->isVisible() );
	ASSERT_TRUE ( s->getColor () == canvas::Color() );
	canvas::Point p(10,10);
	ASSERT_TRUE ( s->getLocation() == p );
	canvas::Size size(100, 100);
	ASSERT_TRUE( s->getSize() == size );
	ASSERT_TRUE( s->boundsChanged() == false );

	c->destroy(s);
}

// Test values of surface after creating it with an invalid Rect as parameter
TEST_F( SurfaceConstructor, from_invalid_rect ) {
    canvas::Canvas *c = getCanvas();
    ASSERT_TRUE( c->createSurface(canvas::Rect(10,10,0,0)) == NULL );
}

// Test values of surface after creating it with an image as parameter
TEST_F( SurfaceConstructor, from_image ) {
	canvas::Canvas *c = getCanvas();
	canvas::Surface* s = c->createSurfaceFromPath( util::getImageName("image.png" ) );

	ASSERT_FALSE ( s->autoFlush() );
	ASSERT_FALSE ( s->isDirty() );
	ASSERT_TRUE ( s->getZIndex() == 1 );
	ASSERT_TRUE ( s->isVisible() );
	ASSERT_TRUE ( s->getColor() == canvas::Color() );
	ASSERT_TRUE ( s->getLocation() == canvas::Point() );
	ASSERT_TRUE( s->getSize() == canvas::Size(500, 375) );
	ASSERT_TRUE( s->boundsChanged() == false );

	c->destroy(s);
}

// Test that the image in canvas is the one from where the surface was created
TEST_F( SurfaceConstructor, from_image_check_image ) {
	canvas::Canvas *c = getCanvas();
	canvas::Surface* s = c->createSurfaceFromPath( util::getImageName("image.png" ) );

	s->autoFlush(true);

	ASSERT_TRUE( util::compareImages( c, getExpectedPath( "image" ) ) );

	c->destroy(s);
}

TEST_F( SurfaceConstructor, destructor ) {
	canvas::Canvas *c = getCanvas();
	canvas::Surface* s1 = c->createSurface( canvas::Rect(0,0,200,200));
	canvas::Surface* s2 = c->createSurface( canvas::Rect(50,50,100,100));
	canvas::Color color1(255,0,0);
	canvas::Color color2(0,255,0);

	s1->autoFlush(true);
	s2->autoFlush(true);
	s1->setColor(color1);
	s2->setColor(color2);
	s1->fillRect(canvas::Rect(0,0,200,200));
	s2->fillRect(canvas::Rect(0,0,100,100));

	ASSERT_TRUE(util::compareImages( c, getExpectedPath( "before_calling_destructor" ) ) );

	c->destroy(s2);

	ASSERT_TRUE(util::compareImages( c, getExpectedPath( "after_calling_destructor" ) ) );

	c->destroy(s1);

	ASSERT_TRUE( util::compareImages( c, "nothing" ) );
}

/// Check the surface created with pixel color default
TEST_F( SurfaceConstructor, pixel_color ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0,0,720,576));
	canvas::Color colorGet;
	canvas::Color colorDefault(0,0,0,0);
	s->autoFlush(true);
	for(int i = 0;i<720;i++){
		for(int j=0;j<576;j++){
		canvas::Point p(i,j);
		s->getPixelColor(p, colorGet);
		ASSERT_TRUE(colorDefault == colorGet);
		}
	}
	getCanvas()->destroy(s);
}

/// Check the surface from an image, is not created when the parameter path is invalid.
TEST_F( SurfaceConstructor, surface_from_inexistent_image ) {
	canvas::Surface *s = getCanvas()->createSurfaceFromPath( util::getImageName( "inexistent_image.jpg" ) );
	ASSERT_TRUE( s == NULL );
}
