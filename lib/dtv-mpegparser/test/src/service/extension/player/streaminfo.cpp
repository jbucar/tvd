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

#include "../../../util.h"
#include "../../../../../src/service/extension/player/streaminfo.h"

TEST( StreamInfo, constructor ) {
	tuner::player::StreamInfo info( "h264", tuner::pes::type::video, 0x1b );
}

TEST( StreamInfo, basic_video ) {
	tuner::player::StreamInfo info( "h264", tuner::pes::type::video, 0x1b );
	ASSERT_TRUE( info.esType() == tuner::pes::type::video );
	ASSERT_TRUE( info.type() == 0x1b );
	ASSERT_TRUE( info.name() == "h264" );
}

TEST( StreamInfo, basic_audio ) {
	tuner::player::StreamInfo info( "aac", tuner::pes::type::audio, 0x11 );
	ASSERT_TRUE( info.esType() == tuner::pes::type::audio );
	ASSERT_TRUE( info.type() == 0x11 );
	ASSERT_EQ( info.name(), "aac" );
}

struct AlwaysFalse {
	bool operator()( tuner::player::StreamInfo * /*s*/, const tuner::ElementaryInfo & /*info*/ ) const {
		return false;
	}
};

TEST( StreamInfo, check_es_type ) {
	tuner::player::StreamInfo info( "h264", tuner::pes::type::video, 0x1b );

	tuner::ElementaryInfo eInfo;
	eInfo.streamType = 0x10;
	eInfo.pid = 1000;
	ASSERT_FALSE( info.check( eInfo, 0xFF ) );

	eInfo.streamType = 0x1b;
	ASSERT_TRUE( info.check( eInfo, 0xFF ) );
}

TEST( StreamInfo, check_tag ) {
	tuner::player::StreamInfo info( "h264", tuner::pes::type::video, 0x1b );
	info.addTag( 0x00, 0x10 );

	tuner::ElementaryInfo eInfo;
	eInfo.streamType = 0x1b;
	eInfo.pid = 1000;

	ASSERT_FALSE( info.check( eInfo, 0xFF ) );
	ASSERT_FALSE( info.check( eInfo, 0x11 ) );
	ASSERT_TRUE( info.check( eInfo, 0x00 ) );
	ASSERT_TRUE( info.check( eInfo, 0x10 ) );
}

TEST( StreamInfo, check_aux ) {
	tuner::player::StreamInfoImpl<AlwaysFalse> info( "h264", tuner::pes::type::video, 0x1b );
	ASSERT_EQ( info.esType(), tuner::pes::type::video );
	ASSERT_EQ( info.type(), 0x1b );
	ASSERT_EQ( info.name(), "h264" );

	tuner::ElementaryInfo eInfo;
	eInfo.streamType = 0x1b;
	eInfo.pid = 1000;

	ASSERT_FALSE( info.check( eInfo, 0xFF ) );
}

TEST( StreamInfo_ARIB_CC, basic ) {
	tuner::player::StreamInfo *info = tuner::player::aribClosedCaption();
	ASSERT_TRUE( info != NULL );

	delete info;
}

