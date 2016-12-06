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
#include "../../util.h"
#include "../../../../src/demuxer/descriptors/generic.h"
#include "../../../../src/demuxer/psi/dsmcc/datacarouselhelper.h"
#include "../../../../src/demuxer/psi/psi.h"
#include "../../../../src/demuxer/psi/pmt.h"
#include "../../../../src/service/service.h"

	
TEST( DataCarouselHelper, constructor ) {
	tuner::dsmcc::DataCarouselHelper dc;
}

void addPmtElement( std::vector<tuner::ElementaryInfo> &elems, tuner::ID pid, tuner::ID streamType ) {
	tuner::ElementaryInfo elem;
	elem.streamType = (util::BYTE) streamType;
	elem.pid = pid;
	elems.push_back( elem );
}

tuner::Pmt *createPmt( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, std::vector<tuner::ElementaryInfo> elems ) {
	//	Contruct Service
	tuner::desc::Descriptors descs;
	return new tuner::Pmt( srvPID, 1, srvID, pcrPID, descs, elems );
}

bool onlyType22( util::BYTE type ) {
	return type == 0x22;
}

bool allTypes( util::BYTE /*type*/ ) {
	return true;
}

TEST( DataCarouselHelper, collect_nothing ) {
	tuner::dsmcc::DataCarouselHelper dc;
	tuner::ID bPID = 100;
	tuner::ID srvID = 10, srvPID = 101, pcrPID = 102;

	std::vector<tuner::ElementaryInfo> elems;
	addPmtElement( elems, bPID, PSI_ST_TYPE_B );
	tuner::Pmt *pmt = createPmt( srvID, srvPID, pcrPID, elems );
	tuner::Service *srv = test::createService( srvID, srvPID, pmt );

	ASSERT_FALSE( dc.collectTags( srv, &allTypes ) );

	delete srv;
}

TEST( DataCarouselHelper, collect_in_stream_identifier_desc_only ) {
	tuner::dsmcc::DataCarouselHelper dc;
	tuner::ID bPID = 100;
	tuner::ID srvID = 10, srvPID = 101, pcrPID = 102;
	util::BYTE tag = 0xa;

	std::vector<tuner::ElementaryInfo> elems;
	addPmtElement( elems, bPID, 0x22 );
	tuner::ElementaryInfo &elem = elems[0];

	{
		util::BYTE streamIdentifier[] = { 0x52, 0x1, tag };
		elem.descriptors.addDescriptor( streamIdentifier, 3 );
	}
	
	tuner::Pmt *pmt = createPmt( srvID, srvPID, pcrPID, elems );
	tuner::Service *srv = test::createService( srvID, srvPID, pmt );

	ASSERT_TRUE( dc.collectTags( srv, &onlyType22 ) );
	ASSERT_TRUE( dc.findTag( tag ) == bPID );
	ASSERT_TRUE( dc.findTag( srvID, tag ) == bPID );

	delete srv;
}

TEST( DataCarouselHelper, collect_in_association_tag_desc_only ) {
	tuner::dsmcc::DataCarouselHelper dc;
	tuner::ID bPID = 100;
	tuner::ID srvID = 10, srvPID = 101, pcrPID = 102;
	util::BYTE tag = 0xa;

	std::vector<tuner::ElementaryInfo> elems;
	addPmtElement( elems, bPID, 0x22 );
	tuner::ElementaryInfo &elem = elems[0];

	{
		util::BYTE descriptor[] = { 0x14, 0x4, 0x0, tag, 0x0, 0x0 };
		elem.descriptors.addDescriptor( descriptor, sizeof(descriptor)/sizeof(util::BYTE) );
	}

	tuner::Pmt *pmt = createPmt( srvID, srvPID, pcrPID, elems );
	tuner::Service *srv = test::createService( srvID, srvPID, pmt );

	ASSERT_TRUE( dc.collectTags( srv, &onlyType22 ) );
	ASSERT_TRUE( dc.findTag( tag ) == bPID );
	ASSERT_TRUE( dc.findTag( srvID, tag ) == bPID );

	dc.clearTags();
	ASSERT_FALSE( dc.findTag( tag ) == bPID );
	ASSERT_FALSE( dc.findTag( srvID, tag ) == bPID );

	delete srv;
}

TEST( DataCarouselHelper, collect_with_both_desc_check_overwrite ) {
	tuner::dsmcc::DataCarouselHelper dc;
	tuner::ID bPID = 100;
	tuner::ID srvID = 10, srvPID = 101, pcrPID = 102;
	util::BYTE tag = 0xa;

	std::vector<tuner::ElementaryInfo> elems;
	addPmtElement( elems, bPID, 0x22 );
	tuner::ElementaryInfo &elem = elems[0];

	{
		util::BYTE descriptor[] = { 0x14, 0x4, 0x0, tag, 0x0, 0x0 };
		elem.descriptors.addDescriptor( descriptor, sizeof(descriptor)/sizeof(util::BYTE) );
	}

	{
		util::BYTE streamIdentifier[] = { 0x52, 0x1, tag };
		elem.descriptors.addDescriptor( streamIdentifier, 3 );
	}

	tuner::Pmt *pmt = createPmt( srvID, srvPID, pcrPID, elems );
	tuner::Service *srv = test::createService( srvID, srvPID, pmt );

	ASSERT_TRUE( dc.collectTags( srv, &onlyType22 ) );
	ASSERT_TRUE( dc.findTag( tag ) == bPID );
	ASSERT_TRUE( dc.findTag( srvID, tag ) == bPID );

	dc.clearTags( srv );
	ASSERT_FALSE( dc.findTag( tag ) == bPID );
	ASSERT_FALSE( dc.findTag( srvID, tag ) == bPID );

	delete srv;
}
