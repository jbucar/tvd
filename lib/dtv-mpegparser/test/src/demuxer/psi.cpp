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
#include "psi.h"
#include "../util.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/pat.h"
#include "../../../src/demuxer/psi/patdemuxer.h"
#include <boost/bind.hpp>
#include <util/buffer.h>

SectionDemuxerTest::SectionDemuxerTest( void )
{
}

SectionDemuxerTest::~SectionDemuxerTest( void )
{
}

void SectionDemuxerTest::SetUp() {
	_demux = createDemux();
	ASSERT_TRUE(_demux != NULL);
}

void SectionDemuxerTest::TearDown() {
	delete _demux;
}

void SectionDemuxerTest::onExpired( tuner::ID pid ) {
	_expired[pid]++;
}

int SectionDemuxerTest::wasExpired( void ) const {
	std::map<tuner::ID,int>::const_iterator it=_expired.find( pid() );
	return it != _expired.end() ? (*it).second : 0;
}

void SectionDemuxerTest::probeSection( const char *file, bool clean/*=true*/ ) {
	if (clean) {
		cleanProbe();
	}
	util::Buffer *sec = test::getSection( file );
	ASSERT_TRUE(sec!=NULL);
	_demux->process( sec );
	delete sec;
}

void SectionDemuxerTest::cleanProbe() {
	_expired.clear();
	_expired[pid()] = 0;
}
