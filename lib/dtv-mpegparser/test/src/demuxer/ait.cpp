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

#include "ait.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/application/types.h"
#include "../../../src/demuxer/psi/application/ait.h"
#include "../../../src/demuxer/psi/application/aitdemuxer.h"
#include <boost/bind.hpp>
#include <util/buffer.h>
#include <boost/foreach.hpp>

#define TEST_SUBROUTINE( test ) \
	test; \
	ASSERT_FALSE(HasFatalFailure());


AitTest::AitTest() {
}

AitTest::~AitTest() {
}

void AitTest::onAit( const boost::shared_ptr<tuner::Ait> &ait ) {
	_ait = ait;
}

tuner::PSIDemuxer *AitTest::createDemux( ) {
	return tuner::createDemuxer<tuner::AITDemuxer, tuner::Ait > (
			pid( ),
			boost::bind( &AitTest::onAit, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void AitTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe( );
	_ait.reset();
}

tuner::ID AitTest::pid() const {
	return 0x04; //TODO: ver pid en documentacion de ARIB
}

void AitTest::testBasic() {
	ASSERT_TRUE( _ait != NULL );
	ASSERT_EQ( wasExpired( ), 0 );
	ASSERT_TRUE( _ait->descriptors( ).empty() );
	ASSERT_EQ( _ait->applications( ).size( ), 1 );
}

void AitTest::testWithApps() {
	TEST_SUBROUTINE(testBasic());
	ASSERT_TRUE( !_ait->applications( )[0].descriptors.empty( ) );
}

void AitTest::testWithoutApps() {
	TEST_SUBROUTINE(testBasic());
	ASSERT_TRUE( _ait->applications( )[0].descriptors.empty( ) );
}

// Aux tests
static void testGingaLocationDescriptor( const tuner::desc::Descriptors desc, const std::string base,
		const std::string classPath, const std::string initialClass ) {
	tuner::ait::GingaApplicationLocationDescriptor appLoc;

	ASSERT_TRUE( AIT_DESC_PARSE(desc, ginga_ncl_app_location, appLoc) );
	ASSERT_EQ( appLoc.base, base );
	ASSERT_EQ( appLoc.classPath, classPath );
	ASSERT_EQ( appLoc.initialClass, initialClass );
}

static void testTransportProtocol(
	tuner::ait::TransportProtocolDescriptor desc,
	const util::WORD id,
	const util::BYTE label )
{
	ASSERT_EQ( desc.protocolID, id );
	ASSERT_EQ( desc.label, label );
}

static void testApplicationNameDescriptor( const tuner::desc::Descriptors desc, const std::string name ) {
	tuner::ait::ApplicationNameDescriptor appNameDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, name, appNameDesc));
	ASSERT_EQ( appNameDesc.size(), 1 );
	ASSERT_EQ( appNameDesc[0].name, name );
}

static void testApplicationDescriptor(
	const tuner::ait::ApplicationDescriptor &desc,
	const util::BYTE boundFlag,
	const util::BYTE visibility,
	const util::BYTE priority )
{
	ASSERT_EQ( desc.serviceBoundFlag, boundFlag );
	ASSERT_EQ( desc.visibility, visibility );
	ASSERT_EQ( desc.priority, priority );
}

static void testFirstApplicationProfile(
	const tuner::ait::ApplicationProfile &appProfile,
	const util::WORD profile,
	const util::BYTE major,
	const util::BYTE minor,
	const util::BYTE micro )
{
	ASSERT_EQ( appProfile.profile, profile );
	ASSERT_EQ( appProfile.major, major );
	ASSERT_EQ( appProfile.minor, minor );
	ASSERT_EQ( appProfile.micro, micro );
}

static void testApplication( const tuner::desc::Descriptors &desc, const std::string &name ) {
	ASSERT_TRUE( !desc.empty() );

	tuner::ait::ApplicationNameDescriptor appNameDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, name, appNameDesc));
	ASSERT_EQ( appNameDesc.size(), 1 );

	const tuner::ait::ApplicationName &appName0 = appNameDesc[0];
	ASSERT_EQ( appName0.name, name );

	tuner::ait::TransportDescriptor protocols;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, transport_protocol, protocols));
	ASSERT_EQ( protocols.size(), 1 );
	tuner::ait::TransportProtocolDescriptor protocol = protocols[0];

	testTransportProtocol(protocol, 0x01, 0x00);
	ASSERT_FALSE(::testing::Test::HasFatalFailure());
	tuner::ait::DSMCCTransportProtocolDescriptor dsmcc0 = protocol.info.get<tuner::ait::DSMCCTransportProtocolDescriptor > ();
	ASSERT_EQ( dsmcc0.remote, 0 );
	ASSERT_EQ( dsmcc0.componentTag, 0x0C );

	tuner::ait::ApplicationDescriptor application;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, application, application));
	testApplicationDescriptor(application, 1, 3, 1);
	ASSERT_FALSE(::testing::Test::HasFatalFailure());

	ASSERT_EQ( application.profiles.size(), 1 );
	testFirstApplicationProfile( application.profiles[0], 1, 1, 0, 0 );
	ASSERT_FALSE(::testing::Test::HasFatalFailure());
	ASSERT_EQ( application.transports.size(), 1 );
	ASSERT_EQ( application.transports[0], 0 );
}

// Tests

TEST_F( AitTest, create_demux ) {
}

TEST_F( AitTest, ait_simple_noapp ) {
	probeSection( "ait_simple_ver0_sec0.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( _ait->version( ) == 0 );
	ASSERT_TRUE( _ait->appType( ) == 0x09 );
	ASSERT_TRUE( _ait->applications( ).size( ) == 0 );
	ASSERT_TRUE( _ait->descriptors( ).empty() );
}

TEST_F( AitTest, ait_singleapp ) {
	probeSection( "ait_single_app_ver1_sec0_lsn0.sec" );
	TEST_SUBROUTINE(testWithApps());

	ASSERT_TRUE( _ait->version( ) == 1 );
	ASSERT_TRUE( _ait->appType( ) == 0x09 );
}

TEST_F( AitTest, ait_singleapp_application_name_descriptor ) {
	probeSection( "ait_single_app_ver1_sec0_lsn0.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::desc::Descriptors &desc = _ait->applications( )[0].descriptors;
	tuner::ait::ApplicationNameDescriptor appNameDesc;

	ASSERT_TRUE( AIT_DESC_PARSE(desc, name, appNameDesc) );

	const tuner::ait::ApplicationName &appName = appNameDesc[0];
	ASSERT_TRUE( appName.name == "COCINEROS" );
}

TEST_F( AitTest, ait_singleapp_ginga_application_descriptor ) {
	probeSection( "ait_single_app_ver1_sec0_lsn0.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::desc::Descriptors &desc = _ait->applications()[0].descriptors;

	TEST_SUBROUTINE( testGingaLocationDescriptor(desc, "/", "", "main.ncl") );
}

TEST_F( AitTest, ait_singleapp_application_descriptor ) {
	probeSection( "ait_single_app_ver1_sec0_lsn0.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::ait::Application &app0 = _ait->applications()[0];
	ASSERT_TRUE( !app0.descriptors.empty() );
	const tuner::desc::Descriptors &desc = app0.descriptors;

	tuner::ait::ApplicationDescriptor appDesc;
	ASSERT_TRUE( AIT_DESC_PARSE( desc, application, appDesc ));

	ASSERT_TRUE( appDesc.profiles.size() == 1 );

	TEST_SUBROUTINE(testFirstApplicationProfile( appDesc.profiles[0], 1, 1, 0, 0 ));

	ASSERT_TRUE( appDesc.transports.size( ) == 1 );
	ASSERT_TRUE( appDesc.transports[0] == 0 );
}

TEST_F( AitTest, ait_singleapp_protocol_descriptors ) {
	probeSection( "ait_single_app_ver1_sec0_lsn0.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::ait::Application &app0 = _ait->applications()[0];
	ASSERT_TRUE( !app0.descriptors.empty( ) );

	const tuner::desc::Descriptors &desc = app0.descriptors;

	tuner::ait::TransportDescriptor protocols;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, transport_protocol, protocols));
	ASSERT_TRUE( protocols.size( ) == 1 );

	TEST_SUBROUTINE(testTransportProtocol(protocols[0], 1, 0));

	const tuner::ait::DSMCCTransportProtocolDescriptor &dsmccDesc = protocols[0]
			.info.get<tuner::ait::DSMCCTransportProtocolDescriptor > ();
	ASSERT_TRUE( dsmccDesc.remote == 0 );
	//ASSERT_TRUE( dsmccDesc.networkID == ? ? );
	//ASSERT_TRUE( dsmccDesc.tsID == ? ? );
	//ASSERT_TRUE( dsmccDesc.serviceID == ? ? );
	ASSERT_TRUE( dsmccDesc.componentTag == 0x0C );
}

TEST_F( AitTest, ait_singleapp_application_descriptor_limits_0 ) {
	probeSection( "ait_app_desc_limits_0.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::ait::Application &app0 = _ait->applications()[0];
	const tuner::desc::Descriptors &desc = app0.descriptors;

	tuner::ait::ApplicationDescriptor appDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, application, appDesc));

	ASSERT_EQ( appDesc.profiles.size(), 1 );
	TEST_SUBROUTINE(testFirstApplicationProfile( appDesc.profiles[0], 0xFFFF, 0xFF, 0xFF, 0xFF ));

	TEST_SUBROUTINE(testApplicationDescriptor(appDesc, 1, 3, 0xFF));

	ASSERT_TRUE( appDesc.transports.size( ) == 7 );
	ASSERT_TRUE( appDesc.transports[0] == 0 );
	ASSERT_TRUE( appDesc.transports[1] == 1 );
	ASSERT_TRUE( appDesc.transports[2] == 2 );
	ASSERT_TRUE( appDesc.transports[3] == 0x0F );
	ASSERT_TRUE( appDesc.transports[4] == 0xAA );
	ASSERT_TRUE( appDesc.transports[5] == 0xFF );
	ASSERT_TRUE( appDesc.transports[6] == 0 );
}

TEST_F( AitTest, ait_application_descriptor_limits_1 ) {
	probeSection( "ait_app_desc_limits_1.sec" );
	TEST_SUBROUTINE(testWithApps());

	const tuner::desc::Descriptors &desc = _ait->applications( )[0].descriptors;

	tuner::ait::ApplicationDescriptor appDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, application, appDesc));

	ASSERT_EQ( appDesc.profiles.size(), 1 );

	TEST_SUBROUTINE(testFirstApplicationProfile( appDesc.profiles[0], 0x0180, 0, 0xF0, 0xFF ));

	TEST_SUBROUTINE(testApplicationDescriptor(appDesc, 0, 0, 0xF0));

	ASSERT_TRUE( appDesc.transports.size( ) == 5 );
	ASSERT_TRUE( appDesc.transports[0] == 0xAA );
	ASSERT_TRUE( appDesc.transports[1] == 0xFF );
	ASSERT_TRUE( appDesc.transports[2] == 0xFF );
	ASSERT_TRUE( appDesc.transports[3] == 0xF0 );
	ASSERT_TRUE( appDesc.transports[4] == 0x0F );
}

TEST_F( AitTest, ait_application_descriptor_limits_2 ) {
	probeSection( "ait_app_desc_limits_2.sec" );
	TEST_SUBROUTINE(testWithApps());

	ASSERT_TRUE( _ait->version( ) == 2 );

	const tuner::desc::Descriptors &desc = _ait->applications( )[0].descriptors;
	tuner::ait::ApplicationDescriptor appDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, application, appDesc));

	ASSERT_EQ( appDesc.profiles.size(), 1 );

	TEST_SUBROUTINE(testFirstApplicationProfile( appDesc.profiles[0], 0x0180, 0, 0xF0, 0xFF ));

	TEST_SUBROUTINE(testApplicationDescriptor(appDesc, 0, 0, 0xF0));

	ASSERT_TRUE( appDesc.transports.size( ) == 240 );
	for ( int i = 0; i < 240; i++ )
		ASSERT_TRUE( appDesc.transports[i] == i );
}

TEST_F( AitTest, DISABLED_ait_long_name_descriptor ) {
	probeSection( "ait_long_name_descriptor.sec" );
	TEST_SUBROUTINE(testWithApps());

	ASSERT_TRUE( _ait->version( ) == 1 );

	const tuner::desc::Descriptors &desc = _ait->applications( )[0].descriptors;
	ASSERT_TRUE( !desc.empty( ));

	tuner::ait::ApplicationNameDescriptor appNameDesc;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, name, appNameDesc));

	const tuner::ait::ApplicationName &appName = appNameDesc[0];
	ASSERT_EQ( appName.name, "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJK"
			"LMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRS"
			"TUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopq"
			"rstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnoprst" );

	TEST_SUBROUTINE(testGingaLocationDescriptor(
	desc,
	"1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJ"
	"KLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQR"
	"STUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnop"
	"qrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnoprst",
	"1234567890abcdefghijklmnopqrstuvwxyzABCDE"
	"FGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLM"
	"NOPQRSTUVWXYZ1234567890abcdef", ""));
}

TEST_F( AitTest, ait_singleapp_application_limits ) {
	probeSection( "ait_orgid_appid_ccode_limit.sec" );
	testBasic();

	const tuner::ait::Application app = _ait->applications( )[0];
	ASSERT_TRUE( app.orgID == 0xFFFFFFFF );
	ASSERT_TRUE( app.id == 0xFF );
	ASSERT_TRUE( app.ctrlCode == 0xFF );
}

TEST_F( AitTest, ait_singleapp_values1fff1fff ) {
	probeSection( "ait_orgid_1fff1fff.sec" );
	testWithoutApps();

	const tuner::ait::Application app = _ait->applications( )[0];
	ASSERT_TRUE( app.orgID == 0x1FFF1FFF );
}

TEST_F( AitTest, ait_singleapp_applicationid_wildcard_fffe ) {
	probeSection( "ait_appID_wildcard_fffe.sec" );
	testWithoutApps();

	const tuner::ait::Application app = _ait->applications( )[0];
	ASSERT_TRUE( app.id == 0xFFFE );
}

TEST_F( AitTest, ait_singleapp_applicationID_wildcard_ffff ) {
	probeSection( "ait_appID_wildcard_ffff.sec" );
	testWithoutApps();

	const tuner::ait::Application app = _ait->applications( )[0];
	ASSERT_TRUE( app.id == 0xFFFF );
}

TEST_F( AitTest, ait_singleapp_applicationID_reserved ) {
	probeSection( "ait_appID_reserved_fffd.sec" );
	testWithoutApps();

	const tuner::ait::Application app = _ait->applications( )[0];
	ASSERT_TRUE( app.id == 0xFFFD );
}

TEST_F( AitTest, ait_change_version ) {
	util::BYTE tagsToCheck[] = {};	//	TODO: Add section with descriptors
	probeSection( "ait_ver0_cni1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->version( ) == 0 );
	ASSERT_EQ( _ait->applications().size(), 1 );
	ASSERT_TRUE( CHECK_TAGS(_ait->descriptors(), tagsToCheck) );

	probeSection( "ait_ver0_cni1.sec" );
	ASSERT_FALSE( _ait );
	ASSERT_TRUE( wasExpired( ) == 0 );

	probeSection( "ait_ver1_cni1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 1 );
	ASSERT_TRUE( _ait->version( ) == 1 );
	ASSERT_EQ( _ait->applications().size(), 1 );
	ASSERT_TRUE( CHECK_TAGS(_ait->descriptors(), tagsToCheck) );

	probeSection( "ait_ver1_cni1.sec" );
	ASSERT_FALSE( _ait );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( AitTest, ait_change_version_cni ) {
	probeSection( "ait_ver0_cni1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->version( ) == 0 );

	probeSection( "ait_ver1_cni0.sec" );
	ASSERT_FALSE( _ait );
	ASSERT_TRUE( wasExpired( ) == 1 );

	probeSection( "ait_ver1_cni1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->version( ) == 1 );
}

TEST_F( AitTest, ait_multi_app1 ) {
	probeSection( "ait_multiple_app_10_11_12_ver1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->version( ) == 1 );

	// should be 3 application within this section
	ASSERT_TRUE( _ait->applications( ).size( ) == 3 );
	ASSERT_TRUE( _ait->applications( )[0].id == 0x10 );
	ASSERT_TRUE( _ait->applications( )[1].id == 0x11 );
	ASSERT_TRUE( _ait->applications( )[2].id == 0x12 );

	{//	Application 0
		const tuner::desc::Descriptors desc0 = _ait->applications( )[0].descriptors;
		TEST_SUBROUTINE(testApplication(desc0, "app_a"));
	}

	{	//	Application 1
		const tuner::desc::Descriptors desc1 = _ait->applications( )[1].descriptors;
		TEST_SUBROUTINE(testApplication(desc1, "app_b"));
	}

	{	//	Application 2
		const tuner::desc::Descriptors desc2 = _ait->applications( )[2].descriptors;
		TEST_SUBROUTINE(testApplication(desc2, "app_c"));
	}
}

TEST_F( AitTest, ait_multi_app2 ) {
	probeSection( "ait_multiple_app_10_12_13_ver2.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->version( ) == 2 );
	ASSERT_TRUE( _ait->applications( ).size( ) == 3 );
	ASSERT_TRUE( _ait->applications( )[0].id == 0x10 );
	ASSERT_TRUE( _ait->applications( )[1].id == 0x12 );
	ASSERT_TRUE( _ait->applications( )[2].id == 0x13 );

	const tuner::desc::Descriptors &desc0 = _ait->applications( )[0].descriptors;
	const tuner::desc::Descriptors &desc1 = _ait->applications( )[1].descriptors;
	const tuner::desc::Descriptors &desc2 = _ait->applications( )[2].descriptors;

	testApplicationNameDescriptor( desc0, "app_a" );
	testApplicationNameDescriptor( desc1, "app_c" );
	testApplicationNameDescriptor( desc2, "app_d" );
}

TEST_F( AitTest, ait_multi_app_change ) {
	probeSection( "ait_multiple_app_10_11_12_ver1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _ait->version() == 1 );

	probeSection( "ait_multiple_app_10_12_13_ver2.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired() != 0 ); //	it should be expired
	ASSERT_TRUE( _ait->version() == 2 );
	ASSERT_TRUE( _ait->applications().size( ) == 3 );
	ASSERT_TRUE( _ait->applications()[0].id == 0x10 );
	ASSERT_TRUE( _ait->applications()[1].id == 0x12 );
	ASSERT_TRUE( _ait->applications()[2].id == 0x13 );

	const tuner::desc::Descriptors &desc0 = _ait->applications()[0].descriptors;
	const tuner::desc::Descriptors &desc1 = _ait->applications()[1].descriptors;
	const tuner::desc::Descriptors &desc2 = _ait->applications()[2].descriptors;

	TEST_SUBROUTINE(testApplicationNameDescriptor( desc0, "app_a" ));
	TEST_SUBROUTINE(testApplicationNameDescriptor( desc1, "app_c" ));
	TEST_SUBROUTINE(testApplicationNameDescriptor( desc2, "app_d" ));
}

TEST_F( AitTest, ait_repeat ) {
	probeSection( "ait_multiple_app_10_11_12_ver1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );

	probeSection( "ait_multiple_app_10_11_12_ver1.sec" );
	ASSERT_FALSE( _ait );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( AitTest, ait_version_change ) {
	probeSection( "ait_multiple_app_10_11_12_ver1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_FALSE( wasExpired( ) );
	ASSERT_TRUE( _ait->version( ) == 1 );

	probeSection( "ait_multiple_app_10_12_13_ver2.sec" );
	ASSERT_TRUE( wasExpired( ) != 0 );
	ASSERT_TRUE( _ait->version( ) == 2 );
}

void testBoundFlag( const tuner::desc::Descriptors desc, const bool flag ) {
	tuner::ait::ApplicationDescriptor app;
	ASSERT_TRUE( AIT_DESC_PARSE(desc, application, app));
	ASSERT_TRUE( app.serviceBoundFlag == flag );
}

TEST_F( AitTest, ait_external_application ) {
	probeSection( "ait_ext_app_ver0.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( _ait->version( ) == 0 );
	ASSERT_TRUE( _ait->applications( ).size( ) == 2 );
	ASSERT_TRUE( _ait->applications( )[0].id == 0x10 );
	ASSERT_TRUE( _ait->applications( )[1].id == 0x11 );
	ASSERT_TRUE( _ait->applications( )[0].orgID == 0xa );
	ASSERT_TRUE( _ait->applications( )[1].orgID == 0xb );

	const tuner::desc::Descriptors &desc0 = _ait->applications()[0].descriptors;
	const tuner::desc::Descriptors &desc1 = _ait->applications()[1].descriptors;

	TEST_SUBROUTINE(testApplicationNameDescriptor( desc0, "app_10" ));
	TEST_SUBROUTINE(testApplicationNameDescriptor( desc1, "app_11" ));
	TEST_SUBROUTINE(testBoundFlag( desc0, 1 ));
	TEST_SUBROUTINE(testBoundFlag( desc1, 0 ));

	probeSection( "ait_ext_app_ver1.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( wasExpired( ) != 0 );
	ASSERT_TRUE( _ait->version( ) == 1 );

	const tuner::desc::Descriptors &desc = _ait->descriptors( );
	tuner::ait::ExternalApplications apps;

	ASSERT_TRUE( AIT_DESC_PARSE(desc, external_app_auth, apps));
	ASSERT_TRUE( apps.size( ) == 1 );
	ASSERT_TRUE( apps[0].orgID == 0xB );
	ASSERT_TRUE( apps[0].id == 0x11 );
	ASSERT_TRUE( apps[0].priority == 0x10 );
}

TEST_F( AitTest, ait_transport_protocol_in_common_loop ) {
	probeSection( "ait_common_loop_02.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_FALSE( wasExpired( ) );
	ASSERT_TRUE( _ait->version( ) == 1 );

	const tuner::desc::Descriptors &desc = _ait->descriptors( );
	tuner::ait::TransportDescriptor protocols;

	ASSERT_TRUE( AIT_DESC_PARSE(desc, transport_protocol, protocols));
	ASSERT_EQ( protocols.size(), 1 );

	TEST_SUBROUTINE(testTransportProtocol(protocols[0], 0x02, 0x03));
}

TEST_F( AitTest, ait_several_transport_protocols ) {
	probeSection( "ait_several_transport_protocol.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( _ait->version( ) == 2 );

	const tuner::desc::Descriptors &desc = _ait->applications( )[0].descriptors;
	tuner::ait::TransportDescriptor protocols;

	ASSERT_TRUE( AIT_DESC_PARSE(desc, transport_protocol, protocols));
	ASSERT_TRUE( protocols.size( ) == 8 );
	TEST_SUBROUTINE(testTransportProtocol( protocols[0], 1, 0 ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[1], 1, 1 ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[2], 1, 2 ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[3], 1, 0xA ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[4], 1, 0xF ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[5], 1, 0xFA ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[6], 1, 0xFE ));
	TEST_SUBROUTINE(testTransportProtocol( protocols[7], 1, 0xFF ));
}

TEST_F( AitTest, ait_big_common_desc_loop ) {
	probeSection( "ait_big_common_desc_loop.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( _ait->version( ) == 1 );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( AitTest, ait_empty_names_and_location ) {
	probeSection( "ait_empty_names_and_location.sec" );
	ASSERT_TRUE( _ait != NULL );
	ASSERT_TRUE( _ait->version( ) == 1 );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _ait->applications( ).size( ) == 1 );

	const tuner::desc::Descriptors &desc0 = _ait->applications( )[0].descriptors;
	TEST_SUBROUTINE(testApplicationNameDescriptor( desc0, "" ));
	TEST_SUBROUTINE(testGingaLocationDescriptor( desc0 , "", "", "" ));
}

TEST_F( AitTest, ait_syntx_ind_0_crc32_error ) {
	probeSection( "ait_ver0_cni1_synt_crc.sec" );
	ASSERT_FALSE( _ait );
}

TEST_F( AitTest, ait_syntx_ind0 ) {
	probeSection( "ait_ver0_cni1_synt.sec" );
	ASSERT_FALSE( _ait );
}

TEST_F( AitTest, ait_err ) {
	probeSection( "ait_err.sec" );
	ASSERT_FALSE( _ait );
}

TEST_F( AitTest, ait_crc_error ) {
	probeSection( "ait_ver0_cni1_crc.sec" );
	ASSERT_FALSE( _ait );
}

TEST_F( AitTest, ait_invalid_tableid ) {
	probeSection( "ait_invalid_tid.sec" );
	ASSERT_FALSE( _ait );
}
