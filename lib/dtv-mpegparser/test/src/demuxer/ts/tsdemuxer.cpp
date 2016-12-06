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

#include "tsdemuxer.h"
#include "../../util.h"
#include "../../../../src/demuxer/ts.h"
#include "../../../../src/demuxer/psi/psi.h"
#include <util/buffer.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

TSDemuxerTest::~TSDemuxerTest( void ) {
}

TSDemuxerTest::TSDemuxerTest( void ) {
}

void TSDemuxerTest::SetUp() {
	_tsDemuxer = new tsdemux::DemuxerImpl();
	_tsDemuxer->start();
}

void TSDemuxerTest::TearDown() {
	_tsDemuxer->stop();
	delete _tsDemuxer;
	_tsDemuxer = NULL;
}

bool TSDemuxerTest::filterSection( tuner::ID pid, const tsdemux::SectionFilterCallback &callback ) {
	return _tsDemuxer->startFilter( new tsdemux::SectionFilter( pid, callback ) );
}

bool TSDemuxerTest::probeTS( const char *file ) {
	fs::path rootPath(test::getTestRoot());
	rootPath /= "ts";
	rootPath /= file;

	FILE *f = fopen( rootPath.string().c_str(),"r+b");
	if (f) {
		util::Buffer *tmp = _tsDemuxer->allocBuffer();
		int bytes = ::fread( tmp->data(), 1, tmp->capacity(), f );
		while (bytes) {
			tmp->resize( bytes );
			_tsDemuxer->pushData( tmp );

			tmp = _tsDemuxer->allocBuffer();
			bytes = ::fread( tmp->data(), 1, tmp->capacity(), f );
		}
		_tsDemuxer->freeBuffer( tmp );
		fclose( f );
		return true;
	}
	return false;
}

TEST_F( TSDemuxerTest, basic_ts_with_pat ) {
	int calls=0;

	ASSERT_TRUE( filterSection( TS_PID_PAT, boost::bind(&test::notifyCounter, &calls) ) );
	ASSERT_TRUE( probeTS( "pat.ts" ) );
	test::wait( test::flags::task_running );
	ASSERT_TRUE( calls == 1 );
}

void notifyCounter( int *counter, int count ) {
	(*counter)++;
	if (count == *counter) {
		test::notify( test::flags::task_running );
	}
}

TEST_F( TSDemuxerTest, three_eit_in_188b ) {
	int calls=0;
	ASSERT_TRUE( filterSection( TS_PID_EIT, boost::bind(&notifyCounter, &calls, 3) ) );
	ASSERT_TRUE( probeTS( "ts_eit_50_51_52.ts" ) );
	test::wait( test::flags::task_running );
	ASSERT_TRUE( calls == 3 );
}

// TEST_F( TSDemuxerTest, DISABLED_pcr_01 ) {
// 	int calls=1;
// 	// ASSERT_TRUE( addFilter( new tsdemux::BasicFilter(
// 	// 	             new tsdemux::PESWrapper(
// 	// 		             0x120,
// 	// 		             tuner::pes::pcr,
// 	// 		             0,
// 	// 		             boost::bind(&notifyCounter, &calls, 1)))));
// 	// ASSERT_TRUE( probeTS( "pcr_01.dat" ) );
// 	// test::wait( test::flags::task_running );
// }

