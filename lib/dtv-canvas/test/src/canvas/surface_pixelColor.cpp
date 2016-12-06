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

#define THRESHOLD 20

TEST_F( Surface, pixelColor_test_arround ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(255,0,0);
	canvas::Color colorGet;
	canvas::Color color;

	s->setPixelColor(point, colorSet );

	s->getPixelColor( canvas::Point(9,9), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(9,10), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(9,11), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(10,9), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( point, colorGet );
	ASSERT_TRUE ( colorSet == colorGet );

	s->getPixelColor( canvas::Point(10,11), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(11,9), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(11,10), colorGet );
	ASSERT_TRUE ( color == colorGet );

	s->getPixelColor( canvas::Point(11,11), colorGet );
	ASSERT_TRUE ( color == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with color red
TEST_F( Surface, pixelColor_get_red ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(255,0,0);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor(point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with color red
TEST_F( Surface, pixelColor_set_red ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(255, 0, 0);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with color green
TEST_F( Surface, pixelColor_get_green ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(0,255,0);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor(point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with color green
TEST_F( Surface, pixelColor_set_green ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(0, 255, 0);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with color blue
TEST_F( Surface, pixelColor_get_blue ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(0,0,255);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with color blue
TEST_F( Surface, pixelColor_set_blue ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(0, 0, 255);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with color red with alpha
TEST_F( Surface, pixelColor_get_red_alpha ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(255,0,0,127);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with color red with alpha
TEST_F( Surface, pixelColor_set_red_alpha ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(255, 0, 0, 127);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with complex color
TEST_F( Surface, pixelColor_get_complex_color ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(200,128,60);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with complex color
TEST_F( Surface, pixelColor_set_complex_color ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(200, 128, 60);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet == colorGet );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with complex color with alpha
TEST_F( Surface, pixelColor_get_complex_color_alpha ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(200,128,60,128);
	canvas::Color colorGet;

	s->setColor(colorSet);
	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet.equals(colorGet, THRESHOLD) );

	getCanvas()->destroy( s );
}

// Tests setPixelColor() with complex color with alpha
TEST_F( Surface, pixelColor_set_complex_color_alpha ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color colorSet(200, 128, 60, 128);
	canvas::Color colorGet;

	s->setPixelColor( point, colorSet );
	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorSet.equals(colorGet, THRESHOLD) );

	getCanvas()->destroy( s );
}

// Load an image with color (254,0,0), get the pixelColor of a point and check the color
TEST_F( Surface, pixelColor_get_from_image_red ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("red.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color color(254,0,0);
	canvas::Color colorGet;

	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorGet == color );

	getCanvas()->destroy( s );
}

// Load an image with color (0,0,254), get the pixelColor of a point and check the color
TEST_F( Surface, pixelColor_get_from_image_blue ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("blue.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color color(0,0,254);
	canvas::Color colorGet;

	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorGet == color );

	getCanvas()->destroy( s );
}

// Load an image with color (0,255,1), get the pixelColor of a point and check the color
TEST_F( Surface, pixelColor_get_from_image_green ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("green.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color color(0,255,1);
	canvas::Color colorGet;

	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorGet == color );

	getCanvas()->destroy( s );
}

// Load an image with color (255,255,0), get the pixelColor of a point and check the color
TEST_F( Surface, pixelColor_get_from_image_yellow ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("yellow.jpg") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(10, 10);
	canvas::Color color(255,255,0);
	canvas::Color colorGet;

	s->getPixelColor( point, colorGet );

	ASSERT_TRUE ( colorGet == color );

	getCanvas()->destroy( s );
}

// Load a complex image, get the pixelColor of diferent points and check the color of each point
TEST_F( Surface, pixelColor_get_from_complex_image ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point p0(0,0), p1(499, 374), p2(248,196);
	canvas::Color c0(130,131,187), c1(147,144,91), c2(56,43,63);
	canvas::Color colorGet;

	s->getPixelColor( p0, colorGet );
	ASSERT_TRUE ( colorGet == c0 );

	s->getPixelColor( p1, colorGet );
	ASSERT_TRUE ( colorGet == c1 );

	s->getPixelColor( p2, colorGet );
	ASSERT_TRUE ( colorGet == c2 );

	getCanvas()->destroy( s );
}

// Try to set the color of a pixel out of the surface.
TEST_F( Surface, pixelColor_set_bad_pixel ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(820, 600);
	canvas::Color colorSet(200, 128, 60);

	ASSERT_FALSE( s->setPixelColor( point, colorSet ) );

	getCanvas()->destroy( s );
}

// Try to get the color of a pixel out of the surface.
TEST_F( Surface, pixelColor_get_bad_pixel ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);

	canvas::Point point(820, 600);
	canvas::Color colorGet;

	ASSERT_FALSE (s->getPixelColor( point, colorGet ) );

	getCanvas()->destroy( s );
}

// Test markDirty after changing pixelColor
TEST_F( Surface, pixerColor_markDirty ) {
	canvas::Rect rect(0,0,100,100);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Point point(10,10);
	canvas::Color color(255,0,0);
	ASSERT_TRUE( s->setPixelColor(point, color) );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	canvas::Rect r(10,10,1,1);
	ASSERT_TRUE( dirtyRegions.at(0) == r );

	getCanvas()->destroy( s );
}

// Tests getPixelColor() with color red with alpha 0
TEST_F( Surface, pixelColor_get_red_alpha_0 ) {
    canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    canvas::Point point(10, 10);
    canvas::Color colorSet(255,0,0,0);
    canvas::Color colorGet;

    s->setColor(colorSet);
    s->setPixelColor( point, colorSet );
    s->getPixelColor( point, colorGet );
    ASSERT_TRUE ( colorGet.equals(colorSet, THRESHOLD) );

    getCanvas()->destroy( s );
}

// Tests getPixelColor() with color red with alpha 122
TEST_F( Surface, pixelColor_get_red_alpha_122 ) {
    canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    canvas::Point point(10, 10);
    canvas::Color colorSet(255,0,0,122);
    canvas::Color colorGet;

    s->setColor(colorSet);
    s->setPixelColor( point, colorSet );
    s->getPixelColor( point, colorGet );
    ASSERT_TRUE ( colorGet.equals(colorSet, THRESHOLD) );

    getCanvas()->destroy( s );
}

// Tests getPixelColor() with color green with alpha 0
TEST_F( Surface, pixelColor_get_green_alpha_0 ) {
    canvas::Surface* s = getCanvas()->createSurface( canvas::Rect(0, 0, 720, 576) );

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    canvas::Point point(10, 10);
    canvas::Color colorSet(0,255,0,0);
    canvas::Color colorGet;

    s->setColor(colorSet);
    s->setPixelColor( point, colorSet );
    s->getPixelColor( point, colorGet );
    ASSERT_TRUE ( colorGet.equals(colorSet, THRESHOLD) );

    getCanvas()->destroy( s );
}

// Tests getPixelColor() with a complex color and alpha 0
TEST_F( Surface, DISABLED_pixelColor_get_complex_color_alpha_0 ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );
	canvas::Point point(0,0);
	canvas::Color setColor, getColor;
	setColor = canvas::Color(127,255,255,0);

	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	s->setColor(setColor);
	s->fillRect(rect);
	s->getPixelColor(point, getColor);

	ASSERT_TRUE( getColor.equals(setColor, THRESHOLD) );

	getCanvas()->destroy( s );
}
