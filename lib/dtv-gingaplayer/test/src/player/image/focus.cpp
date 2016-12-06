/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "imageplayer.h"
#include "../../util.h"
#include "../../../../src/player.h"
#include "../../../../src/system.h"
#include "../../../../src/player/imageplayer.h"
#include <canvas/rect.h>

class ImagePlayerFocus : public ImagePlayer {
protected:
    std::string property() const { return "focus"; };
};


TEST_F( ImagePlayerFocus, invalid_values ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	
	//	Invalid values
	ASSERT_FALSE ( setProperty( _player, "focus", "invalidValue") );
	ASSERT_FALSE ( setProperty( _player, "focusBorderColor", "invalidColourValue") );
	ASSERT_FALSE ( setProperty( _player, "selBorderColor", "invalidColourValue") );
	ASSERT_FALSE ( setProperty( _player, "focusBorderWidth", 1.1f) );
	ASSERT_FALSE ( setProperty( _player, "focusBorderWidth", -1.0f) );
	ASSERT_FALSE ( setProperty( _player, "focusBorderWidth", "InvalidValue") );
	ASSERT_FALSE ( setProperty( _player, "focusBorderTransparency", 2.0f) );
	ASSERT_FALSE ( setProperty( _player, "focusSrc", util::getImageName("invalidValue")) );
	ASSERT_FALSE ( setProperty( _player, "focusSelSrc", util::getImageName("invalidValue")) );
}

TEST_F( ImagePlayerFocus,  with_focus_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );	
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );		
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus, without_focus_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );	
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );			
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "focus", false ) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, on_off_on ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) ); 
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "focus", false ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );	
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus, selected_without_focus ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, with_focus ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );			
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus, without_focus) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", false ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, with_focus_white ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "white") );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus_white" ) ) );
}

TEST_F( ImagePlayerFocus, with_focus_selected ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true) );
	ASSERT_TRUE ( setProperty( _player, "selBorderColor", "green") );
		
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus, with_focus_selected__with_source_and_border ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSelSrc", util::getImageName("green.jpg")) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", -10) );
 	ASSERT_TRUE ( setProperty( _player, "selBorderColor", "blue") );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus_selected__with_source_and_border" ) ) );
}

TEST_F( ImagePlayerFocus, selected_when_no_focus) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", false) );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true) );
		
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, borderWidthInZero) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 0) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, borderWidthIn10) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("withBorderWidthIn10" ) ) );
}

TEST_F( ImagePlayerFocus, borderWidthInMinus10 ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", -10) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("withBorderWidthInMinus10" ) ) );
}

TEST_F( ImagePlayerFocus,  withBorderTransparencyIn0) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );	
	ASSERT_TRUE ( setProperty( _player, "focusBorderTransparency", 0.0f) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus,  withTotalBorderTransparency) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderTransparency", 1.0f) );
		
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus, withBorderTransparency) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderTransparency", 0.5f) );
		
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("50%BoderTransparency" ) ) );
}

TEST_F( ImagePlayerFocus, focus_src) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSrc", util::getImageName("green.jpg")) );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("green_square_with_focus" ) ) );
}

TEST_F( ImagePlayerFocus,  srcWithoutFocus) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", false) );
	ASSERT_TRUE ( setProperty( _player, "focusSrc", util::getImageName("green.jpg")) );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("without_focus" ) ) );
}

TEST_F( ImagePlayerFocus,  selSrcNoSelected) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );			
	ASSERT_TRUE ( setProperty( _player, "focusSelSrc", util::getImageName("green.jpg")) );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("with_focus" ) ) );
}

TEST_F( ImagePlayerFocus,  srcAndSelSrcSelected) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSrc", util::getImageName("yellow.jpg")) );
	ASSERT_TRUE ( setProperty( _player, "focusSelSrc", util::getImageName("green.jpg")) );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("green_square_with_focus" ) ) );
}

TEST_F( ImagePlayerFocus,  selSrc) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSelected", true) );
	ASSERT_TRUE ( setProperty( _player, "focusSelSrc", util::getImageName("green.jpg")) );
	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("green_square_with_focus" ) ) );
}

TEST_F( ImagePlayerFocus, limit_left_top_corner ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_left_top_corner" ) ) );
}

TEST_F( ImagePlayerFocus, limit_top_side ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(310,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_top_side" ) ) );
}

TEST_F( ImagePlayerFocus, limit_right_top_corner ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_right_top_corner" ) ) );
}

TEST_F( ImagePlayerFocus, limit_right_side ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,238,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_right_side" ) ) );
}

TEST_F( ImagePlayerFocus, limit_right_bottom_corner ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,476,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_right_bottom_corner" ) ) );
}

TEST_F( ImagePlayerFocus, limit_bottom_side) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(310,476,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_bottom_side" ) ) );
}

TEST_F( ImagePlayerFocus, limit_left_bottom_corner) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,476,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_left_bottom_corner" ) ) );
}

TEST_F( ImagePlayerFocus, limit_left_side) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,238,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );	
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("limit_left_side" ) ) );
}

TEST_F( ImagePlayerFocus, borderWidthIn10_resize_player) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 10) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("withBorderWidthIn10" ) ) );

	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,50,50) ) );
	ASSERT_TRUE( compareImages(canvas(), getExpectedPath("withBorderWidthIn10_2")) );

	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,200,200) ) );
	ASSERT_TRUE( compareImages(canvas(), getExpectedPath("withBorderWidthIn10_3")) );
}

TEST_F( ImagePlayerFocus, player_with_focus_and_same_zindex) {
	player::Player* player2 = createPlayer( player::type::image );
	player::Player* player3 = createPlayer( player::type::image );

	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,200,150) ) );
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "zIndex", 2 ) );
	ASSERT_TRUE ( setProperty( _player, "focusBorderWidth", 4) );
	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(200,200,200,150) ) );
	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("green.jpg") ) );
	ASSERT_TRUE ( setProperty( player2, "zIndex", 2 ) );
	ASSERT_TRUE ( setProperty( player2, "focusBorderWidth", 4) );
	ASSERT_TRUE ( setProperty( player3, "bounds", canvas::Rect(300,300,200,150) ) );
	ASSERT_TRUE ( setProperty( player3, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( player3, "zIndex", 2 ) );
	ASSERT_TRUE ( setProperty( player3, "focusBorderWidth", 4) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( play(player2) );
	ASSERT_TRUE ( play(player3) );


	ASSERT_TRUE ( setProperty( player3, "focus", true) );
	ASSERT_TRUE ( setProperty( _player, "focus", true) );
	ASSERT_TRUE ( setProperty( player2, "focus", true) );

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("withBorderSameZIndex" ) ) );

	destroyPlayer(player2);
	destroyPlayer(player3);
}
