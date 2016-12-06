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
#include "../../src/fontinfo.h"
#include <algorithm>

TEST( font, default_constructor ) {
	canvas::FontInfo f;
	const canvas::font::family::Type &families = f.families();
	ASSERT_TRUE( std::find(families.begin(), families.end(), DEFAULT_FONT) != families.end() );
	ASSERT_TRUE( f.size() == 10);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
}

TEST( font, constructor_from_families_type ) {
	canvas::font::family::Type families;
	families.push_back( "DejaVuSans" );
	canvas::FontInfo f(families, 20);
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
	ASSERT_TRUE( f.familiesAsString() == "DejaVuSans,Tiresias");
}

TEST( font, constructor_from_families_type_with_duplicates ) {
	canvas::font::family::Type families;
	families.push_back( "DejaVuSans" );
	families.push_back( "DejaVuSans" );	
	canvas::FontInfo f(families, 20);
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
	ASSERT_TRUE( f.familiesAsString() == "DejaVuSans,Tiresias");
}

TEST( font, constructor_from_string ) {
	canvas::FontInfo f("DejaVuSans", 20);
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
	ASSERT_TRUE( f.familiesAsString() == "DejaVuSans,Tiresias");
}

TEST( font, constructor_from_string_with_duplicates ) {
	canvas::FontInfo f("DejaVuSans, Tiresias", 20);
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
	ASSERT_TRUE( f.familiesAsString() == "DejaVuSans,Tiresias");
}

TEST( font, constructor_family_size ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Tiresias");
	canvas::FontInfo f(familiesSet, 20);

	canvas::font::family::Type familiesGet = f.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Tiresias") != familiesGet.end() );
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == false );
	ASSERT_TRUE( f.italic() == false);
	ASSERT_TRUE( f.smallCaps() == false);
}

TEST( font, set_bold_italic_small ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	canvas::FontInfo f(familiesSet, 20);
	f.bold(true);
	f.italic(true);
	f.smallCaps(true);

	canvas::font::family::Type familiesGet = f.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Ubuntu-R") != familiesGet.end() );
	ASSERT_TRUE( f.size() == 20);
	ASSERT_TRUE( f.bold() == true );
	ASSERT_TRUE( f.italic() == true);
	ASSERT_TRUE( f.smallCaps() == true); 
}

TEST( font, change_size ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	canvas::FontInfo f(familiesSet, 20);
	f.size(25);

	canvas::font::family::Type familiesGet = f.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Ubuntu-R") != familiesGet.end() );
	ASSERT_TRUE( f.size() == 25);
}

TEST( font, change_family ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	canvas::FontInfo f(familiesSet, 20);

	canvas::font::family::Type newFamilies;
	newFamilies.push_back("Tiresias");
	f.families(newFamilies);
    
	canvas::font::family::Type familiesGet = f.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Tiresias") != familiesGet.end() );
	ASSERT_TRUE( f.size() == 20);
}

TEST( font, multiples_families ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	familiesSet.push_back("DejaVuSans");
	familiesSet.push_back("Tiresias");
	canvas::FontInfo f(familiesSet, 20);

	canvas::font::family::Type familiesGet = f.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Ubuntu-R") != familiesGet.end() );
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "DejaVuSans") != familiesGet.end() );
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Tiresias") != familiesGet.end() );
	ASSERT_TRUE( f.size() == 20);
}

TEST( font, familiesAsString ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	familiesSet.push_back("DejaVuSans");
	familiesSet.push_back("Tiresias");
	canvas::FontInfo f(familiesSet, 20);
	ASSERT_TRUE( f.familiesAsString() == "Ubuntu-R,DejaVuSans,Tiresias");
}

TEST( font, familiesAsStringWithoutDefault ) {
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Ubuntu-R");
	familiesSet.push_back("DejaVuSans");
	canvas::FontInfo f(familiesSet, 20);

	ASSERT_TRUE( f.familiesAsString() == "Ubuntu-R,DejaVuSans,Tiresias");
}

TEST( font, copy_constructor ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet;
	familiesSet.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet, 20, false, true, false);

	//	Copy font
	canvas::FontInfo f2 = f1;
	const canvas::font::family::Type &familiesGet = f2.families();
	ASSERT_TRUE( std::find(familiesGet.begin(), familiesGet.end(), "Tiresias") != familiesGet.end() );
	ASSERT_TRUE( f2.size() == 20);
	ASSERT_TRUE( f2.bold() == false );
	ASSERT_TRUE( f2.italic() == true );
	ASSERT_TRUE( f2.smallCaps() == false );
}

TEST( font, compare_equal_equal ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, true, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, true, false);

	ASSERT_TRUE( f1 == f2 );
}

TEST( font, compare_equal_equal1 ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	familiesSet1.push_back("Tiresias" );	
	canvas::FontInfo f1(familiesSet1, 20, false, true, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, true, false);

	ASSERT_TRUE( f1 == f2 );
}

TEST( font, compare_not_equal ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, true, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, false, false);

	ASSERT_FALSE( f1 == f2 );
}

TEST( font, compare_less_equal ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, true, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, true, false);

	ASSERT_FALSE( f1 < f2 );
}

TEST( font, compare_less_size_not_equal_size ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, false, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 21, false, false, false);

	ASSERT_TRUE( f1 < f2 );
}

TEST( font, compare_not_equal_bold ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, false, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, true, false, false);

	ASSERT_TRUE( f1 < f2 );
}

TEST( font, compare_not_equal_italic ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, false, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, true, false);

	ASSERT_TRUE( f1 < f2 );
}

TEST( font, compare_not_equal_smallCaps ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, false, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	canvas::FontInfo f2(familiesSet2, 20, false, false, true);

	ASSERT_TRUE( f1 < f2 );
}

TEST( font, compare_not_equal_families ) {
	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet1;
	familiesSet1.push_back("Tiresias" );
	canvas::FontInfo f1(familiesSet1, 20, false, false, false);

	//	Tiresias, no bold, italic, no small caps
	canvas::font::family::Type familiesSet2;
	familiesSet2.push_back("Tiresias" );
	familiesSet2.push_back("Dejavu");
	canvas::FontInfo f2(familiesSet2, 20, false, false, false);

	ASSERT_TRUE( f1 < f2 );
}

