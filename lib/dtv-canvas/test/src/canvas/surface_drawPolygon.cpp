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

class SurfaceDrawPolygon : public Surface {
protected:
    std::string getSubDirectory() const { return "DrawPolygon"; }
};

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle1 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle1_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle2 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle2_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open2" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle3 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle3_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open3" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle4 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle4_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open4" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle5 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle5_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open5" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle6 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle6_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p6);
	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open6" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle7 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle7_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p7);
	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open7" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle8 ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the middle of the surface
TEST_F( SurfaceDrawPolygon, middle8_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(80,40),
				  p2(100,40),
				  p3(120,60),
				  p4(120,80),
				  p5(100,100),
				  p6(80,100),
				  p7(60,80),
				  p8(60,60);

	puntos.push_back(p8);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "middle_open8" ) ) );

	getCanvas()->destroy( s );
}

//Draw a polygon on the top left of the surface
TEST_F( SurfaceDrawPolygon, top_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(50,0), p2(150,0), p3(200,50), p4(150,100),p5(50,100),p6(0,50);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

//Draw a open polygon on the top left of the surface
TEST_F( SurfaceDrawPolygon, top_left_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(50,0), p2(150,0), p3(200,50), p4(150,100),p5(50,100),p6(0,50);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left_open" ) ) );

	getCanvas()->destroy( s );
}

// Draw a filled ellipse on the top left corner of the surface.
TEST_F( SurfaceDrawPolygon, top_left_last_point_equal_first ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(50,0), p2(150,0), p3(200,50), p4(150,100),p5(50,100),p6(0,50),p7(50,0);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, top_left_last_point_equal_first_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(50,0), p2(150,0), p3(200,50), p4(150,100),p5(50,100),p6(0,50), p7(50,0);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	puntos.push_back(p5);
	puntos.push_back(p6);
	puntos.push_back(p7);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, top_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,0), p2(719,0), p3(719,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "triangle_top_right" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, top_right_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,0), p2(719,0), p3(719,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_right_open" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, bottom_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(0,500), p2(0,475), p3(100,475), p4(100,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_left" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, bottom_left_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(0,500), p2(0,475), p3(100,475), p4(100,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_left_open" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, complex_intersect_points ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(0,476), p2(100,575), p3(100,476), p4(0,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complex_intersect_points" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, complex_intersect_points_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(0,476), p2(100,575), p3(100,476), p4(0,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "complex_intersect_points_open" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, bottom_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,500), p2(620,475), p3(719,475), p4(719,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_right" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, DISABLED_bottom_right_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,500), p2(620,475), p3(719,475), p4(719,575);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_right_open" ) ) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from 1 point
TEST_F( Surface,  drawPolygon_1point ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(1,1);
	puntos.push_back(p1);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from 1 point
TEST_F( Surface,  drawPolygon_1point_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(1,1);
	puntos.push_back(p1);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}


// Draw a polygon from three points with the same x position
TEST_F( SurfaceDrawPolygon, DISABLED_from_3_points_with_eq_x ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(150,100),p2(150,200),p3(150,300);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "from_3_points_with_eq_x" ) ) );

	getCanvas()->destroy( s );
}

// Draw a polygon from three points with the same x position
TEST_F( SurfaceDrawPolygon, from_3_points_with_eq_x_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(150,100),p2(150,200),p3(150,300);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawPolygon(puntos, false) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "from_3_points_with_eq_x" ) ) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from two points
TEST_F( SurfaceDrawPolygon, from_2_points ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(150,100),p2(150,200);
	puntos.push_back(p1);
	puntos.push_back(p2);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from two points
TEST_F( SurfaceDrawPolygon, from_2_points_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(150,100),p2(150,200);
	puntos.push_back(p1);
	puntos.push_back(p2);


	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_top ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,-1), p2(719,-1), p3(719,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_top_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,-1), p2(719,-1), p3(719,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,0), p2(759,0), p3(759,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_right_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,0), p2(759,0), p3(759,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_bottom ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,500), p2(719,500), p3(719,576);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_bottom_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,500), p2(719,500), p3(719,576);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(-100,200), p2(100,200), p3(100,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, some_part_outside_left_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(-100,200), p2(100,200), p3(100,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_top ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,-200), p2(719,-200), p3(719,-1);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(720,0), p2(789,0), p3(789,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_right_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(720,0), p2(789,0), p3(789,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_bottom ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,576), p2(719,576), p3(719,596);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_bottom_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(620,576), p2(719,576), p3(719,596);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(-100,200), p2(-1,200), p3(-1,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawPolygon, completely_out_left_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;
	canvas::Point p1(-100,200), p2(-1,200), p3(-1,250);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from an empty points collection
TEST_F( SurfaceDrawPolygon, nullpoints ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos) );

	getCanvas()->destroy( s );
}

// Try to draw a polygon from an empty points collection
TEST_F( SurfaceDrawPolygon, nullpoints_open ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	std::vector<canvas::Point> puntos;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawPolygon(puntos, false) );

	getCanvas()->destroy( s );
}

// Draw a polygon with alpha on the middle of the surface .
TEST_F( SurfaceDrawPolygon, polygon_with_alpha_and_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "polygon_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a open polygon with alpha on the middle of the surface .
TEST_F( SurfaceDrawPolygon, polygon_with_alpha_and_solid_background_open ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos, false) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "polygon_with_alpha_and_solid_background_open" ) ) );

    getCanvas()->destroy( s );
}

// Draw a polygon with alpha on the middle of the surface .
TEST_F( SurfaceDrawPolygon, polygon_with_alpha_and_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "polygon_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a polygon with alpha on the middle of the surface .
TEST_F( SurfaceDrawPolygon, DISABLED_polygon_with_alpha_and_transparent_background_open ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos, false) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "polygon_with_alpha_and_transparent_background_open" ) ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent polygon over a solid background.
TEST_F( SurfaceDrawPolygon, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent polygon over a solid background.
TEST_F( SurfaceDrawPolygon, full_transparent_over_solid_background_open ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos, false) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent polygon over a transparent background.
TEST_F( SurfaceDrawPolygon, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}

// Draw a open 	full transparent polygon over a transparent background.
TEST_F( SurfaceDrawPolygon, full_transparent_over_transparent_background_open ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    std::vector<canvas::Point> puntos;
    canvas::Point p1(220,188), p2(420,188), p3(500,288), p4(420,388),p5(220,388),p6(140,288);
    puntos.push_back(p1);
    puntos.push_back(p2);
    puntos.push_back(p3);
    puntos.push_back(p4);
    puntos.push_back(p5);
    puntos.push_back(p6);
    ASSERT_TRUE( s->drawPolygon(puntos, false) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background_open") ) );

    getCanvas()->destroy( s );
}

// Test markDirty after drawing a polygon
TEST_F( SurfaceDrawPolygon, markDirty_drawPolygon ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();

	canvas::Point p1(50,0),
				  p2(75,20),
				  p3(63,40),
				  p4(38,40),
				  p5(25,20);

	std::vector<canvas::Point> points;

	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	points.push_back(p4);
	points.push_back(p5);
	s->drawPolygon(points); // Point

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	canvas::Rect r1(25, 0, 51, 41);
	ASSERT_TRUE( dirtyRegions.at(0) == r1 );

	getCanvas()->destroy( s );
}

// Test markDirty after drawing a open polygon
TEST_F( SurfaceDrawPolygon, markDirty_drawPolygon_open ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();

	canvas::Point p1(50,0),
				  p2(75,20),
				  p3(63,40),
				  p4(38,40),
				  p5(25,20);

	std::vector<canvas::Point> points;

	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	points.push_back(p4);
	points.push_back(p5);
	s->drawPolygon(points, false); // Point

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	canvas::Rect r1(25, 0, 51, 41);
	ASSERT_TRUE( dirtyRegions.at(0) == r1 );

	getCanvas()->destroy( s );
}

