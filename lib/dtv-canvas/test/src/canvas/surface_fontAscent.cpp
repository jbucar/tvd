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

#define THRESHOLD 3

static bool compare( int value1, int value2, int tolerance ) {
    return abs(value1-value2) <= tolerance;
}

class SurfaceFontAscent : public Surface {
protected:
	virtual void SetUp() {
		Surface::SetUp();
		getCanvas()->addFontDirectory( util::getFontRoot() );
	}
};

TEST_F(SurfaceFontAscent, without_font_family_setted_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare (s->fontAscent(), 20, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted_bold_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(20);
    f.bold(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare(s->fontAscent(), 20, THRESHOLD ));
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.italic(true);
    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted_bold_and_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(20);
    f.bold(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) ); 
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(20);
    f.smallCaps(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted_bold_italic_and_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(20);
    f.bold(true);
    f.smallCaps(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 10, THRESHOLD) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, without_font_family_setted_size50) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    f.size(50);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 50, THRESHOLD) );
    
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_tiresias_family_setted_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_tiresias_family_setted_bold_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(20);
    f.bold(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_tiresias_family_setted_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.italic(true);
    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_tiresias_family_setted_bold_and_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(20);
    f.bold(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD) );
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_tiresias_family_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(20);
    f.smallCaps(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD ) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_tiresias_family_setted_bold_italic_and_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(20);
    f.bold(true);
    f.smallCaps(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 20, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_tiresias_family_setted) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 10, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_tiresias_family_setted_size50) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("Tiresias");
    f.families(families);
    f.size(50);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 50, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}



 TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    

    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_bold_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(20);
    f.bold(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.italic(true);
    f.size(20);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_bold_and_italic_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(20);
    f.bold(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_dejavusans_family_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(20);
    f.smallCaps(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_bold_italic_and_smallcaps_size20) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(20);
    f.bold(true);
    f.smallCaps(true);
    f.italic(true);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    
    
    ASSERT_TRUE( compare( s->fontAscent(), 19, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}


TEST_F(SurfaceFontAscent, with_dejavusans_family_setted) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );

    
    ASSERT_TRUE( compare( s->fontAscent(), 10, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}



TEST_F(SurfaceFontAscent, with_dejavusans_family_setted_size50) {
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;

    canvas::font::family::Type families;
    families.push_back("DejaVuSans");
    f.families(families);
    f.size(50);
    s->setFont(f);
    
    ASSERT_TRUE( s != NULL );
    ASSERT_TRUE( compare( s->fontAscent(), 47, THRESHOLD ) );
    
    
    getCanvas()->destroy(s);   
}





