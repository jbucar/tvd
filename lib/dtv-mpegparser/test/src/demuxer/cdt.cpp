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
#include "cdt.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/cdt.h"
#include "../../../src/demuxer/psi/cdtdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include "../../../src/service/types.h"
#include <boost/bind.hpp>
#include <util/buffer.h>

CdtTest::CdtTest() {
}

CdtTest::~CdtTest() {
}

void CdtTest::onCdt( const boost::shared_ptr<tuner::Cdt> &cdt ) {
	_cdt = cdt;
}

tuner::ID CdtTest::pid() const {
	return TS_PID_CDT;
}

tuner::PSIDemuxer *CdtTest::createDemux() {
	return tuner::createDemuxer<tuner::CDTDemuxer, tuner::Cdt > (
		pid(),
		boost::bind( &CdtTest::onCdt, this, _1 ),
		boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void CdtTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_cdt.reset();
}

TEST_F( CdtTest, create_demux ) {
}

// TEST_F( CdtTest, cdt_ver1 ) {
// 	probeSection( "cdt_ver1.sec" );
// 	ASSERT_TRUE( _cdt != NULL );
// 	ASSERT_TRUE( _cdt->version( ) == 1 );
// 	ASSERT_TRUE( _cdt->downloadID( ) == 0x73a );
// 	ASSERT_TRUE( _cdt->nitID( ) == 0x073a );
// }


