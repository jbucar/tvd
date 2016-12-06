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

#include "streamevent.h"
#include "../../../../src/demuxer/psi/psi.h"
#include "../../../../src/demuxer/psi/dsmcc/biop/streamevent.h"
#include "../../../../src/demuxer/psi/dsmcc/streameventdemuxer.h"
#include "../../../../src/demuxer/psi/dsmcc/biop/objectlocation.h"
#include "util/buffer.h"
#include <boost/bind.hpp>

StreamEventTest::StreamEventTest( void ) {
	_seDemux = NULL;
}

StreamEventTest::~StreamEventTest( void ) {
	
}

void StreamEventTest::onEvent( util::WORD /*eventID*/, util::Buffer *streamEvent ) {
	_buffer = *streamEvent;
	_count++;
}

tuner::PSIDemuxer *StreamEventTest::createDemux() {
	_seDemux = new tuner::dsmcc::StreamEventDemuxer( pid() );
	return _seDemux;
}

void StreamEventTest::registerEvent( util::DWORD eventID ) {
	tuner::dsmcc::StreamEventDemuxer::Callback onEvent = boost::bind( &StreamEventTest::onEvent, this, (util::WORD) eventID, _1 );
	_seDemux->registerStreamEvent( (util::WORD) eventID, onEvent );
}

void StreamEventTest::cleanProbe( void ) {
	SectionDemuxerTest::cleanProbe( );
	_count=0;
	_buffer.resize(0);
}

tuner::ID StreamEventTest::pid( void ) const {
	return 0x100;
}

TEST_F( StreamEventTest, create_demux ) {
}

TEST_F( StreamEventTest, basic_register ) {
	registerEvent( 1 );
	probeSection( "ste01_ver1.sec" );
	ASSERT_TRUE( _count == 1 );
	ASSERT_FALSE( _buffer.length() == 0 );
	
	std::string dataPayload = "1A:36:00:01:00:00:00:00:"
		"00:00:00:00:2B:05:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:78:2D:73:62:74:76:64:3A:2F:2F:2F:74:"
		"6D:70:2F:70:72:75:65:62:61:2C:32:2E:32:2E:33:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload );

}

TEST_F( StreamEventTest, basic_register_2 ) {
	registerEvent( 1 );
	probeSection( "ste02_ver1.sec" );
	ASSERT_TRUE( _count == 1 );
	ASSERT_FALSE( _buffer.length() == 0 );
	
	std::string dataPayload = "1A:22:00:01:00:00:00:00:"
		"00:00:00:00:17:07:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:4C:75:61:54:65:73:74:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload );
}

TEST_F( StreamEventTest, event_id ) {
	registerEvent( 2 );
	probeSection( "ste_eid02_ver2.sec" );
	ASSERT_TRUE( _count == 1 );
	std::string dataPayload = "1A:24:00:02:00:00:00:00:"
		"00:00:00:00:19:07:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:45:76:65:6E:74:54:65:73:74:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload );
}

TEST_F( StreamEventTest, event_empty_descriptor ) {
	registerEvent( 0 ); //	without event_id
	probeSection( "ste_empty.sec" );
	ASSERT_TRUE( _buffer.length() == 0 );
	ASSERT_TRUE( _count == 0 );
}

TEST_F( StreamEventTest, version_change ) {
	registerEvent( 1 );
	probeSection( "ste_eid01_ver1.sec");
	ASSERT_TRUE( _count == 1 );
	ASSERT_FALSE( _buffer.length() == 0 );
	std::string dataPayload_ver1 = "1A:24:00:01:00:00:00:00:"
		"00:00:00:00:19:07:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:45:76:65:6E:74:54:65:73:74:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload_ver1 );

	probeSection( "ste_eid01_ver2.sec");
	ASSERT_TRUE( _count == 1 );
	ASSERT_FALSE( _buffer.length() == 0 );
	std::string dataPayload_ver2 = "1A:24:00:01:00:00:00:00:"
		"00:00:00:00:19:07:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:45:76:65:6E:74:54:65:73:74:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload_ver2 );
}

TEST_F( StreamEventTest, version_change2 ) {
	registerEvent( 1 );
	probeSection( "ste_eid01_ver1.sec");
	ASSERT_TRUE( _count == 1 );
	ASSERT_FALSE( _buffer.length() == 0 );
	std::string dataPayload_ver1 = "1A:24:00:01:00:00:00:00:"
		"00:00:00:00:19:07:01:4C:49:46:49:41:5F:42:41:53:"
		"45:49:44:2C:45:76:65:6E:74:54:65:73:74:00";
	ASSERT_TRUE( _buffer.asHexa() == dataPayload_ver1 );
	
	probeSection( "ste_eid01_ver1.sec");
	ASSERT_TRUE( _count == 0 );
	ASSERT_TRUE( _buffer.length() == 0 );
}

TEST_F( StreamEventTest, bad_crc ){
	registerEvent( 1 );
	probeSection( "ste_bad_crc.sec" );
	ASSERT_TRUE( _count == 0 );
	ASSERT_TRUE( _buffer.length() == 0 );
}

TEST_F( StreamEventTest, invalid_table_id ) {
	registerEvent( 1 );
	probeSection( "ste_invalid_tableid.sec" );
	ASSERT_TRUE( _count == 0 );
	ASSERT_TRUE( _buffer.length() == 0 );
}
