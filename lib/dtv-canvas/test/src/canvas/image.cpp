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

// Test para el constructor de image
TEST_F( Surface, image_constructor ) {
	canvas::Surface* i  = getCanvas()->createSurfaceFromPath(util::getImageName("image.png") );
	canvas::Size s1,s2(500,375);

	ASSERT_TRUE( i != NULL );
	s1 = i->getSize();

	ASSERT_TRUE( s1 == s2 );

	getCanvas()->destroy( i );
}

#define THRESHOLD 2

// Test para el constructor de image (imagen con alpha)
TEST_F( Surface, image_constructor_with_alpha ) {
	canvas::Surface* i  = getCanvas()->createSurfaceFromPath(util::getImageName("alphatest.png") );
	canvas::Size s1,s2(380,287);
	canvas::Color c1, c2(38, 80, 102, 158);
	canvas::Point p(30, 70);

	ASSERT_TRUE( i != NULL );
	s1 = i->getSize();

	ASSERT_TRUE( s1 == s2 );
	i->getPixelColor( p, c1 );
	ASSERT_TRUE( c1.equals(c2, THRESHOLD) );

	getCanvas()->destroy( i );
}

// Test para el constructor de image (imagen con alpha)
TEST_F( Surface, image_constructor_with_alpha2 ) {
	canvas::Surface* i  = getCanvas()->createSurfaceFromPath(util::getImageName("lifia.png") );
	canvas::Size s1,s2(171,44);
	canvas::Color c1, c2(0, 0, 0, 0);
	canvas::Point p(10, 10);

	ASSERT_TRUE( i != NULL );
	s1 = i->getSize();

	ASSERT_TRUE( s1 == s2 );
	i->getPixelColor( p, c1 );
	ASSERT_TRUE( c1.equals(c2, THRESHOLD) );

	getCanvas()->destroy( i );
}

// Test para el constructor de image desde una imagen inexistente
TEST_F( Surface, image_constructor_from_inexistent_image ) {
	canvas::Surface* i  = getCanvas()->createSurfaceFromPath(util::getImageName("nn.png") );

	ASSERT_FALSE( i );
}

TEST_F( Surface, image_getSize) {
	canvas::Surface* i1 = getCanvas()->createSurfaceFromPath(util::getImageName("image.png"));
	canvas::Surface* i2 = getCanvas()->createSurfaceFromPath(util::getImageName("red.jpg"));

	ASSERT_TRUE( i1 && i2 );

	int w = i1->getSize().w;
	int h = i1->getSize().h;

	ASSERT_TRUE( w == 500 );
	ASSERT_TRUE( h == 375 );

	w = i2->getSize().w;
	h = i2->getSize().h;

	ASSERT_TRUE( w == 100 );
	ASSERT_TRUE( h == 100 );

	getCanvas()->destroy(i1);
	getCanvas()->destroy(i2);
}

TEST_F( Surface, image_getPixelColor ) {
	canvas::Surface* i1 = getCanvas()->createSurfaceFromPath(util::getImageName("image.png"));
	canvas::Surface* i2 = getCanvas()->createSurfaceFromPath(util::getImageName("red.jpg"));

	ASSERT_TRUE( i1 && i2 );

	canvas::Point p1(10,10),
			      p2(373,364),
				  p3(0,0);

	canvas::Color color(0,0,0),
				  c1(138,135,188),
				  c2(98,92,44),
				  c3(254,0,0);

	ASSERT_TRUE( i1->getPixelColor(p1, color) );
	ASSERT_TRUE( color.equals(c1, 0) );

	ASSERT_TRUE( i1->getPixelColor(p2, color) );
	ASSERT_TRUE( color.equals(c2, 0) );

	ASSERT_TRUE( i2->getPixelColor(p3, color) );
	ASSERT_TRUE( color.equals(c3, 0) );

	getCanvas()->destroy(i1);
	getCanvas()->destroy(i2);
}
