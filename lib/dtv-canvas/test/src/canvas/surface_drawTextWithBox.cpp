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

class SurfaceDrawTextWithBox : public Surface {
protected:
	std::string getSubDirectory() const { return "DrawTextWithBox"; }

	virtual void SetUp() {
		Surface::SetUp();
		getCanvas()->addFontDirectory( util::getFontRoot() );
	}
};

// Draw a basic text with box in the surface
TEST_F( SurfaceDrawTextWithBox, basic){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}

// Draw a basic text with box in the surface and not set font
TEST_F( SurfaceDrawTextWithBox, basic_not_set_font){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	
	canvas::Rect box(0, 0, 250, 250);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}

// Draw a basic text with box in the surface without text
TEST_F( SurfaceDrawTextWithBox, basic_without_text){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box, "") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic_without_text" ) ) );

	getCanvas()->destroy( s );
}

// Draw bold text with box in the surface
TEST_F( SurfaceDrawTextWithBox, bold){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	//f.family("Tiresias");
	canvas::font::family::Type families;
	families.push_back("Tiresias");
	f.families(families);
	f.bold(true);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );
	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bold" ) ) );
	ASSERT_FALSE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}


// Draw text with box in the surface with specific size
TEST_F( SurfaceDrawTextWithBox, size){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	f.size(24);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "size" ) ) );

	getCanvas()->destroy( s );
}


// Draw text with box in the surface with specific size 1
TEST_F( SurfaceDrawTextWithBox, size_1){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	f.size(1);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "size_1" ) ) );

	getCanvas()->destroy( s );
}

// Draw italic text with box in the surface
TEST_F( SurfaceDrawTextWithBox, italic){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	f.italic(true);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "italic" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box in the surface with Tiresias family font
TEST_F( SurfaceDrawTextWithBox, family){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;
	
	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	//f.family("Tiresias");
	canvas::font::family::Type families;
	families.push_back("Tiresias");
	f.families(families);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "family" ) ) );

	getCanvas()->destroy( s );
}



// Draw smallcaps text with box in the surface
TEST_F( SurfaceDrawTextWithBox, DISABLED_smallcaps){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;
	
	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	f.smallCaps(true);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "smallcaps" ) ) );

	getCanvas()->destroy( s );
}

// Draw bold,italic,smallcaps text with box in the surface with specific size and family font
TEST_F( SurfaceDrawTextWithBox, DISABLED_bold_italic_smallcaps_size_family){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;
	
	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	f.bold(true);
	f.italic(true);
	f.smallCaps(true);
	f.size(24);
	//f.family("Tiresias");
	canvas::font::family::Type families;
	families.push_back("Tiresias");
	f.families(families);
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bold_italic_smallcaps_size_family" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the top left corner of the surface
TEST_F( SurfaceDrawTextWithBox, top_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the top of the surface
TEST_F( SurfaceDrawTextWithBox, top){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	int x = (720 - size.w) / 2;
	canvas::Rect box(x, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "top" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the top right corner of the surface
TEST_F( SurfaceDrawTextWithBox, top_right){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(720 - size.w, 0, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "top_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the right side of the surface
TEST_F( SurfaceDrawTextWithBox, right){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	int x = 720 - size.w;
	int y = (576 - size.h) / 2;
	canvas::Rect box(x, y, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "right" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the bottom right corner of the surface
TEST_F( SurfaceDrawTextWithBox, bottom_right){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(720 - size.w, 576 - size.h, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bottom_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the bottom of the surface
TEST_F( SurfaceDrawTextWithBox, bottom){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	int x = (720 - size.w) / 2;
	canvas::Rect box(x, 576 - size.h, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bottom" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the bottom left corner of the surface
TEST_F( SurfaceDrawTextWithBox, bottom_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 576 - size.h, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bottom_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw text with box on the left side of the surface
TEST_F( SurfaceDrawTextWithBox, left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	int y = (576 - size.h) / 2;
	canvas::Rect box(0, y, size.w, size.h);
	ASSERT_TRUE( s->drawText(box ,"Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "left" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text with box completely out of the surface (top-left)
TEST_F( SurfaceDrawTextWithBox, outside_top_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	canvas::Rect box( -size.w, -size.h, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box completely out of the surface (top)
TEST_F( SurfaceDrawTextWithBox, outisde_top ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) / 2;
	canvas::Rect box( x, -size.h, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box completely out of the surface (top-right)
TEST_F( SurfaceDrawTextWithBox, outside_top_right){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	canvas::Rect box( 576, -size.h, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box completely out of the surface (right)
TEST_F( SurfaceDrawTextWithBox, outside_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int y = (576 - size.h) / 2;
	canvas::Rect box( 720, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box completely out of the surface (bottom-right)
TEST_F( SurfaceDrawTextWithBox, outside_bottom_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	canvas::Rect box( 720, 576, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box completely out of the surface (bottom)
TEST_F( SurfaceDrawTextWithBox, outside_bottom ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) / 2;
	canvas::Rect box( x, 576, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box completely out of the surface (bottom-left)
TEST_F( SurfaceDrawTextWithBox, outside_bottom_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	canvas::Rect box( -size.w, 576, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box completely out of the surface (left)
TEST_F( SurfaceDrawTextWithBox, outside_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int y = (576 - size.h) / 2;
	canvas::Rect box( -size.w, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box part outside the surface (top-left)
TEST_F( SurfaceDrawTextWithBox, part_outside_top_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	canvas::Rect box( -1, -1, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box part outside the surface (top)
TEST_F( SurfaceDrawTextWithBox, part_outisde_top ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) / 2;
	canvas::Rect box( x, -1, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box part outside the surface (top-right)
TEST_F( SurfaceDrawTextWithBox, part_outside_top_right){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) + 1;
	canvas::Rect box( x, -1, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box part outside the surface (right)
TEST_F( SurfaceDrawTextWithBox, part_outside_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) + 1;
	int y = (576 - size.h) / 2;
	canvas::Rect box( x, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box part outside the surface (bottom-right)
TEST_F( SurfaceDrawTextWithBox, part_outside_bottom_right ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) + 1;
	int y = (576 - size.h) + 1;
	canvas::Rect box( x, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box part outside the surface (bottom)
TEST_F( SurfaceDrawTextWithBox, part_outside_bottom ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int x = (720 - size.w) / 2;
	int y = (576 - size.h) + 1;
	canvas::Rect box( x, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text with box part outside the surface (bottom-left)
TEST_F( SurfaceDrawTextWithBox, part_outside_bottom_left){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int y = (576 - size.h) + 1;
	canvas::Rect box( -1, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}


// Draw a text with box part outside the surface (left)
TEST_F( SurfaceDrawTextWithBox, part_outside_left ) {
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size );

	int y = (576 - size.h) / 2;
	canvas::Rect box( -1, y, size.w, size.h );
	ASSERT_FALSE( s->drawText(box, "Hola Mundo !!!") );

	getCanvas()->destroy( s );
}

// Draw a text larger than the box width
TEST_F( SurfaceDrawTextWithBox, larger_than_width){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("AAAAAA", size);

	canvas::Rect box(0, 0, size.w - (size.w/2), size.h);
	ASSERT_TRUE( s->drawText(box, "AAAAAA") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "larger_than_width" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text larger than the box height
TEST_F( SurfaceDrawTextWithBox, larger_than_height){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Holaaaaaaaaa", size);

	canvas::Rect box(0, 0, size.w, size.h - (size.h / 2) );
	ASSERT_TRUE( s->drawText(box, "Holaaaaaaaaa") );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a text larger than the box height and width
TEST_F( SurfaceDrawTextWithBox, larger_than_height_and_width){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Holaaaaaaaaa", size);

	canvas::Rect box(0, 0, size.w - (size.w / 2), size.h - (size.h / 2) );
	ASSERT_TRUE( s->drawText(box, "Holaaaaaaaaa") );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a text larger than the box height and width with multiple words
TEST_F( SurfaceDrawTextWithBox, larger_than_height_and_width_multiple_words ){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w - (size.w / 2), (size.h * 2) - (size.h / 2) );
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "larger_than_height_and_width_multiple_words" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text shorter than the box width
TEST_F( SurfaceDrawTextWithBox, shorter_than_width){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w + 100, size.h);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text shorter than the box height
TEST_F( SurfaceDrawTextWithBox, shorter_than_height){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w, size.h + 100);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text shorter than the box width and height
TEST_F( SurfaceDrawTextWithBox, shorter_than_width_and_height){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo !!!", size);

	canvas::Rect box(0, 0, size.w + 100, size.h + 100);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo !!!") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text with multiple words, larger than the box width.
TEST_F( SurfaceDrawTextWithBox, larger_than_width_height_ok){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola mundo nuevamente", size);

	canvas::Rect box(0, 0, size.w - (size.w / 2), size.h * 3);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo nuevamente") );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "larger_than_width_hegiht_ok" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text with box in the surface with spacing grater than 0
TEST_F( SurfaceDrawTextWithBox, with_spacing_gt_0){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo", size);

	canvas::Rect box(0, 0, (size.w+10) / 2, (size.h+11) * 2);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo", canvas::wrapAnywhere, 10) );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "spacing_gt_0" ) ) );

	getCanvas()->destroy( s );
}

// Draw a text with box in the surface with spacing lower than 0
TEST_F( SurfaceDrawTextWithBox, with_spacing_lt_0){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo", size);

	canvas::Rect box(0, 0, (size.w+10) / 2, size.h * 2);
	ASSERT_FALSE( s->drawText(box, "Hola Mundo", canvas::wrapAnywhere, -10) );

	getCanvas()->destroy( s );
}

// Draw a text with box in the surface with spacing equal than 0
TEST_F( SurfaceDrawTextWithBox, with_spacing_eq_0){
	canvas::Rect rect(0, 0, 720, 576);
	canvas::Surface* s = getCanvas()->createSurface(rect);
	canvas::FontInfo f;
	canvas::Size size;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(f);
	s->measureText("Hola Mundo", size);

	canvas::Rect box(0, 0, (size.w+10) / 2, size.h * 2);
	ASSERT_TRUE( s->drawText(box, "Hola Mundo", canvas::wrapAnywhere, 0) );

	ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "spacing_eq_0" ) ) );

	getCanvas()->destroy( s );
}
