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

#include "mediaplayer.h"

TEST_F( MediaPlayerTest, move ) {
	canvas::Point p(10, 10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 10 );
	ASSERT_EQ( _testData.bounds.y, 10 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move2 ) {
	canvas::Point p(200, 10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 200 );
	ASSERT_EQ( _testData.bounds.y, 10 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move_invalid ) {
	canvas::Point p(-10, 10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move_invalid2 ) {
	canvas::Point p(10, -10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move_invalid3 ) {
	canvas::Point p(-10, -10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move_invalid4 ) {
	canvas::Point p(10, 10);
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, move_twice ) {
	canvas::Point p(10, 10);
	_mp->play("test.avi");
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 10 );
	ASSERT_EQ( _testData.bounds.y, 10 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
	
	p.x = 0;
	p.y = 0;
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, changeMode ) {
	_mp->play("test.avi");
	ASSERT_TRUE( _sys->screen()->mode( _sys->screen()->connector(), canvas::mode::m1080i_60 ) );
	ASSERT_EQ( _testData.nMoveResize, 1 );
	_mp->stop();
}

TEST_F( MediaPlayerTest, changeAspect ) {
	_mp->play("test.avi");
	ASSERT_TRUE( _sys->screen()->aspect( canvas::aspect::a4_3 ) );
	ASSERT_EQ( _testData.nMoveResize, 1 );
	_mp->stop();
}

TEST_F( MediaPlayerTest, changeMode_after_stop ) {
	_mp->play("test.avi");
	_mp->stop();
	ASSERT_TRUE( _sys->screen()->mode( _sys->screen()->connector(), canvas::mode::m1080i_60 ) );
}

TEST_F( MediaPlayerNoDestroyTest, changeMode_after_deleted ) {
	_mp->play("test.avi");
	_mp->stop();
	DEL( _mp );
	ASSERT_TRUE( _sys->screen()->mode( _sys->screen()->connector(), canvas::mode::m1080i_60 ) );
}

TEST_F( MediaPlayerTest, resize ) {
	canvas::Size r(300, 400);
	_mp->play("test.avi");
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 300 );
	ASSERT_EQ( _testData.bounds.h, 400 );
}

TEST_F( MediaPlayerTest, resize2 ) {
	canvas::Size r(300, 10);
	_mp->play("test.avi");
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 300 );
	ASSERT_EQ( _testData.bounds.h, 10 );
}

TEST_F( MediaPlayerTest, resize_invalid ) {
	canvas::Size r(300, 0);
	_mp->play("test.avi");
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, resize_invalid2 ) {
	canvas::Size r(0, 200);
	_mp->play("test.avi");
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, resize_invalid3 ) {
	canvas::Size r(0, -5);
	_mp->play("test.avi");
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, resize_invalid4 ) {
	canvas::Size r(50, 50);
	_mp->resize( r );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, resize_and_move ) {
	canvas::Size r(10, 200);
	canvas::Point p(10, 10);
	_mp->play("test.avi");
	_mp->resize( r );
	_mp->move( p );
	ASSERT_EQ( _testData.bounds.x, 10 );
	ASSERT_EQ( _testData.bounds.y, 10 );
	ASSERT_EQ( _testData.bounds.w, 10 );
	ASSERT_EQ( _testData.bounds.h, 200 );
}

TEST_F( MediaPlayerTest, moveResize ) {
	canvas::Rect rect(10, 10, 500, 500);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 10 );
	ASSERT_EQ( _testData.bounds.y, 10 );
	ASSERT_EQ( _testData.bounds.w, 500 );
	ASSERT_EQ( _testData.bounds.h, 500 );
}

TEST_F( MediaPlayerTest, moveResize2 ) {
	canvas::Rect rect(0, 0, 500, 500);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 500 );
	ASSERT_EQ( _testData.bounds.h, 500 );
}

TEST_F( MediaPlayerTest, moveResize_invalid ) {
	canvas::Rect rect(10, 10, 500, 0);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid2 ) {
	canvas::Rect rect(10, -5, 500, 500);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid3 ) {
	canvas::Rect rect(-5, 10, 500, 500);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid4 ) {
	canvas::Rect rect(5, 10, 0, 500);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid5 ) {
	canvas::Rect rect(5, 10, 10, 500);
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid6 ) {
	canvas::Rect rect(5, 10, 10, 800);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid7 ) {
	canvas::Rect rect(5, 10, 721, 576);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, moveResize_invalid8 ) {
	canvas::Rect rect(5, 10, 720, 577);
	_mp->play("test.avi");
	_mp->moveResize( rect );
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, fullscreen ) {
	_mp->play("test.avi");
	_mp->setFullScreen();
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}

TEST_F( MediaPlayerTest, fullscreen2 ) {
	_mp->play("test.avi");
	canvas::Rect rect(5, 10, 50, 50);
	_mp->moveResize( rect );
	_mp->setFullScreen();
	ASSERT_EQ( _testData.bounds.x, 0 );
	ASSERT_EQ( _testData.bounds.y, 0 );
	ASSERT_EQ( _testData.bounds.w, 720 );
	ASSERT_EQ( _testData.bounds.h, 576 );
}