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

#include "extension.h"
#include "../../../../../src/demuxer/psi/nit.h"

EwbsTest::EwbsTest()
{
}

EwbsTest::~EwbsTest()
{
}

void EwbsTest::SetUp()
{
	ServiceManagerTest::SetUp();
}

void EwbsTest::TearDown() {
}

void EwbsTest::onEmergency( bool state ) {
	states.push_back( state );
}

void EwbsTest::start( tuner::ewbs::Extension *ext ) {
	ext->onEmergency( boost::bind(&EwbsTest::onEmergency,this,_1) );
	initMgr( ext );
}

tuner::Service *EwbsTest::createSrv( tuner::ID srvID, tuner::ID srvIDAlert, bool haveSI/*=true*/ ) {
	tuner::ID srvPID = 100;
	tuner::ID pcrPID = 1000;

	tuner::ServiceID id(1, 1, srvID);
	tuner::Service *srv = new tuner::Service( 8, id, srvPID );

	{	//	PMT
		std::vector<tuner::ElementaryInfo> elems;
		if (haveSI) {
			test::addStream( elems, 0x6, 1001, 0x38 );
			{	//	Add data component descriptor
				util::BYTE desc[] = { 0xFD, 0x2, 0x00, 0x08 };
				elems[0].descriptors.addDescriptor( desc, 4 );
			}
		}

		tuner::desc::Descriptors desc;
		{	//	0xFC
			util::BYTE hiAlert = srvIDAlert >> 8;
			util::BYTE loAlert = srvIDAlert & 0x00FF;
			util::BYTE fcDescriptor[] = { 0xFC, 0x06, hiAlert, loAlert, 0xFF, 0x02, 0x0E, 0x7F };
			desc.addDescriptor( fcDescriptor, sizeof(fcDescriptor)/sizeof(util::BYTE) );
		}

		tuner::Pmt *pmt = new tuner::Pmt( srvPID, 1, srvID, pcrPID, desc, elems );
		boost::shared_ptr<tuner::Pmt> ptr(pmt);
		srv->process( ptr );
	}

	{	//	NIT
		std::vector<tuner::Nit::ElementaryInfo> elements;

		tuner::Nit::ElementaryInfo info;
		info.tsID = 1;
		info.nitID = 1;
		elements.push_back( info );

		tuner::desc::Descriptors desc;
		{       //      0xFA
			util::BYTE faDescriptor[] = { 0xFA, 0x02, 0x0E, 0x7F };
			desc.addDescriptor( faDescriptor, sizeof(faDescriptor)/sizeof(util::BYTE) );
		}

		tuner::Nit *nit = new tuner::Nit( 1, 1, true, desc, elements );
		boost::shared_ptr<tuner::Nit> ptr( nit );
		srv->process( ptr );
	}

	return srv;
}

TEST_F( EwbsTest, constructor ) {
	tuner::ewbs::Extension ext;
}

TEST_F( EwbsTest, basic ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension(231);
	start( ext );
	tuner::ID srvID = 0x7da;
	tuner::Service *srv = createSrv( srvID, srvID );
	mgr()->readySrv( srv );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );
	ASSERT_TRUE( ext->inEmergency( srvID ) );

	const tuner::ewbs::EmergencyInfo &info = ext->getAll();
	ASSERT_EQ( info.size(), 1 );
	ASSERT_EQ( info[0].srvID, srvID );
	ASSERT_EQ( info[0].level, 1 );

	stopSrv( srv );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
}

TEST_F( EwbsTest, alert_unknown_service ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension(231);

	start( ext );
	tuner::ID srvID = 0x7da;
	tuner::ID alertID = 0x7db;
	tuner::Service *srv = createSrv( srvID, alertID );
	mgr()->readySrv( srv );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );
	ASSERT_FALSE( ext->inEmergency( srvID ) );
	ASSERT_TRUE( ext->inEmergency( alertID ) );

	const tuner::ewbs::EmergencyInfo &info = ext->getAll();
	ASSERT_EQ( info.size(), 1 );
	ASSERT_EQ( info[0].srvID, alertID );
	ASSERT_EQ( info[0].level, 1 );

	stopSrv( srv );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
}

TEST_F( EwbsTest, two_service ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension(231);
	start( ext );

	//	Start first srv
	tuner::ID srvID = 0x7da;
	tuner::Service *srv1 = createSrv( srvID, srvID );
	mgr()->readySrv( srv1 );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );
	ASSERT_TRUE( ext->inEmergency( srvID ) );

	{
		const tuner::ewbs::EmergencyInfo &info = ext->getAll();
		ASSERT_EQ( info.size(), 1 );
		ASSERT_EQ( info[0].srvID, srvID );
		ASSERT_EQ( info[0].level, 1 );
	}

	tuner::ID srvID2 = 0x7db;
	tuner::Service *srv2 = createSrv( srvID2, srvID2 );
	mgr()->readySrv( srv2 );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_TRUE( ext->inEmergency( srvID2 ) );

	{
		const tuner::ewbs::EmergencyInfo &info = ext->getAll();
		ASSERT_EQ( info.size(), 2 );
		ASSERT_EQ( info[1].srvID, srvID2 );
		ASSERT_EQ( info[1].level, 1 );
	}

	stopSrv( srv1 );
	ASSERT_EQ( states.size(), 1 );

	stopSrv( srv2 );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
}

TEST_F( EwbsTest, one_service_active_freq_with_two_services ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension(231);
	start( ext );

	tuner::ID srvID = 0x7da;
	tuner::ID srvID2 = 0x7db;

	//	Start first srv
	tuner::Service *srv1 = createSrv( srvID, srvID2 );
	mgr()->readySrv( srv1 );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );
	ASSERT_FALSE( ext->inEmergency( srvID ) );
	ASSERT_TRUE( ext->inEmergency( srvID2 ) );

	tuner::Service *srv2 = createSrv( srvID2, srvID2 );
	mgr()->readySrv( srv2 );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_FALSE( ext->inEmergency( srvID ) );
	ASSERT_TRUE( ext->inEmergency( srvID2 ) );

	stopSrv( srv1 );
	ASSERT_EQ( states.size(), 1 );

	stopSrv( srv2 );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
}

TEST_F( EwbsTest, alert_stop_alert ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension(231);
	start( ext );

	tuner::ID srvID = 0x7da;
	tuner::ID srvID2 = 0x7db;

	//	Start first srv
	tuner::Service *srv1 = createSrv( srvID, srvID );
	mgr()->readySrv( srv1 );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );
	ASSERT_TRUE( ext->inEmergency( srvID ) );

	stopSrv( srv1 );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
	ASSERT_FALSE( ext->inEmergency( srvID ) );

	//	Start second srv
	tuner::Service *srv2 = createSrv( srvID2, srvID2 );
	mgr()->readySrv( srv2 );
	ASSERT_EQ( states.size(), 3 );
	ASSERT_EQ( states[2], true );
	ASSERT_TRUE( ext->inEmergency( srvID2 ) );

	stopSrv( srv2 );
	ASSERT_EQ( states.size(), 4 );
	ASSERT_EQ( states[3], false );
	ASSERT_FALSE( ext->inEmergency( srvID2 ) );
}

TEST_F( EwbsTest, automatic_setting ) {
	tuner::ewbs::Extension *ext = new tuner::ewbs::Extension();
	start( ext );
	tuner::ID srvID = 0x7da;
	tuner::Service *srv = createSrv( srvID, srvID );
	mgr()->readySrv( srv );
	ASSERT_EQ( states.size(), 1 );
	ASSERT_EQ( states[0], true );

	stopSrv( srv );
	ASSERT_EQ( states.size(), 2 );
	ASSERT_EQ( states[1], false );
}

