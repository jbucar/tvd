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
#include "../../../../src/player/imageplayer.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class ImagePlayerFit : public ImagePlayer {
protected:
    std::string property() const { return "fit"; };

    std::string getFitExpectedPath( std::string subProperty, std::string fileName ) const {
        fs::path root;
        root = getExpectedPath(subProperty);
        root /= fileName;
        return root.string();
    }
};

TEST_F( ImagePlayerFit, hidden_equal_size ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );		
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("hidden", "equal_size") ) );
}

TEST_F( ImagePlayerFit, hidden_smaller ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "smaller") ) );
}

TEST_F( ImagePlayerFit, hidden_bigger ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "bigger") ) );
}

TEST_F( ImagePlayerFit, hidden_equal_size_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("hidden", "equal_size" )  ) );
}

TEST_F( ImagePlayerFit, hidden_smaller_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "smaller") ) );
}

TEST_F( ImagePlayerFit, hidden_bigger_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "bigger") ) );
}

TEST_F( ImagePlayerFit, hidden_equal_size_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("hidden", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, hidden_smaller_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, hidden_bigger_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "bigger_alpha") ) );
}

TEST_F( ImagePlayerFit, hidden_equal_size_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("hidden", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, hidden_smaller_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,20) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "smaller_alpha") ) );
}

TEST_F( ImagePlayerFit, hidden_bigger_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("hidden", "bigger_alpha") ) );
}

TEST_F( ImagePlayerFit, fill_equal_size ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size") ) );
}

TEST_F( ImagePlayerFit, fill_default ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, fill_smaller ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, fill_bigger ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, fill_equal_size_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, fill_smaller_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, fill_bigger_dynamic) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, fill_equal_size_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_default_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_smaller_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_bigger_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_equal_size_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("fill", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_smaller_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, fill_bigger_alpha_dynamic) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ));
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","fill" ));
	ASSERT_TRUE ( compareImages(canvas(), getFitExpectedPath("fill", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_equal_size ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, meet_smaller ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,300) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, meet_bigger ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, meet_equal_size_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, meet_smaller_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, meet_bigger_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, meet_equal_size_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_smaller_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_bigger_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_equal_size_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_smaller_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_bigger_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meet" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_equal_size ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_smaller ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_bigger ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_equal_size_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_smaller_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_bigger_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_equal_size_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_smaller_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_bigger_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_equal_size_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_smaller_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, meet_best_bigger_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","meetBest" ) );
	ASSERT_TRUE ( compareImages( canvas(), getFitExpectedPath("meet_best", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_equal_size ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "equal_size") ) );
}

TEST_F( ImagePlayerFit, slice_smaller ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "smaller") ) );
}

TEST_F( ImagePlayerFit, slice_bigger ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, slice_equal_size_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 500,375) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "equal_size" ) ) );
}

TEST_F( ImagePlayerFit, slice_smaller_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 150,150) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "smaller" ) ) );
}

TEST_F( ImagePlayerFit, slice_bigger_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "bigger" ) ) );
}

TEST_F( ImagePlayerFit, slice_equal_size_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_smaller_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_bigger_alpha ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green" ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "bigger_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_equal_size_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 171,44) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "equal_size_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_smaller_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 100,20) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "smaller_alpha" ) ) );
}

TEST_F( ImagePlayerFit, slice_bigger_alpha_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("lifia.png") ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0, 720,576) ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "fit","slice" ) );
	ASSERT_TRUE( compareImages( canvas(), getFitExpectedPath("slice", "bigger_alpha" ) ) );
}
