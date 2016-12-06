/*******************************************************************************

  Copyright (C) 2012, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#include "mediaplayer.h"
#include <boost/math/special_functions/round.hpp>

TEST_F( MediaPlayerTest, mute ) {
	ASSERT_FALSE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 0 );
	_mp->play("test.avi");
	_mp->mute( true );
	ASSERT_TRUE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 1 );
}

TEST_F( MediaPlayerTest, mute_before_play ) {
	ASSERT_FALSE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 0 );
	_mp->mute( true );
	_mp->play("test.avi");
	ASSERT_TRUE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 1 );
}

TEST_F( MediaPlayerTest, mute_double ) {
	ASSERT_FALSE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 0 );
	_mp->mute( true );
	ASSERT_TRUE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 1 );
	_mp->mute( false );
	ASSERT_FALSE( _mp->mute() );
	ASSERT_EQ( _testData.nMuted, 0 );
}

TEST_F( MediaPlayerTest, basic_volume ) {
	ASSERT_EQ( boost::math::iround(float(MAX_VOLUME+MIN_VOLUME)/2.0f), _mp->volume() );
}

TEST_F( MediaPlayerTest, volume ) {
	ASSERT_EQ( _testData.nVol, 0 );
	_mp->volume( 12 );
	ASSERT_EQ( _mp->volume(), 12 );
	ASSERT_EQ( _testData.nVol, 1 );
}

TEST_F( MediaPlayerTest, volume2 ) {
	_mp->volume( 12 );
	ASSERT_EQ( _mp->volume(), 12 );
	_mp->volume( 15 );
	ASSERT_EQ( _mp->volume(), 15 );
	ASSERT_EQ( _testData.nVol, 2 );
}

TEST_F( MediaPlayerTest, volume_and_mute ) {
	_mp->volume( 12 );
	_mp->mute( true );
	ASSERT_EQ( _mp->volume(), 12 );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nMuted, 1 );
}

TEST_F( MediaPlayerTest, volume_and_mute2 ) {
	_mp->mute( true );
	ASSERT_EQ( _testData.nMuted, 1 );
	_mp->volume( 12 );
	ASSERT_EQ( _testData.nMuted, 0 );
	ASSERT_EQ( _mp->volume(), 12 );
	ASSERT_EQ( _testData.nVol, 1 );
}

TEST_F( MediaPlayerTest, volume_max ) {
	_mp->volume( MAX_VOLUME );
	ASSERT_EQ( _mp->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
}

TEST_F( MediaPlayerTest, volume_min ) {
	_mp->volume( MIN_VOLUME );
	ASSERT_EQ( _mp->volume(), MIN_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
}

TEST_F( MediaPlayerTest, volume_out_of_bounds ) {
	_mp->volume( MAX_VOLUME );
	ASSERT_EQ( _mp->volume(), MAX_VOLUME );
	_mp->volume( MAX_VOLUME * 2 );
	ASSERT_EQ( _mp->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
}

TEST_F( MediaPlayerTest, volume_out_of_bounds2 ) {
	_mp->volume( MAX_VOLUME );
	ASSERT_EQ( _mp->volume(), MAX_VOLUME );
	_mp->volume( -20 );
	ASSERT_EQ( _mp->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
}