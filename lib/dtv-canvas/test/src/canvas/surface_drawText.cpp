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
#include <boost/math/special_functions/round.hpp>

class SurfaceDrawText : public Surface {
protected:
	std::string getSubDirectory() const { return "DrawText"; }

	virtual void SetUp() {
		Surface::SetUp();
		getCanvas()->addFontDirectory( util::getFontRoot() );
	}
};

// Draw a basic text in the surface
TEST_F( SurfaceDrawText, basic){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "basic" ) ) );

    getCanvas()->destroy(s);
}

// Draw a basic text in the surface and not set font

TEST_F(SurfaceDrawText, basic_not_set_font) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));

    ASSERT_TRUE(s->drawText(canvas::Point(250, 250), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "basic_not_set_font" ) ) );

    getCanvas()->destroy(s);
}

// Draw a basic text in the surface without text

TEST_F(SurfaceDrawText, basic_without_text) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE( s->drawText(canvas::Point(x, y), "" ));
    ASSERT_TRUE(util::compareImages(getCanvas(), "nothing"));

    getCanvas()->destroy(s);
}

// Draw bold text in the surface
TEST_F( SurfaceDrawText, bold){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.bold(true);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "bold" ) ) );
    ASSERT_FALSE(util::compareImages(getCanvas(), getExpectedPath( "basic" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size
TEST_F( SurfaceDrawText, size){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.size(24);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "size" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size 1
TEST_F( SurfaceDrawText, size_1){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.size(1);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "size_1" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size 380
TEST_F(SurfaceDrawText, size_380) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;
	canvas::Size tam;
	int y, x;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	f.size(380);
	s->setFont(f);
    
	int ascent = s->fontAscent();
	s->measureText("II--", tam);

	y = ascent + boost::math::iround((s->getSize().h - tam.h) / 2.0);
	x = boost::math::iround((s->getSize().w - tam.w) / 2.0);

	ASSERT_TRUE(s->drawText(canvas::Point(x, y), "II--"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "size_380" ) ) );

	getCanvas()->destroy(s);
}

// Draw italic text in the surface
TEST_F( SurfaceDrawText, italic){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.italic(true);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "italic" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with Tiresias family font
TEST_F( SurfaceDrawText, family){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "family" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with Tiresias family font
TEST_F( SurfaceDrawText, inexistent_family) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    canvas::font::family::Type families;
    families.push_back("pepelui");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "family" ) ) );

    getCanvas()->destroy(s);
}

// Draw smallcaps text in the surface

TEST_F(SurfaceDrawText, DISABLED_smallcaps) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.smallCaps(true);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "smallcaps" ) ) );

    getCanvas()->destroy(s);
}

// Draw bold,italic,smallcaps text in the surface with specific size and family font

TEST_F(SurfaceDrawText, DISABLED_bold_italic_smallcaps_size_family) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.bold(true);
    f.italic(true);
    f.smallCaps(true);
    f.size(24);
    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "bold_italic_smallcaps_size_family" ) ) );

    getCanvas()->destroy(s);
}

// Draw text on the top right corner of the surface
TEST_F( SurfaceDrawText, top_right){
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	s->setFont(f);

	canvas::Size tam;
	s->measureText("Hola Mundo !!!", tam);
	int y = s->fontAscent();
	int x = s->getSize().w - tam.w;

	ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "top_right" ) ) );

	getCanvas()->destroy(s);
}

// Draw text on the top left corner of the surface
TEST_F( SurfaceDrawText, top_left){
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	s->setFont(f);

	int y = s->fontAscent();

	ASSERT_TRUE(s->drawText(canvas::Point(0, y), "Hola Mundo !!!"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy(s);
}

// Draw text on the bottom right corner of the surface
TEST_F( SurfaceDrawText, bottom_right){
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	s->setFont(f);

	canvas::Size tam;
	s->measureText("Hola Mundo !!!", tam);
	int y = 575 - s->fontDescent();
	int x = s->getSize().w - tam.w;

	ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "bottom_right" ) ) );

	getCanvas()->destroy(s);
}

// Draw text on the bottom left corner of the surface
TEST_F( SurfaceDrawText, bottom_left){
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	s->setFont(f);

	int y = 575 - s->fontDescent();

	ASSERT_TRUE(s->drawText(canvas::Point(0, y), "Hola Mundo !!!"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "bottom_left" ) ) );

	getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (top-left)
TEST_F(SurfaceDrawText, outside_top_left) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = -tam.h;
    x = -tam.w;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (top-right)
TEST_F(SurfaceDrawText, outside_top_right) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = -tam.h;
    x = s->getSize().w;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (bottom-right)

TEST_F(SurfaceDrawText, outside_bottom_right) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = s->getSize().h + 1;
    x = s->getSize().w + 1;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );


    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (bottom-left)
TEST_F(SurfaceDrawText, outside_bottom_left) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = s->getSize().h + 1;
    x = -tam.w;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );


    getCanvas()->destroy(s);
}

// Draw a text with some part outside of the surface (bottom-left)
TEST_F(SurfaceDrawText, some_part_outside_bottom_left) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = s->getSize().h - tam.h + 1;
    x = -1;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text with some part outside of the surface (bottom-right)
TEST_F(SurfaceDrawText, some_part_outside_bottom_right) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	s->setFont(f);

	canvas::Size tam;
	s->measureText("Hola Mundo !!!", tam);
	int y = s->getSize().h - s->fontDescent() - 1;
	int x = s->getSize().w - tam.w + 5;

	ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "bottom_right_outside" ) ) );

	getCanvas()->destroy(s);
}

// Draw a text with some part outside of the surface (top-right)
TEST_F(SurfaceDrawText, some_part_outside_top_right) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = -1;
    x = s->getSize().w - tam.w + 1;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text with some part outside of the surface (top-left)
TEST_F(SurfaceDrawText, some_part_outside_top_left) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = -1;
    x = -1;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (top)
TEST_F(SurfaceDrawText, completely_out_top) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = -tam.h;
    x = boost::math::iround(s->getSize().w / 2.0)-boost::math::iround(tam.w / 2.0);
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (bottom)
TEST_F(SurfaceDrawText, completely_out_bottom) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = s->getSize().h;
    x = boost::math::iround(s->getSize().w / 2.0)-boost::math::iround(tam.w / 2.0);
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (left)
TEST_F(SurfaceDrawText, completely_out_left) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround(s->getSize().h / 2.0)-boost::math::iround(tam.h / 2.0);
    x = -(s->getSize().w);
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a text completely out of the surface (right)
TEST_F(SurfaceDrawText, completely_out_right) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround(s->getSize().h / 2.0)-boost::math::iround(tam.h / 2.0);
    x = s->getSize().w;
    ASSERT_FALSE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!") );

    getCanvas()->destroy(s);
}

// Draw a basic text in the surface
TEST_F( SurfaceDrawText, DejaVuSans_basic){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));

    ASSERT_TRUE( util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_basic" ) ) );
    ASSERT_FALSE( util::compareImages(getCanvas(), getExpectedPath( "basic" ) ) );

    getCanvas()->destroy(s);
}

// Draw a basic text in the surface without text
TEST_F(SurfaceDrawText, DejaVuSans_basic_without_text) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE( s->drawText(canvas::Point(x, y), "" ));

    ASSERT_TRUE(util::compareImages(getCanvas(), "nothing" ) );

    getCanvas()->destroy(s);
}

// Draw bold text in the surface
TEST_F( SurfaceDrawText, DejaVuSans_bold){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.bold(true);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));

    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_bold" ) ) );
    ASSERT_FALSE(util::compareImages(getCanvas(), getExpectedPath( "bold" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size
TEST_F( SurfaceDrawText, DejaVuSans_size){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.size(24);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_size" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size 1
TEST_F( SurfaceDrawText, DejaVuSans_size_1){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.size(1);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_size_1" ) ) );

    getCanvas()->destroy(s);
}

// Draw text in the surface with specific size 400
TEST_F(SurfaceDrawText, DejaVuSans_size_400) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	canvas::FontInfo f;
	canvas::Size tam;
	int y, x;

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	f.size(360);
	canvas::font::family::Type families;
	families.push_back("DejaVuSans");
	f.families(families);
	s->setFont(f);
	int descent = s->fontDescent();
	s->measureText("II--", tam);
	y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h - descent;
	x = boost::math::iround((s->getSize().w - tam.w) / 2.0);

	ASSERT_TRUE(s->drawText(canvas::Point(x, y), "II--"));
	ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_size_400" ) ) );

	getCanvas()->destroy(s);
}

// Draw italic text in the surface
TEST_F( SurfaceDrawText, DejaVuSans_italic){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.italic(true);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_italic" ) ) );

    getCanvas()->destroy(s);
}

// Draw smallcaps text in the surface
TEST_F( SurfaceDrawText, DISABLED_DejaVuSans_smallcaps){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.smallCaps(true);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_smallcaps" ) ) );

    getCanvas()->destroy(s);
}

// Draw bold,italic,smallcaps text in the surface with specific size and family font
TEST_F( SurfaceDrawText, DISABLED_DejaVuSans_bold_italic_smallcaps_size_family){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    canvas::Size tam;
    int y, x;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    f.bold(true);
    f.italic(true);
    f.smallCaps(true);
    f.size(24);
    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "DejaVuSans_bold_italic_smallcaps_size_family" ) ) );

    getCanvas()->destroy(s);
}

// Draw a basic text with newline in the surface 
TEST_F(SurfaceDrawText, with_newline) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!! \n Hola ginga!!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!! \n Hola ginga!!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "with_newline" ) ) );

    getCanvas()->destroy(s);   
}

// Draw a basic text with indentation in the surface
TEST_F(SurfaceDrawText, with_indentation) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!\tHola ginga!!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!\tHola ginga!!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "with_indentation" ) ) );

    getCanvas()->destroy(s);   
}

// Test markDirty after drawing a text
TEST_F( SurfaceDrawText, markDirty_drawText ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	s->setFont(canvas::FontInfo() );
	getCanvas()->flush();

	canvas::Size size;
	canvas::Point p1(50,50), p2(100,100);

	ASSERT_TRUE( s->measureText("Hola", size ) );
	ASSERT_TRUE( s->drawText( p1, "Hola" ) );
	ASSERT_TRUE( s->drawText( p2, "" ) );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_EQ( 1, dirtyRegions.size() );

	int ascent = s->fontAscent();
	canvas::Rect r(50,50-ascent,size.w,size.h);
	ASSERT_TRUE( dirtyRegions.at(0) == r );

	getCanvas()->destroy( s );
}

TEST_F( SurfaceDrawText, resize){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    int y, x;
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255, 0, 0));
    s->setFont(f);
    s->measureText("Hola Mundo !!!", tam);
    y = boost::math::iround((s->getSize().h - tam.h) / 2.0) + tam.h;
    x = boost::math::iround((s->getSize().w - tam.w) / 2.0);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "basic" ) ) );

    canvas::Size size(720, 800);
    s->resize(size);
    ASSERT_TRUE(s->drawText(canvas::Point(x, y), "Hola Mundo !!!"));
    ASSERT_TRUE(util::compareImages(getCanvas(), getExpectedPath( "basic" ) ) );

    getCanvas()->destroy(s);
}
