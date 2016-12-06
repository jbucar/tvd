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

#include <gtest/gtest.h>
#include "../../src/color.h"

TEST( color, default_constructor ) {
    canvas::Color color;
    ASSERT_TRUE( color.r == 0 );
    ASSERT_TRUE( color.g == 0 );
    ASSERT_TRUE( color.b == 0 );
    ASSERT_TRUE( color.alpha == 0 );
}

TEST( color, constructor_with_alpha ) {
    canvas::Color color(25, 54, 46, 128);
    ASSERT_TRUE( color.r == 25 );
    ASSERT_TRUE( color.g == 54 );
    ASSERT_TRUE( color.b == 46 );
    ASSERT_TRUE( color.alpha == 128 );
}

TEST( color, constructor_without_alpha ) {
    canvas::Color color(32, 43, 78);
    ASSERT_TRUE( color.r == 32 );
    ASSERT_TRUE( color.g == 43 );
    ASSERT_TRUE( color.b == 78 );
    ASSERT_TRUE( color.alpha == 255 );
}

TEST( color, equals_no_threshold ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(0, 0, 0);
    ASSERT_TRUE( color1.equals( color2, 0 ) );
    ASSERT_TRUE( color2.equals( color1, 0 ) );
}

TEST( color, not_equals_no_threshold ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(1, 0, 0);
    ASSERT_FALSE( color1.equals( color2, 0 ) );
    ASSERT_FALSE( color2.equals( color1, 0 ) );
}

TEST( color, equals_with_threshold_1 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(1, 0, 0);
    ASSERT_TRUE( color1.equals( color2, 1 ) );
    ASSERT_TRUE( color2.equals( color1, 1 ) );
}

TEST( color, not_equals_with_threshold_1 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(2, 0, 0);
    ASSERT_FALSE( color1.equals( color2, 1 ) );
    ASSERT_FALSE( color2.equals( color1, 1 ) );
}

TEST( color, equals_with_threshold_2 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(2, 0, 0);
    ASSERT_TRUE( color1.equals( color2, 2 ) );
    ASSERT_TRUE( color2.equals( color1, 2 ) );
}

TEST( color, not_equals_with_threshold_2 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(3, 1, 0);
    ASSERT_FALSE( color1.equals( color2, 2 ) );
    ASSERT_FALSE( color2.equals( color1, 2 ) );
}

TEST( color, equals_with_threshold_3 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(2, 1, 0);
    ASSERT_TRUE( color1.equals( color2, 3 ) );
    ASSERT_TRUE( color2.equals( color1, 3 ) );
}

TEST( color, not_equals_with_threshold_3 ) {
    canvas::Color color1(0, 0, 0);
    canvas::Color color2(4, 1, 1);
    ASSERT_FALSE( color1.equals( color2, 3 ) );
    ASSERT_FALSE( color2.equals( color1, 3 ) );
}





