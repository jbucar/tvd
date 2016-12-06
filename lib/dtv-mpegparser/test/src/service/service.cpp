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
#include "../util.h"
#include "../../../src/service/service.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/pmt.h"
#include "../../../src/demuxer/psi/nit.h"
#include "../../../src/demuxer/psi/sdt.h"
#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>

tuner::Pmt *createPmt( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID ) {
	std::vector<tuner::ElementaryInfo> elems;
	test::addStream( elems, 0x1b, 1001 );
	test::addStream( elems, 0x11, 1002 );

	tuner::desc::Descriptors desc;

	{       //      0xFC
		util::BYTE fcDescriptor[] = { 0xFC, 0x06, 0xF1, 0xC0, 0xFF, 0x02, 0x0E, 0x7F };
		desc.addDescriptor( fcDescriptor, sizeof(fcDescriptor)/sizeof(util::BYTE) );
	}

	{       //      0xFA
		util::BYTE faDescriptor[] = { 0xFA, 0x02, 0x0E, 0x7F };
		desc.addDescriptor( faDescriptor, sizeof(faDescriptor)/sizeof(util::BYTE) );
	}

	return new tuner::Pmt( srvPID, 1, srvID, pcrPID, desc, elems );
}

tuner::Nit *createNit() {
	std::vector<tuner::Nit::ElementaryInfo> elements;

	tuner::Nit::ElementaryInfo info;
	info.tsID = 1;
	info.nitID = 1;
	elements.push_back( info );

	tuner::desc::Descriptors desc;
	{       //      0xFC
		util::BYTE fcDescriptor[] = { 0xFC, 0x06, 0xF1, 0xC0, 0xFF, 0x02, 0x0E, 0x7F };
		desc.addDescriptor( fcDescriptor, sizeof(fcDescriptor)/sizeof(util::BYTE) );
	}
	{       //      0xFA
		util::BYTE faDescriptor[] = { 0xFA, 0x02, 0x0E, 0x7F };
		desc.addDescriptor( faDescriptor, sizeof(faDescriptor)/sizeof(util::BYTE) );
	}

	return new tuner::Nit( 1, 1, true, desc, elements );
}

tuner::Sdt *createSdt( tuner::ID srvID ) {
	std::vector<tuner::Sdt::Service> services;

	tuner::Sdt::Service srv;
	srv.serviceID = srvID;
	srv.eitSchedule = false;
	srv.eitPresentFollowing = true;

	{       //      0xFC
		util::BYTE fcDescriptor[] = { 0xFC, 0x06, 0xF1, 0xC0, 0xFF, 0x02, 0x0E, 0x7F };
		srv.descriptors.addDescriptor( fcDescriptor, sizeof(fcDescriptor)/sizeof(util::BYTE) );
	}
	{       //      0xFA
		util::BYTE faDescriptor[] = { 0xFA, 0x02, 0x0E, 0x7F };
		srv.descriptors.addDescriptor( faDescriptor, sizeof(faDescriptor)/sizeof(util::BYTE) );
	}

	services.push_back( srv );
	return new tuner::Sdt( 1, 1, 1, true, services );
}

TEST( Service, custom_constructor ) {
	tuner::ServiceID id(1,1,10);
	tuner::Service ser( 8, id, 11 );

	ASSERT_TRUE( ser.networkID() == 8 );
	ASSERT_TRUE( ser.serviceID().nitID == 1 );
	ASSERT_TRUE( ser.serviceID().tsID == 1 );
	ASSERT_TRUE( ser.id() == 10 );
	ASSERT_TRUE( ser.pid() == 11 );
	ASSERT_TRUE( ser.pcrPID() == TS_PID_NULL );
	ASSERT_TRUE( ser.type() == tuner::service::type::dtv );
	ASSERT_TRUE( ser.status() == tuner::service::status::undefined );
	ASSERT_TRUE( ser.state() == tuner::service::state::present );
	ASSERT_TRUE( ser.provider().empty() );
	ASSERT_TRUE( ser.name().empty() );
	ASSERT_TRUE( ser.networkName().empty() );
	ASSERT_TRUE( ser.elements().empty() );
}

TEST( Service, process_valid_pmt ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ID pcrPID = 10;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );

	boost::shared_ptr<tuner::Pmt> pmt( createPmt( programID, pid, pcrPID ) );
    ASSERT_TRUE( ser.process( pmt ) );

	ASSERT_TRUE( ser.networkID() == 8 );
	ASSERT_TRUE( ser.serviceID().nitID == 1 );
	ASSERT_TRUE( ser.serviceID().tsID == 1 );
	ASSERT_TRUE( ser.id() == programID );
	ASSERT_TRUE( ser.pid() == pid );
	ASSERT_TRUE( ser.pcrPID() == pcrPID );
	ASSERT_TRUE( ser.type() == tuner::service::type::dtv );
	ASSERT_TRUE( ser.state() == tuner::service::state::complete );
	ASSERT_TRUE( ser.provider().empty() );
	ASSERT_TRUE( ser.name().empty() );
	ASSERT_TRUE( ser.networkName().empty() );
	ASSERT_TRUE( ser.elements().size() == 2 );

	util::BYTE tagsToCheck[] = { 0xFC, 0xFA };
	ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::pmt), tagsToCheck) );
}

TEST( Service, process_other_pmt_programID ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ID pcrPID = 10;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );

	boost::shared_ptr<tuner::Pmt> pmt( createPmt( programID+1, pid, pcrPID ) );
	ASSERT_DEATH( ser.process( pmt ), "" );
}

TEST( Service, process_other_pmt_pid ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ID pcrPID = 10;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );

	boost::shared_ptr<tuner::Pmt> pmt( createPmt( programID, pid+1, pcrPID ) );
	ASSERT_DEATH( ser.process( pmt ), "" );
}

TEST( Service, process_pmt_update ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ID pcrPID = 10;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );
	util::BYTE tagsToCheck[] = { 0xFC, 0xFA };

	{
		boost::shared_ptr<tuner::Pmt> pmt( createPmt( programID, pid, pcrPID ) );
		ASSERT_TRUE( ser.process( pmt ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::pmt), tagsToCheck) );
	}

	ser.expire( tuner::service::desc::pmt );

	{
		boost::shared_ptr<tuner::Pmt> pmt( createPmt( programID, pid, pcrPID ) );
		ASSERT_TRUE( ser.process( pmt ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::pmt), tagsToCheck) );
	}
}

TEST( Service, process_nit ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );
	util::BYTE tagsToCheck[] = { 0xFC, 0xFA };

	{
		boost::shared_ptr<tuner::Nit> nit( createNit() );
		ASSERT_FALSE( ser.process( nit ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::nit), tagsToCheck) );
	}

	ser.expire( tuner::service::desc::nit );

	{
		boost::shared_ptr<tuner::Nit> nit( createNit() );
		ASSERT_FALSE( ser.process( nit ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::nit), tagsToCheck) );
	}
}

TEST( Service, process_sdt ) {
	tuner::ID pid = 20;
	tuner::ID programID = 5000;
	tuner::ServiceID id(1, 1, programID);
	tuner::Service ser( 8, id, pid );
	util::BYTE tagsToCheck[] = { 0xFC, 0xFA };

	{
		boost::shared_ptr<tuner::Sdt> sdt( createSdt(programID) );
		ASSERT_FALSE( ser.process( sdt ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::sdt), tagsToCheck) );
	}

	ser.expire( tuner::service::desc::sdt );

	{
		boost::shared_ptr<tuner::Sdt> sdt( createSdt(programID) );
		ASSERT_FALSE( ser.process( sdt ) );
		ASSERT_TRUE( CHECK_TAGS( ser.descriptors(tuner::service::desc::sdt), tagsToCheck) );
	}
}

