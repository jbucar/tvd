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

// Tests markDirty
TEST_F( Surface, markDirty ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();
	s->markDirty();

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );
	ASSERT_TRUE( dirtyRegions.front() == rect );

	getCanvas()->destroy( s );
}

// Tests markDirty( canvas::Rect )
TEST_F( Surface, markDirty_from_rect ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r1(0,0,10,10),
				 r2(100,100,200,200),
				 r3(200,200,0,0);

	s->markDirty( r1 );
	s->markDirty( r2 );
	s->markDirty( r3 );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 2 );
	ASSERT_TRUE( dirtyRegions.at(0) == r1 );
	ASSERT_TRUE( dirtyRegions.at(1) == r2 );

	getCanvas()->destroy( s );
}

// Tests markDirty( canvas::Rect ). Se marca una region y luego se marca otra
// que la incluya. (la ultima debe reemplazar a la primera)
TEST_F( Surface, markDirty_new_region_includes_old_region ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r1(0,0,100,100),
	r2(0,0,200,200);

	s->markDirty( r1 );
	s->markDirty( r2 );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );
	ASSERT_TRUE( dirtyRegions.at(0) == r2 );

	getCanvas()->destroy( s );
}

// Tests markDirty( canvas::Rect ). Se marca una region y luego se marca otra
// que este incluida en la primera. (la ultima region no debe aparecer)
TEST_F( Surface, markDirty_old_region_includes_new_region ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r1(0,0,100,100),
				 r2(0,0,200,200);

	s->markDirty( r2 );
	s->markDirty( r1 );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );
	ASSERT_TRUE( dirtyRegions.at(0) == r2 );

	getCanvas()->destroy( s );
}

// Tests markDirty( canvas::Rect ) part outside the canvas
TEST_F( Surface, markDirty_part_outside ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r1(-1,-1,10,10),   //top-right
				 r2(711,-1,10,10),  //top-left
				 r3(711,567,10,10), //bottom-left
				 r4(-1,567,10,10);  //bottom-right

	s->markDirty( r1 );
	s->markDirty( r2 );
	s->markDirty( r3 );
	s->markDirty( r4 );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 4 );
	ASSERT_TRUE( dirtyRegions.at(0) == canvas::Rect(r1) );
	ASSERT_TRUE( dirtyRegions.at(1) == canvas::Rect(r2) );
	ASSERT_TRUE( dirtyRegions.at(2) == canvas::Rect(r3) );
	ASSERT_TRUE( dirtyRegions.at(3) == canvas::Rect(r4) );

	getCanvas()->destroy( s );
}

// Tests markDirty( canvas::Rect ) outside the canvas
TEST_F( Surface, markDirty_outside ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	canvas::Rect r1(-10,-10,10,10), //top-left
				 r2(720,-10,10,10), //top-right
				 r3(720,576,10,10), //bottom-right
				 r4(-10,576,10,10); //bottom-left

	s->markDirty( r1 );
	s->markDirty( r2 );
	s->markDirty( r3 );
	s->markDirty( r4 );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 4 );
	ASSERT_TRUE( dirtyRegions.at(0) == canvas::Rect(r1) );
	ASSERT_TRUE( dirtyRegions.at(1) == canvas::Rect(r2) );
	ASSERT_TRUE( dirtyRegions.at(2) == canvas::Rect(r3) );
	ASSERT_TRUE( dirtyRegions.at(3) == canvas::Rect(r4) );

	getCanvas()->destroy( s );
}

