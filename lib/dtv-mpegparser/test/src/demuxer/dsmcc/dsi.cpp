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
#include "dsi.h"
#include "../../../../src/demuxer/psi/psi.h"
#include "../../../../src/demuxer/psi/dsmcc/dsi.h"
#include "../../../../src/demuxer/psi/dsmcc/dii.h"
#include "../../../../src/demuxer/psi/dsmcc/dsmcc.h"
#include "../../../../src/demuxer/psi/dsmcc/module.h"
#include "../../../../src/demuxer/psi/dsmcc/dsmccdemuxer.h"
#include "../../../../src/demuxer/psi/dsmcc/module/memorydata.h"
#include "../../../../src/resourcemanager.h"
#include <boost/bind.hpp>
#include <util/buffer.h>
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>

DsiTest::DsiTest() {
	tuner::dsmcc::MemoryData::initialize( 100, 4096 );
}

DsiTest::~DsiTest() {
	cleanProbe();
	tuner::dsmcc::MemoryData::cleanup();
}

void DsiTest::onDii( tuner::dsmcc::DSMCCDemuxer *demux, const boost::shared_ptr<tuner::dsmcc::DII> &dii ) {
	_dii = dii;
	demux->filterModules( dii->downloadID(), dii->modules(), dii->blockSize() );
}

void DsiTest::onModule( tuner::dsmcc::DSMCCDemuxer * /*demux*/, const boost::shared_ptr<tuner::dsmcc::Module> &module ) {
	_module = module;
}

void DsiTest::onDsi( tuner::dsmcc::DSMCCDemuxer * /*demux*/, const boost::shared_ptr<tuner::dsmcc::DSI> &dsi ) {
	_dsi = dsi;
}

tuner::PSIDemuxer *DsiTest::createDemux( ) {
	tuner::ResourceManager *resMgr = new tuner::ResourceManager( "/tmp/test", 100, 1000, 400*1024*1024 );
	tuner::dsmcc::DSMCCDemuxer *demux = new tuner::dsmcc::DSMCCDemuxer( pid(), resMgr );

	tuner::dsmcc::DSMCCDemuxer::DSICallback onDSI = boost::bind( &DsiTest::onDsi, this, demux, _1 );
	demux->onDSI( onDSI );

	tuner::dsmcc::DSMCCDemuxer::DIICallback onDII = boost::bind( &DsiTest::onDii, this, demux, _1 );
	demux->onDII( onDII );

	tuner::dsmcc::DSMCCDemuxer::ModuleCallback onModule = boost::bind( &DsiTest::onModule, this, demux, _1 );
	demux->onModule( onModule );

	return demux;
}

void DsiTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_dsi.reset();
	_dii.reset();
	_module.reset();
}

tuner::ID DsiTest::pid() const {
	return 0x099; // pid del dsmcc???
}

TEST_F( DsiTest, create_demux ) {
}

void testFirstCompatibilityDescriptor(
	const tuner::dsmcc::compatibility::Descriptors &desc,
	const util::BYTE  type,
	const util::DWORD specifier,
	const util::WORD  model,
	const util::WORD  version )
{
	ASSERT_TRUE( desc.size() == 1 );
	const tuner::dsmcc::compatibility::Descriptor desc0 = desc[0];

	ASSERT_TRUE( desc0.model == model );
	ASSERT_TRUE( desc0.specifier == specifier );
	ASSERT_TRUE( desc0.type == type );
	ASSERT_TRUE( desc0.version == version );
}

TEST_F( DsiTest, dsi_tc001_tag0xA ) {
	probeSection( "dsi_tc001_ver1_tag0x0A.sec");
	ASSERT_TRUE( _dsi ? true : false );
	_dsi->show();
	const tuner::dsmcc::compatibility::Descriptors &compatibility = _dsi->compatibilities();
	ASSERT_TRUE( compatibility.size() == 0 );

	const util::Buffer &data = _dsi->privateData();
	// private data tiene 67 bytes.
	ASSERT_TRUE( data.length() > 0 );
}

TEST_F( DsiTest, dii_sample1 ) {
	probeSection( "dii_sample1.sec");
	ASSERT_TRUE( _dii ? true : false );

	const tuner::dsmcc::compatibility::Descriptors &compatibility = _dii->compatibilities();
	ASSERT_TRUE( compatibility.size() == 0 );

	const util::Buffer &data = _dii->privateData();
	ASSERT_TRUE( data.length() > 0 );

	{	//	Parse group info indication
		tuner::dsmcc::group::GroupInfoIndication gii;
		tuner::dsmcc::group::parse( data.bytes(), data.length(), gii );
		tuner::dsmcc::group::show( gii );
	}
	printf( "rako: dump=%s\n", data.asHexa().c_str() );

	_dii->show();
}

TEST_F( DsiTest, mped_sample1 ) {
	probeSection( "mped_sample1.sec");
}

void testModuleData( const tuner::dsmcc::module::Type &module, const tuner::ID id, const util::DWORD  size,
		const util::BYTE  version, size_t bufflen ) {
	ASSERT_TRUE( module.id == id );
	ASSERT_TRUE( module.size == size );
	ASSERT_TRUE( module.version == version );
	ASSERT_TRUE( module.info.length() == bufflen );
}

TEST_F( DsiTest, dii_v1_sample_tc001 ) {
	probeSection( "dii_v1_sample_tc001.sec" );
	ASSERT_TRUE( _dii ? true : false );
	_dii->show();

	const tuner::dsmcc::module::Modules &mods = _dii->modules();
	ASSERT_TRUE(  mods.size() == 2 );
	testModuleData( mods[0], 0x0001, 0x000004cf, 0x01, 0x15 );
	ASSERT_FALSE( HasFatalFailure() );
	testModuleData( mods[1], 0x0002, 0x000000d6, 0x01, 0x15 );
	ASSERT_FALSE( HasFatalFailure() );
}

void testModuleDataHeader( const tuner::dsmcc::Module &module,  const util::DWORD downloadID,
		const tuner::ID id, const util::BYTE version ) {
	ASSERT_TRUE( module.downloadID() == downloadID );
	ASSERT_TRUE( module.id() == id );
	ASSERT_TRUE( module.version() == version );
}

TEST_F( DsiTest, basic_module ) {
	probeSection( "dii_v1_sample_tc001.sec" );

	const tuner::dsmcc::module::Modules &modules = _dii->modules();
	ASSERT_TRUE(  modules.size() == 2 );

	const tuner::dsmcc::module::Type &diiModule0 = modules[0];
	const tuner::ID id0 = diiModule0.id;
	ASSERT_TRUE( id0 == 1 );

	const tuner::dsmcc::module::Type &diiModule1 = modules[1];
	const tuner::ID id1 = diiModule1.id;
	ASSERT_TRUE( id1 == 2 );

	probeSection( "ddb01_v1_sample_tc001.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0001, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( _module->id() == id0 );

	probeSection( "ddb02_v1_sample_tc001.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0002, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( _module->id() == id1 );
 }

TEST_F( DsiTest, ddb_module_1024 ) {
	probeSection( "dii04_small_payload.sec" );
	ASSERT_TRUE( _dii ? true : false );

	probeSection( "ddb03_small_payload.sec");
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0001, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	probeSection( "ddb04_small_payload.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0002, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( DsiTest, ddb_compressed ) {
	probeSection( "dsi05_compress.sec" );
	ASSERT_TRUE( _dsi ? true : false );

	probeSection( "dii05_compress.sec" );
	ASSERT_TRUE( _dii ? true : false );

	probeSection( "ddb05_compress_01.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0001, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	probeSection( "ddb05_compress_02.sec");
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0002, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( DsiTest, transactionId_update_flag ) {
	probeSection( "dsi06_update.sec");
	ASSERT_TRUE( _dsi ? true : false );

	probeSection( "dii06_update.sec");
	ASSERT_TRUE( _dii ? true : false );

	probeSection( "ddb06_update_01.sec");
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0001, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	probeSection( "ddb06_update_02.sec");
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0002, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( DsiTest, compress_and_update_flag ) {
	probeSection( "dsi07_compress_update.sec" );
	ASSERT_TRUE( _dsi ? true : false );

	probeSection( "dii07_compress_update.sec" );
	ASSERT_TRUE( _dii ? true : false );

	probeSection( "ddb07_compress_update_01.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0001, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	probeSection( "ddb07_compress_update_02.sec" );
	ASSERT_TRUE( _module ? true : false );
	testModuleDataHeader( *_module, 0x0002, 0x0002, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( DsiTest, big_module ) {

// TEST COMENTADO por no subir las secciones (mas de 300MB)

//	probeSection( "dsmcc_size_4066_big/DSI.sec" );
//	ASSERT_TRUE( _dsi ? true : false );
//
//	probeSection( "dsmcc_size_4066_big/DII.sec" );
//	ASSERT_TRUE( _dii ? true : false );
//
//	//	last section number is 0001_016118.sec
//	//	last section number is 0001_080590.sec
//
//	char name[255];
//	for (int i=0; i<=80590; i++) {
//		snprintf( name, 255, "dsmcc_size_4066_big/0001_%06d.sec", i );
//		probeSection( name );
//	}
//	ASSERT_TRUE( _module ? true : false );
}

TEST_F( DsiTest, multiple_files ) {

// TEST COMENTADO por no subir las secciones (mas de 300MB)

//	printf( "\n\n--Multiple-Files-Test--\n" );
//	printf( "-DSI-\n" );
//	probeSection( "dsmcc_multiple_files/DSI.sec" );
//	ASSERT_TRUE( _dsi ? true : false );
//
//	printf( "-DII-\n" );
//	probeSection( "dsmcc_multiple_files/DII.sec" );
//	ASSERT_TRUE( _dii ? true : false );
//
//	printf( "-Module-01-\n" );
//	char name[255];
//	for (int i=0; i<=16; i++) {
//		snprintf( name, 255, "dsmcc_multiple_files/0001_%06d.sec", i );
//		probeSection( name );
//	}
//	ASSERT_TRUE( _module ? true : false );
//
//	printf( "-Module-02-\n" );
//	for (int i=0; i<=5; i++) {
//		snprintf( name, 255, "dsmcc_multiple_files/0002_%06d.sec", i );
//		probeSection( name );
//	}
//	ASSERT_TRUE( _module ? true : false );
//
//	printf( "-Module-03-\n" );
//	for (int i=0; i<=11; i++) {
//		snprintf( name, 255, "dsmcc_multiple_files/0003_%06d.sec", i );
//		probeSection( name );
//	}
//	ASSERT_TRUE( _module ? true : false );
}
