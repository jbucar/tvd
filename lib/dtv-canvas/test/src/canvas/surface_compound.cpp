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
#include "../../../src/fontinfo.h"
#include "../../../src/surface.h"

class SurfaceCompound : public Surface {
protected:
    std::string getSubDirectory() const { return "Compound"; }
};

TEST_F( SurfaceCompound, figures ) {

	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect( 0, 0, 720, 576 ) );

	ASSERT_TRUE( s != NULL );

	s->autoFlush( true );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->fillRect(canvas::Rect(200, 200, 100, 100)) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawRect(canvas::Rect(210, 210, 100, 100)) );

	s->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s->fillRect( canvas::Rect(220, 220, 100, 100) ) );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->fillRoundRect(canvas::Rect(230, 230, 100, 100), 10, 10) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawRoundRect(canvas::Rect(240, 240, 100, 100), 10, 10) );

	s->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s->fillRoundRect(canvas::Rect(250, 250, 100, 100), 10, 10) );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->fillEllipse( canvas::Point(320, 320), 50, 50, 0, 0 ) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawEllipse( canvas::Point(330, 330), 50, 50, 0, 0 ) );

	s->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s->fillEllipse( canvas::Point(340, 340), 50, 50, 0, 0 ) );

	s->setColor( canvas::Color( 255, 255, 255, 224 ) );
	ASSERT_TRUE( s->drawLine( 200, 200, 340, 340 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "figures" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceCompound, figures_and_text ) {
	canvas::Surface* s = getCanvas()->createSurface( canvas::Rect( 0, 0, 720, 576 ) );

	ASSERT_TRUE( s != NULL );
	s->setFont( canvas::FontInfo("Tiresias", 10) );

	s->autoFlush( true );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(100, 90), "fillRect( 100,100,100,100 )" ) );
	ASSERT_TRUE( s->fillRect(canvas::Rect(100, 100, 100, 100)) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(300, 90), "drawRect( 300,100,100,100 )" ) );
	ASSERT_TRUE( s->drawRect(canvas::Rect(300, 100, 100, 100)) );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(500, 90), "fillRoundRect( 500,100,100,100,10,10 )" ) );
	ASSERT_TRUE( s->fillRoundRect(canvas::Rect(500, 100, 100, 100), 10, 10) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(100, 290), "drawRoundRect( 100,300,100,100,10,10 )" ) );
	ASSERT_TRUE( s->drawRoundRect(canvas::Rect(100, 300, 100, 100), 10, 10) );

	s->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(300, 290), "fillEllipse( 350,350,50,50,0,0 )" ) );
	ASSERT_TRUE( s->fillEllipse( canvas::Point(350, 350), 50, 50, 0, 0 ) );

	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(500, 290), "drawEllipse( 550,350,50,50,0,0 )" ) );
	ASSERT_TRUE( s->drawEllipse( canvas::Point(550, 350), 50, 50, 0, 0 ) );

	s->setColor( canvas::Color( 255, 255, 255, 224 ) );
	ASSERT_TRUE( s->drawText( canvas::Point(100, 490), "drawLine( 100,500,600,500 )" ) );
	ASSERT_TRUE( s->drawLine( 100, 500, 600, 500 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "figures_and_text" ) ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceCompound, figures_in_surfaces ) {
	canvas::Rect r = canvas::Rect( 0, 0, 720, 576 );
	canvas::Point p = canvas::Point( 0, 0 );
	canvas::Surface* s = getCanvas()->createSurface( r );

	ASSERT_TRUE( s != NULL );

	s->autoFlush( true );

	canvas::Surface* s1 = getCanvas()->createSurface( r );
	s1->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s1->fillRect(canvas::Rect(200, 200, 100, 100)) );
	s->blit( p, s1, r );
	getCanvas()->destroy( s1 );

	canvas::Surface* s2 = getCanvas()->createSurface( r );
	s2->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s2->drawRect(canvas::Rect(210, 210, 100, 100)) );
	s->blit( p, s2, r );
	getCanvas()->destroy( s2 );

	canvas::Surface* s3 = getCanvas()->createSurface( r );
	s3->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s3->fillRect(canvas::Rect(220, 220, 100, 100)) );
	s->blit( p, s3, r );
	getCanvas()->destroy( s3 );

	canvas::Surface* s4 = getCanvas()->createSurface( r );
	s4->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s4->fillRoundRect(canvas::Rect(230, 230, 100, 100), 10, 10) );
	s->blit( p, s4, r );
	getCanvas()->destroy( s4 );

	canvas::Surface* s5 = getCanvas()->createSurface( r );
	s5->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s5->drawRoundRect(canvas::Rect(240, 240, 100, 100), 10, 10) );
	s->blit( p, s5, r );
	getCanvas()->destroy( s5 );

	canvas::Surface* s6 = getCanvas()->createSurface( r );
	s6->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s6->fillRoundRect(canvas::Rect(250, 250, 100, 100), 10, 10) );
	s->blit( p, s6, r );
	getCanvas()->destroy( s6 );

	canvas::Surface* s7 = getCanvas()->createSurface( r );
	s7->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s7->fillEllipse( canvas::Point(320, 320), 50, 50, 0, 0 ) );
	s->blit( p, s7, r );
	getCanvas()->destroy( s7 );

	canvas::Surface* s8 = getCanvas()->createSurface( r );
	s8->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s8->drawEllipse( canvas::Point(330, 330), 50, 50, 0, 0 ) );
	s->blit( p, s8, r );
	getCanvas()->destroy( s8 );

	canvas::Surface* s9 = getCanvas()->createSurface( r );
	s9->setColor( canvas::Color( 0, 0, 255, 224 ) );
	ASSERT_TRUE( s9->fillEllipse( canvas::Point(340, 340), 50, 50, 0, 0 ) );
	s->blit( p, s9, r );
	getCanvas()->destroy( s9 );

	canvas::Surface* s10 = getCanvas()->createSurface( r );
	s10->setColor( canvas::Color( 255, 255, 255, 224 ) );
	ASSERT_TRUE( s10->drawLine( 200, 200, 340, 340 ) );
	s->blit( p, s10, r );
	getCanvas()->destroy( s10 );
	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "figures_in_surfaces") ) );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceCompound, image_text ) {
	canvas::Rect r = canvas::Rect( 0, 0, 720, 576 );

	canvas::Surface* s = getCanvas()->createSurface( r );
	ASSERT_TRUE( s != NULL );

	canvas::Surface* image = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( image != NULL );

	s->autoFlush( true );
	ASSERT_TRUE( s->blit(canvas::Point(0, 0), image, canvas::Rect(0,0,500,375) ) );

	canvas::Surface* s1 = getCanvas()->createSurface( r );
	canvas::FontInfo f;
	f.size( 100 );
	s1->setFont( f );
	s1->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s1->drawText(canvas::Point(300,300),"Text") );
	s->blit( canvas::Point(0,0), s1, r );
	getCanvas()->destroy( s1 );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "image_text" ) ) );

	getCanvas()->destroy( image );
	getCanvas()->destroy( s );
}

TEST_F( SurfaceCompound, figure_text ) {
	canvas::Rect r = canvas::Rect( 0, 0, 720, 576 );

	canvas::Surface* s = getCanvas()->createSurface( r );
	ASSERT_TRUE( s != NULL );

	canvas::Surface* image = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	ASSERT_TRUE( image != NULL );

	s->autoFlush( true );

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 6; j++) {
		s->setColor( canvas::Color( (util::BYTE)(j*20), (util::BYTE)(0), (util::BYTE)(i*20), (util::BYTE)(220+(j*i)) ) );
		ASSERT_TRUE( s->fillRect(canvas::Rect(i*90, j*96, 90, 96)) );
		}
	}

	canvas::Surface* s1 = getCanvas()->createSurface( r );
	canvas::FontInfo f;
	f.size( 100 );
	s1->setFont( f );
	s1->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s1->drawText(canvas::Point(300,300),"Text") );
	s->blit( canvas::Point(0,0), s1, r );
	getCanvas()->destroy( s1 );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "figure_text" ) ) );

	getCanvas()->destroy( image );
	getCanvas()->destroy( s );
}

TEST_F( SurfaceCompound, text_text ) {
	canvas::Rect r = canvas::Rect( 0, 0, 720, 576 );

	canvas::FontInfo f;
	f.size( 100 );

	canvas::Surface* s = getCanvas()->createSurface( r );
	ASSERT_TRUE( s != NULL );
	s->autoFlush( true );
	s->setFont( f );
	s->setColor( canvas::Color( 255, 0, 0, 224 ) );
	ASSERT_TRUE( s->drawText(canvas::Point(250,250),"Text") );

	canvas::Surface* s1 = getCanvas()->createSurface( r );
	s1->setFont( f );
	s1->setColor( canvas::Color( 0, 255, 0, 224 ) );
	ASSERT_TRUE( s1->drawText(canvas::Point(300,300),"Text") );
	s->blit( canvas::Point(0,0), s1, r );
	getCanvas()->destroy( s1 );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "text_text" ) ) );

	getCanvas()->destroy( s );
}
