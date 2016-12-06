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

#include "textplayer.h"
#include "../../util.h"
#include "../../../../src/player/textplayer.h"
#include "../../../../src/device.h"

std::string TextPlayer::playerType() const {
	return "TextPlayer";
}

std::string TextPlayer::property() const {
	return "basic";
}

player::type::type TextPlayer::type() const {
	return player::type::text;
}

TEST_F(TextPlayer, Text_basic) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, Text_basic2) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(100,100,620,476) ));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("basic2")));
}

TEST_F(TextPlayer, Text_normal) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "normal"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, Text_size_big) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontSize", 22));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("size_big")));
}

TEST_F(TextPlayer, Text_bold) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontWeight", "bold"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("bold")));
}

TEST_F(TextPlayer, Text_italic) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontStyle", "italic"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("italic")));
}

TEST_F(TextPlayer, DISABLED_Text_smallcaps) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "small-caps"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("smallcaps")));
}

TEST_F(TextPlayer, DISABLED_Text_bold_italic_smallcaps_size) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(_player->setProperty("fontStyle", "italic"));
	ASSERT_TRUE(_player->setProperty("fontWeight", "bold"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "small-caps"));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("bold_italic_smallcaps_size")));
}

TEST_F(TextPlayer, Text_empty) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("empty.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("empty")));
}

TEST_F(TextPlayer, Text_big) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("big.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("big")));
}

TEST_F(TextPlayer, Text_small) {

	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 1));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("small")));

}

TEST_F(TextPlayer, Text_fontFamily_DejaVuSans) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_DejaVuSans")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, Text_fontFamily_normal) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "normal"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_DejaVuSans")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, Text_fontFamily_size_big) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontSize", 22));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_size_big")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("size_big")));
}

TEST_F(TextPlayer, Text_fontFamily_bold) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontWeight", "bold"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_bold")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("bold")));
}

TEST_F(TextPlayer, Text_fontFamily_italic) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontStyle", "italic"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_italic")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("italic")));
}

TEST_F(TextPlayer, DISABLED_Text_fontFamily_smallcaps) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "small-caps"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_smallcaps")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("smallcaps")));
}

TEST_F(TextPlayer, DISABLED_Text_fontFamily_DejaVuSans_bold_italic_smallcaps_size) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(_player->setProperty("fontStyle", "italic"));
	ASSERT_TRUE(_player->setProperty("fontWeight", "bold"));
	ASSERT_TRUE(_player->setProperty("fontVariant", "small-caps"));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_DejaVuSans_bold_italic_smallcaps_size")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("bold_italic_smallcaps_size")));
}

TEST_F(TextPlayer, Text_fontFamily_empty) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("empty.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("empty")));
}

TEST_F(TextPlayer, Text_fontFamily_big) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("big.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("big")));
}

TEST_F(TextPlayer, Text_fontFamily_small) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 1));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("small")));
}

TEST_F(TextPlayer, Text_test) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("test.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontSize", 10));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("test")));
}

TEST_F(TextPlayer, invalid_source) {
	ASSERT_FALSE(_player->setProperty("src", util::getImageName("invalid.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_FALSE(play(_player));
}

TEST_F(TextPlayer, play) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,500, 375)));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	play(_player);
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("play")));
}

TEST_F(TextPlayer, stop) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,500, 375)));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	play(_player);
	stop(_player);
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("stopped")));
}

TEST_F(TextPlayer, invalid_file) {
	ASSERT_FALSE(_player->setProperty("src", util::getImageName("invalid.png")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,500, 375)));
	ASSERT_FALSE(play(_player));
}

TEST_F(TextPlayer, play_after_stop) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,500, 375)));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("play")));

	stop(_player);
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("stopped")));

	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,500, 375)));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("play")));
}

TEST_F(TextPlayer, set_src_file) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
}

TEST_F(TextPlayer, set_src_http) {
	ASSERT_TRUE(_player->setProperty("src", "http://path/some.txt"));
}

TEST_F(TextPlayer, set_src_https) {
	ASSERT_TRUE(_player->setProperty("src", "https://path/some.txt"));
}

TEST_F(TextPlayer, set_src_rtsp) {
	ASSERT_FALSE(_player->setProperty("src", "rtsp://path/some.txt"));
}

TEST_F(TextPlayer, set_src_rtp) {
	ASSERT_FALSE(_player->setProperty("src", "rtp://path/some.txt"));
}

TEST_F(TextPlayer, set_src_mirror) {
	ASSERT_FALSE(_player->setProperty("src", "ncl-mirror://path/some.txt"));
}

TEST_F(TextPlayer, set_src_sbtvd) {
	ASSERT_FALSE(_player->setProperty("src", "sbtvd-ts://path/some.txt"));
}

TEST_F(TextPlayer, set_src_invalid_schema) {
	ASSERT_FALSE(_player->setProperty("src", "invalid://path/some.txt"));
}

TEST_F(TextPlayer, fontFamily_multiples_families) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "DejaVuSans,Verdana"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_DejaVuSans")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, fontFamily_multiples_families2) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "Verdana,DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_Verdana")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("basic")));
}

TEST_F(TextPlayer, fontFamily_multiples_families3) {
	ASSERT_TRUE(_player->setProperty("src", util::getImageName("some.txt")));
	ASSERT_TRUE(_player->setProperty("bounds", canvas::Rect(0,0,720, 576)));
	ASSERT_TRUE(_player->setProperty("fontColor", "red"));
	ASSERT_TRUE(_player->setProperty("fontFamily", "sdkgdfg,adfsdgdfd,DejaVuSans"));
	ASSERT_TRUE(_player->setProperty("fontSize", 14));
	ASSERT_TRUE(play(_player));
	ASSERT_TRUE(compareImages(canvas(), getExpectedPath("fontFamily_DejaVuSans")));
	ASSERT_FALSE(compareImages(canvas(), getExpectedPath("basic")));
}
