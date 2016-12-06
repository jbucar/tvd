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
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class SurfaceCompositionModeTransparency : public Surface {
protected:
    std::string getSubDirectory() const { return "CompositionMode"; }
    std::string getInternalDirectory() const { return "Transparency"; }

    std::string getExpectedPath( const std::string &fileName ) const {
        fs::path root( Surface::getExpectedPath( getInternalDirectory() ) );
        root /= fileName;
        return root.string();
    }
};

/*Los fill y la transparencia de imagenes funcionan. Los draw y el dibujar texto no funcionan */

/*Draw in the surface with alpha: fillRect, fillPolygon, drawEllipse, fillEllipse, fillRoundRect, drawText, and blit */
void draw_compositionMode1_transparency(canvas::Surface* s, canvas::Canvas* c, canvas::Surface* sblit) {         // TODO
	canvas::Surface* i = c->createSurfaceFromPath(util::getImageName("image.png"));

	s->autoFlush(true);

	s->blit(canvas::Point(0,0), i , canvas::Rect(0,0,500,375));
	s->setColor(canvas::Color(255,255,0,200));
	s->fillRect(canvas::Rect(20, 20, 200, 200)) ;

	s->setColor(canvas::Color(255,0,0));
	s->drawLine( 420, 10, 235, 85);
	s->setColor(canvas::Color(255,255,0,200));
	std::vector<canvas::Point> puntos;
	canvas::Point p1(300,10), p2(350, 10), p3(400,85), p4(250,85);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	s->fillPolygon(puntos);

	s->setColor(canvas::Color(255,0,0));
	s->drawLine( 450, 80, 560, 80);
	s->setColor(canvas::Color(255,255,0,200));
	s->drawEllipse( canvas::Point(500, 60), 50, 50, 0, 0 ) ;

	s->setColor(canvas::Color(255,0,0));
	s->drawRect(canvas::Rect(50, 450, 90, 90));
	s->setColor(canvas::Color(255,255,0,200));
	s->fillEllipse( canvas::Point(100, 476), 50, 50, 0, 0 ) ;

	s->setColor(canvas::Color(255,0,0));
	s->drawRoundRect( canvas::Rect(350, 450, 100, 60), 10, 10 );
	s->setColor(canvas::Color(255,255,0,200));
	s->fillRoundRect( canvas::Rect(300, 450, 100, 100), 10, 10 );

	s->setColor(canvas::Color(255,0,0));
	std::vector<canvas::Point> puntos2;
	canvas::Point p5(550,550), p6(650, 550), p7(600,450);
	puntos2.push_back(p5);
	puntos2.push_back(p6);
	puntos2.push_back(p7);
	s->drawPolygon(puntos2);

	canvas::FontInfo f;
	f.size(32);
	s->setFont(f);
	s->setColor(canvas::Color(255,255,0,200));
	s->drawText(canvas::Point(550,560),"HOLA");

	canvas::Point target(200,200);
	canvas::Rect source(0,0,380,287);
	s->blit(canvas::Point(225,200), i , canvas::Rect(0,0,500,375));
	s->blit( target, sblit, source );

	c->destroy( i );
}
 
 /*Draw in the surface with alpha: blit, line, drawRect, drawRoundRect, drawPolygon */
void draw_compositionMode2(canvas::Surface* s, canvas::Canvas* c, canvas::Surface* sblit) {
	canvas::Surface* i = c->createSurfaceFromPath(util::getImageName("alphatest.png"));
	canvas::Surface* i2 = c->createSurfaceFromPath(util::getImageName("lifia.png"));
	s->autoFlush(true);

	s->setColor(canvas::Color(255,0,0));
	s->fillRect(canvas::Rect(20, 20, 200, 200)) ;
	s->blit(canvas::Point(0,0), i , canvas::Rect(0,0,380,287));

	s->setColor(canvas::Color(255,0,0));
	std::vector<canvas::Point> puntos;
	canvas::Point p1(300,10), p2(350, 10), p3(400,85), p4(250,85);
	puntos.push_back(p1);
	puntos.push_back(p2);
	puntos.push_back(p3);
	puntos.push_back(p4);
	s->fillPolygon(puntos);
	s->setColor(canvas::Color(255,255,0,200));
	s->drawLine( 420, 10, 235, 85);

	s->setColor(canvas::Color(255,0,0));
	s->drawEllipse( canvas::Point(500, 60), 50, 50, 0, 0 ) ;
	s->setColor(canvas::Color(255,255,0,200));
	s->drawLine( 500, 10, 500, 140);

	s->setColor(canvas::Color(255,0,0));
	s->fillEllipse( canvas::Point(100, 476), 50, 50, 0, 0 ) ;
	s->setColor(canvas::Color(255,255,0,200));
	s->drawRect(canvas::Rect(50, 450, 90, 90)) ;

	s->setColor(canvas::Color(255,0,0));
	s->fillRoundRect( canvas::Rect(300, 450, 100, 100), 10, 10 );
	s->setColor(canvas::Color(255,255,0,200));
	s->drawRoundRect( canvas::Rect(350, 450, 100, 60), 10, 10 );

	s->setColor(canvas::Color(255,0,0));
	canvas::FontInfo f;
	f.size(32);
	s->setFont(f);
	s->drawText(canvas::Point(550,560),"HOLA");
	s->setColor(canvas::Color(255,255,0,200));
	std::vector<canvas::Point> puntos2;
	canvas::Point p5(550,550), p6(650, 550), p7(600,450);
	puntos2.push_back(p5);
	puntos2.push_back(p6);
	puntos2.push_back(p7);
	s->drawPolygon(puntos2);

	canvas::Point target(220,180);
	canvas::Rect source(0,0,380,287);
	s->blit( target, sblit, source );
	s->blit(canvas::Point(220,200), i2 , canvas::Rect(0,0,171,44));

	c->destroy( i );
	c->destroy( i2 );
}

/*Test composition mode getter default */
TEST_F( SurfaceCompositionModeTransparency, getDefault ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	ASSERT_TRUE(s1->getCompositionMode() == canvas::composition::source_over);
	getCanvas()->destroy( s1 );
}

/*Test compositionMode setter*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_setter ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	s1->setCompositionMode(canvas::composition::source_in);
	ASSERT_TRUE(s1->getCompositionMode() == canvas::composition::source_in);
	getCanvas()->destroy( s1 );
}

/*Create a surface  with default compositionMode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, compositionMode1_default ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_source_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with default mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, compositionMode2_default ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	draw_compositionMode2(s1,getCanvas(),s2);
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_source_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );    
}

/*Create a surface  with source_over mode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, compositionMode1_source_over ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source_over);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_source_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source_over mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, compositionMode2_source_over ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png") );
	s1->setCompositionMode(canvas::composition::source_over);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_source_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );  
}

/*Create a surface  with destination_over mode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_destination_over ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_over);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_destination_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_over mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_destination_over ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png") );
	s1->setCompositionMode(canvas::composition::destination_over);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_destination_over" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with clear mode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_clear ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::clear);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_clear" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with clear mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_clear ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png") );
	s1->setCompositionMode(canvas::composition::clear);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_clear" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source mode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_source ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_source" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_source ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png") );
	s1->setCompositionMode(canvas::composition::source);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_source" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination mode and then call compositionMode1 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_destination ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_destination" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination mode and then call compositionMode2 function*/
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_destination ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png") );
	s1->setCompositionMode(canvas::composition::destination);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_destination" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}
/*Create a surface  with destination_in mode and then call compositionMode1 function*/ 


/*Create a surface  with destination_in mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_destination_in ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_in);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_destination_in" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_in mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_destination_in ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_in);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_destination_in" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source_out mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_source_out ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source_out);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_source_out" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source_out mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_source_out ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source_out);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_source_out" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_out mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_destination_out ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_out);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_destination_out" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_out mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_destination_out ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_out);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_destination_out" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source_atop mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_source_atop ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source_atop);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_source_atop" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with source_atop mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_source_atop ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::source_atop);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_source_atop" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_atop mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_destination_atop ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_atop);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_destination_atop" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with destination_atop mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_destination_atop ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::destination_atop);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_destination_atop" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with x_or mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_x_or ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::x_or);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_x_or" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with x_or mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_x_or ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::x_or);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_x_or" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with plus mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_plus ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::plus);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_plus" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with plus mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_plus ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::plus);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_plus" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with multiply mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_multiply ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::multiply);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_multiply" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with multiply mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_multiply ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::multiply);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_multiply" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with screen mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_screen ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::screen);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_screen" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with screen mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_screen ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::screen);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_screen" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with overlay mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_overlay ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::overlay);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_overlay" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with overlay mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_overlay ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::overlay);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_overlay" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with darken mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_darken ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::darken);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_darken" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with darken mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_darken ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::darken);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_darken" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with lighten mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_lighten ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::lighten);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_lighten" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with lighten mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_lighten ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::lighten);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_lighten" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with color_dodge mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_color_dodge ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::color_dodge);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_color_dodge" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with color_dodge mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_color_dodge ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::color_dodge);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_color_dodge" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with color_burn mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_color_burn ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::color_burn);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_color_burn" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with color_burn mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_color_burn ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::color_burn);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_color_burn" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with hard_light mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_hard_light ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::hard_light);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_hard_light" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with hard_light mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_hard_light ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::hard_light);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_hard_light" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with soft_light mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_soft_light ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::soft_light);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_soft_light" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with soft_light mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_soft_light ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::soft_light);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_soft_light" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with difference mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_difference ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::difference);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_difference" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with difference mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_difference ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::difference);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_difference" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with exclusion mode and then call compositionMode1 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode1_exclusion ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::exclusion);
	draw_compositionMode1_transparency(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode1_exclusion" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/*Create a surface  with exclusion mode and then call compositionMode2 function*/ 
TEST_F( SurfaceCompositionModeTransparency, DISABLED_compositionMode2_exclusion ) {
	canvas::Surface* s1 = getCanvas()->createSurface(canvas::Rect(0,0,720,576));
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "alphatest.png" ) );
	s1->setCompositionMode(canvas::composition::exclusion);
	draw_compositionMode2(s1,getCanvas(),s2);

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "compositionMode2_exclusion" ) ) );
	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}
