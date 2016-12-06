/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "cat.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/cat.h"
#include "../../../src/demuxer/psi/catdemuxer.h"
#include <boost/bind.hpp>

CatTest::CatTest()
{
}

CatTest::~CatTest()
{
}

tuner::PSIDemuxer *CatTest::createDemux() {
	return tuner::createDemuxer<tuner::CATDemuxer,tuner::Cat>(
		TS_PID_CAT,
		boost::bind( &CatTest::onCat, this, _1 ),
		boost::bind( &SectionDemuxerTest::onExpired, this, _1 ) );
}

void CatTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_cat.reset();
}

void CatTest::onCat( const boost::shared_ptr<tuner::Cat> &cat ) {
	ASSERT_FALSE( _cat );
	_cat = cat;
}

TEST_F( CatTest, create_demux ) {
}

TEST( Cat, getSystemName ) {
	ASSERT_STREQ( tuner::desc::ca::getSystemName( 0x1860 ), "Kudelski SA" );
	ASSERT_STREQ( tuner::desc::ca::getSystemName( 0x0 ), "Reserved" );
	ASSERT_STREQ( tuner::desc::ca::getSystemName( 0xAA02 ), "Unknown" );
}

TEST_F( CatTest, cat_basic ) {
	//	Probe basic cat
	probeSection( "cat.sec" );
	_cat->show();
	ASSERT_TRUE( _cat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _cat->version() == 1 );

	const tuner::desc::ca::Systems & sys = _cat->systems();
	ASSERT_EQ( sys.size(), 8 );
	ASSERT_EQ( sys[0].systemID, 0x1860 );
	ASSERT_EQ( sys[0].pid, 0x124 );
	ASSERT_EQ( sys[0].data.size(), 0 );
	ASSERT_STREQ( tuner::desc::ca::getSystemName( sys[0].systemID ), "Kudelski SA" );

	ASSERT_EQ( sys[1].systemID, 0x1802 );
	ASSERT_EQ( sys[1].pid, 0x11f );
	ASSERT_EQ( sys[1].data.size(), 0 );
	ASSERT_STREQ( tuner::desc::ca::getSystemName( sys[1].systemID ), "Kudelski SA" );
}

