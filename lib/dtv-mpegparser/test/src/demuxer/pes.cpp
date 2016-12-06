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
#include "../../../src/types.h"
#include "../../../src/demuxer/pes/pesdemuxer.h"
#include "../../../src/demuxer/pes/caption/captiondemuxer.h"
#include "../util.h"
#include <util/buffer.h>
#include <util/mcr.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class PESTest : public testing::Test {
public:
	PESTest( void ) {}
	virtual ~PESTest( void ) {}

	virtual void SetUp() { _demux = NULL; }
	virtual void TearDown() {
		CLEAN_ALL( tuner::arib::Management *, _management );
		CLEAN_ALL( tuner::arib::Statement *, _stats );
		delete _demux;
	}

	void onManagement( tuner::arib::Management *m, tuner::arib::CaptionDemuxer *demux ) {
		m->show();
		_management.push_back( m );
		util::BYTE group= (m->group() == 0) ? 1 : 0x21;
		demux->filterGroup( group );
	}

	void onStatement( tuner::arib::Statement *s ) {
		s->show();
		_stats.push_back( s );
	}

protected:
	bool probe( const char *file ) {
		fs::path rootPath(test::getTestRoot());
		rootPath /= "ts";
		rootPath /= file;

		FILE *f = fopen( rootPath.string().c_str(),"r+b");
		if (f) {
			util::Buffer tmp( 1024 );
			int bytes = ::fread( tmp.data(), 1, tmp.capacity(), f );
			while (bytes) {
				tmp.resize( bytes );
				// printf( "[test] Parse: bytes=%d, data=%s\n",
				// 	bytes, tmp.asHexa().c_str() );
				_demux->parse( tmp.bytes(), bytes );
				bytes = ::fread( tmp.data(), 1, tmp.capacity(), f );
			}
			fclose( f );
			return true;
		}
		return false;
	}

	tuner::PESDemuxer *_demux;
	std::vector<tuner::arib::Management *> _management;
	std::vector<tuner::arib::Statement *> _stats;
};

TEST_F( PESTest, caption_pes ) {
	tuner::arib::CaptionDemuxer *caption = new tuner::arib::CaptionDemuxer(true);
	caption->onManagement( boost::bind(&PESTest::onManagement,this,_1,caption) );
	caption->onStatement( boost::bind(&PESTest::onStatement,this,_1) );
	_demux = caption;
	ASSERT_TRUE( probe( "caption.pes" ) );
	ASSERT_EQ( _management.size(), 2 );
	ASSERT_EQ( _management[0]->group(), 0 );
	ASSERT_EQ( _management[0]->pts(), 0x47063c6e );
	ASSERT_EQ( _management[1]->group(), 0 );
	ASSERT_EQ( _management[1]->pts(), 0x4706f23f );

	ASSERT_EQ( _stats.size(), 0 );
}


TEST_F( PESTest, si_pes ) {
	tuner::arib::CaptionDemuxer *caption = new tuner::arib::CaptionDemuxer(false);
	caption->filterGroup( 0x1 );
	caption->onManagement( boost::bind(&PESTest::onManagement,this,_1,caption) );
	caption->onStatement( boost::bind(&PESTest::onStatement,this,_1) );
	_demux = caption;
	ASSERT_TRUE( probe( "si.pes" ) );
	ASSERT_EQ( _management.size(), 0 );
	ASSERT_EQ( _stats.size(), 1 );
	ASSERT_EQ( _stats[0]->pts(), 0 );
	ASSERT_TRUE( _stats[0]->dataUnit().length() > 0 );
}

TEST_F( PESTest, video_pes ) {
	_demux = new tuner::PESDemuxer( tuner::pes::type::video );
	ASSERT_TRUE( probe( "video.pes" ) );
}

TEST_F( PESTest, audio_pes ) {
	_demux = new tuner::PESDemuxer( tuner::pes::type::audio );
	ASSERT_TRUE( probe( "audio.pes" ) );
}
