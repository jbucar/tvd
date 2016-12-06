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

#include "videoplayer.h"
#include "../../util.h"
#include "../../../../src/player/videoplayer.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::string VideoPlayer::playerType() const {
    return "VideoPlayer";
}

std::string VideoPlayer::property() const {
    return "basic";
}

player::type::type VideoPlayer::type() const {
    return player::type::video;
}

TEST_F( VideoPlayer, invalid_file ) {
    ASSERT_FALSE ( setProperty(_player, "src", "video.mp4" ) );
}

TEST_F( VideoPlayer, set_src_file ) {
    ASSERT_TRUE( setProperty(_player, "src", util::getImageName("video.mp4") ) );
}

TEST_F( VideoPlayer, set_src_http ) {
    ASSERT_FALSE( setProperty(_player, "src", "http://path/video.mp4") );
    ASSERT_FALSE( setProperty(_player, "src", "HTTP://path/video.mp4") );
    ASSERT_FALSE( setProperty(_player, "src", "hTtP://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_https ) {
    ASSERT_FALSE( setProperty(_player, "src", "https://path/video.mp4") );
    ASSERT_FALSE( setProperty(_player, "src", "HTTPS://path/video.mp4") );
    ASSERT_FALSE( setProperty(_player, "src", "hTtPs://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_rtsp ) {
    ASSERT_TRUE( setProperty(_player, "src", "rtsp://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_rtp ) {
    ASSERT_TRUE( setProperty(_player, "src", "rtp://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_mirror ) {
    ASSERT_FALSE( setProperty(_player, "src", "ncl-mirror://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_sbtvd ) {
    ASSERT_TRUE( setProperty(_player, "src", "sbtvd-ts://path/video.mp4") );
}

TEST_F( VideoPlayer, set_src_invalid_schema ) {
    ASSERT_FALSE( setProperty(_player, "src", "invalid://path/video.mp4" ) );
}
