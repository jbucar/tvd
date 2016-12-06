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

class SurfaceCompositionMode : public Surface {
protected:
    std::string getSubDirectory() const { return "CompositionMode"; }
};

TEST_F( SurfaceCompositionMode, source_over_basic ) {
	canvas::Rect rect(0,0,300,300);
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );
	canvas::Surface* s2 = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );
	s1->autoFlush(true);

	s2->setColor(canvas::Color(255,0,0,128));
	ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,300,300)) );

	ASSERT_TRUE( s1->blit( canvas::Point(0,0), s2 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "source_over_basic" )) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

TEST_F( SurfaceCompositionMode, source_basic ) {
	canvas::Rect rect(0,0,300,300);
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );
	canvas::Surface* s2 = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );
	s1->autoFlush(true);

	s2->setColor(canvas::Color(255,0,0,128));
	ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,300,300)) );

	s1->setCompositionMode(canvas::composition::source);
	ASSERT_TRUE( s1->blit( canvas::Point(0,0), s2 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "source_basic" )) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

TEST_F( SurfaceCompositionMode, source_full_transparent ) {
	canvas::Rect rect(0,0,300,300);
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );
	canvas::Surface* s2 = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );
	s1->autoFlush(true);

	s2->setColor(canvas::Color(0,0,0,0));
	ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,300,300)) );

	s1->setCompositionMode(canvas::composition::source);
	ASSERT_TRUE( s1->blit( canvas::Point(0,0), s2 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "source_300x300_fullTransparent" )) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

TEST_F( SurfaceCompositionMode, mixed ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName("image.png") );
	canvas::Surface* s2 = getCanvas()->createSurface( canvas::Rect(0,0,200,200) );
	canvas::Surface* s3 = getCanvas()->createSurface( canvas::Rect(0,0,100,100) );
	
	ASSERT_TRUE( s1 != NULL );
	ASSERT_TRUE( s2 != NULL );
	ASSERT_TRUE( s3 != NULL );
	s1->autoFlush(true);

	s2->setColor(canvas::Color(255,0,0,128));
	ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,200,200)) );

	s3->setColor(canvas::Color(0,255,0,128));
	ASSERT_TRUE( s3->fillRect(canvas::Rect(0,0,100,100)) );

	s1->setCompositionMode(canvas::composition::source_over);
	ASSERT_TRUE( s1->blit( canvas::Point(0,0), s2 ) );

	s1->setCompositionMode(canvas::composition::source);
	ASSERT_TRUE( s1->blit( canvas::Point(0,0), s3 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "mixed" )) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

