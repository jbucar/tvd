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
#include "../../../src/canvas.h"
#include "../../../src/rect.h"

TEST_F( Surface, canvas_invalidateRegion_simple ) {
	canvas::Rect r1(0,0,100,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();
	
	ASSERT_TRUE( dirtyRegions.size()==1 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_not_intersected ) {
	canvas::Rect r1(0,0,100,100);
	canvas::Rect r2(200,200,100,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();
	
	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==r2 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_diagonal ) {
	canvas::Rect r1(0,0,100,100);
	canvas::Rect r2(50,50,100,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==r2 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_diagonal_invert ) {
	canvas::Rect r1(0,0,100,100);
	canvas::Rect r2(50,50,100,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r2);
	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r2 );
	ASSERT_TRUE( dirtyRegions[1]==r1 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_diagonal2 ) {
	canvas::Rect r1(0,0,100,100);
	canvas::Rect r2(20,20,100,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==1 );
	ASSERT_TRUE( dirtyRegions[0]==canvas::Rect(0,0,120,120) );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_horizontal ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,140,100,20);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==canvas::Rect(50,140,50,20) );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_horizontal_invert ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,140,100,20);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r2);
	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==canvas::Rect(50,140,50,20) );
	ASSERT_TRUE( dirtyRegions[1]==r1 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_vertical ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(140,150,20,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==canvas::Rect(140,200,20,50) );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_vertical_invert ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(140,150,20,100);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r2);
	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==canvas::Rect(140,200,20,50) );
	ASSERT_TRUE( dirtyRegions[1]==r1 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_horizontal_longer ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,140,200,20);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==r2 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_horizontal_longer_invert ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(50,140,200,20);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r2);
	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r2 );
	ASSERT_TRUE( dirtyRegions[1]==r1 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_vertical_higher ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(140,50,20,200);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==2 );
	ASSERT_TRUE( dirtyRegions[0]==r1 );
	ASSERT_TRUE( dirtyRegions[1]==r2 );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,50,60,200);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r1);
	c->invalidateRegion(r2);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==1 );
	ASSERT_TRUE( dirtyRegions[0]==canvas::Rect(100,50,100,200) );
}

TEST_F( Surface, canvas_invalidateRegion_2_rects_intersected_invert ) {
	canvas::Rect r1(100,100,100,100);
	canvas::Rect r2(120,50,60,200);
	canvas::Canvas *c = getCanvas();

	c->invalidateRegion(r2);
	c->invalidateRegion(r1);
	std::vector<canvas::Rect> dirtyRegions = c->dirtyRegions();

	ASSERT_TRUE( dirtyRegions.size()==1 );
	ASSERT_TRUE( dirtyRegions[0]==canvas::Rect(100,50,100,200) );
}