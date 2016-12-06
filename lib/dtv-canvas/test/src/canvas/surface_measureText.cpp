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

class SurfaceMeasureText : public Surface {
protected:
    std::string getSubDirectory() const { return "MeasureText"; }
};

#define THRESHOLD 3

// Set font and then draw a basic text in the surface 
TEST_F( SurfaceMeasureText, basic_setFont){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255,0,0));
    f.size(10);
    s->setFont(f);
    ASSERT_TRUE(s->measureText("Hola Mundo !!!", tam));

    ASSERT_LE( abs(tam.h-13), THRESHOLD );
    ASSERT_LE( abs(tam.w-67), THRESHOLD );

    getCanvas()->destroy( s );
}

// Draw a basic text in the surface
TEST_F( SurfaceMeasureText, basic){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setColor(canvas::Color(255,0,0));    
    ASSERT_TRUE(s->measureText("Hola Mundo !!!", tam));

    ASSERT_LE( abs(tam.h-13), THRESHOLD );
    ASSERT_LE( abs(tam.w-67), THRESHOLD );

    getCanvas()->destroy( s );
}

// Call measureText with empty text
TEST_F( SurfaceMeasureText, null){
    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
    canvas::FontInfo f;
    
    canvas::Size tam;

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);
    s->setFont(f);
    s->measureText("", tam);
    ASSERT_LE( abs(tam.h-12), THRESHOLD );
    ASSERT_EQ(0, tam.w);

    getCanvas()->destroy( s );
}

// Draw a basic text with ñÑáÁéÉíÍóÓÚ
TEST_F( SurfaceMeasureText, spetial_symbols) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));
	ASSERT_TRUE( s != NULL );

	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));

	canvas::Size tam1;
	canvas::Size tam2;
	ASSERT_TRUE(s->measureText("ñÑáÁéÉíÍóÓúÚ", tam1));
	ASSERT_TRUE(s->measureText("nNaAeEiIoOuU", tam2));
	ASSERT_LE( abs(tam1.h-tam2.h), THRESHOLD );
	ASSERT_LE( abs(tam1.w-tam2.w), THRESHOLD );

	getCanvas()->destroy( s );
}
