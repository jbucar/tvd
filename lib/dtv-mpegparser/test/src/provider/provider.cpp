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
#include "provider.h"
#include "../util.h"
#include "../../../src/provider/filter.h"
#include "../../../src/provider/provider.h"
#include "../../../src/demuxer/psi/patdemuxer.h"
#include <util/buffer.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


OkProvider::OkProvider( void )
{
	stopCount = 0;
	_myDisp = new util::task::DispatcherImpl();
	setDispatcher( _myDisp );
}

OkProvider::~OkProvider()
{
	delete _myDisp;
}

TEST( Provider, default_constructor ) {
	OkProvider prov;
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

//	Start/stop
TEST( Provider, start ) {
	OkProvider prov;
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.stopCount == 0 );

	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.stopCount == 0 );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	{	//	Check networks
		ASSERT_EQ( 62, prov.getNetworksCount() );
		std::string n = prov.getNetworkName(1);
		ASSERT_EQ( "183", n );
		ASSERT_EQ( 1, prov.findNetwork( n ) );
	}

	ASSERT_EQ( size_t(-1), prov.currentNetwork() );
	ASSERT_EQ( prov.isLocked(), false );

	prov.stop();
	ASSERT_TRUE( prov.stopCount == 0 );
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

//	get_network_count
TEST( Provider, get_networks_count_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.getNetworksCount(), "" );
}

//	get_network_name
TEST( Provider, get_networks_name_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.getNetworkName(1), "" );
}

//	find_network
TEST( Provider, find_network_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.findNetwork("183"), "" );
}

//	currentNetwork
TEST( Provider, current_network_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.currentNetwork(), "" );
}

//	isLocked
TEST( Provider, is_locked_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.isLocked(), "" );
}

//	set_network
TEST( Provider, set_network_without_start ) {
	OkProvider prov;
	ASSERT_DEATH( prov.setNetwork(1), "" );
}

TEST( Provider, set_network_ok ) {
	OkProvider prov;
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	ASSERT_EQ( prov.isLocked(), false );
	ASSERT_TRUE( prov.state() > tuner::impl::state::idle );
	ASSERT_TRUE( prov.stopCount == 0 );
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 1 );
}

TEST( Provider, set_network_and_lock_timeout ) {
	OkProvider prov;
	prov._myDisp->onPost( boost::bind(test::notify,test::flags::on_task) );
	int lockCount=0;
	int timeoutCount=0;
	prov.onLockChanged( boost::bind(test::notifyCounter,&lockCount) );
	prov.onLockTimeout( boost::bind(test::notifyCounter,&timeoutCount) );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );

	ASSERT_EQ( prov.isLocked(), false );

	//	Wait for task!
	test::wait( test::flags::on_task );
	prov._myDisp->runAll();

	//	Wait task running
	test::wait( test::flags::task_running );
	ASSERT_EQ( lockCount, 0 );
	ASSERT_EQ( timeoutCount, 1 );
	ASSERT_EQ( prov.isLocked(), false );

	prov.stop();
}

TEST( Provider, set_network_and_lock_ok ) {
	TunerProvider<LockTuner> prov;
	prov._myDisp->onPost( boost::bind(test::notify,test::flags::on_task) );
	int lockCount=0;
	int timeoutCount=0;
	prov.onLockChanged( boost::bind(test::notifyCounter,&lockCount) );
	prov.onLockTimeout( boost::bind(test::notifyCounter,&timeoutCount) );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );

	ASSERT_EQ( prov.isLocked(), false );

	//	Wait for task!
	test::wait( test::flags::on_task );
	prov._myDisp->runAll();

	//	Wait task running
	test::wait( test::flags::task_running );
	ASSERT_EQ( lockCount, 1 );
	ASSERT_EQ( timeoutCount, 0 );
	ASSERT_EQ( prov.isLocked(), true );

	prov.stop();
}

TEST( Provider, set_network_and_lock_repeat ) {
	TunerProvider<LockTuner> prov;
	prov._myDisp->onPost( boost::bind(test::notify,test::flags::on_task) );
	int lockCount=0;
	int timeoutCount=0;
	prov.onLockChanged( boost::bind(test::notifyCounter,&lockCount) );
	prov.onLockTimeout( boost::bind(test::notifyCounter,&timeoutCount) );
	ASSERT_TRUE( prov.start() );

	for (int i=0; i<10; i++) {
		ASSERT_TRUE( prov.setNetwork( i ) );
		ASSERT_EQ( prov.isLocked(), false );

		//	Wait for task!
		test::wait( test::flags::on_task );
		prov._myDisp->runAll();

		//	Wait task running
		test::wait( test::flags::task_running );
		ASSERT_EQ( timeoutCount, 0 );
		ASSERT_EQ( prov.isLocked(), true );
	}

	prov.stop();
}

TEST( Provider, set_network_twice_same ) {
	OkProvider prov;
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	ASSERT_TRUE( prov.stopCount == 0 );
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 1 );
}

TEST( Provider, set_network_twice ) {
	OkProvider prov;
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	ASSERT_TRUE( prov.stopCount == 0 );
	ASSERT_TRUE( prov.setNetwork( 2 ) );
	ASSERT_TRUE( prov.currentNetwork() == 2 );
	ASSERT_TRUE( prov.stopCount == 1 );
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 2 );
}

TEST( Provider, set_network_twice_with_explicit_stop ) {
	OkProvider prov;
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	ASSERT_TRUE( prov.stopCount == 0 );
	prov.stopCurrent();
	ASSERT_TRUE( prov.stopCount == 1 );
	ASSERT_TRUE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == 1 );
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 2 );
}

TEST( Provider, set_network_fail ) {
	StartNetworkFailProvider prov;
	ASSERT_TRUE( prov.start() );
	ASSERT_FALSE( prov.setNetwork( 1 ) );
	ASSERT_TRUE( prov.currentNetwork() == size_t(-1) );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 0 );
}

TEST( Provider, stop_current_when_idle ) {
	StartNetworkFailProvider prov;
	ASSERT_TRUE( prov.start() );
	prov.stopCurrent();
	prov.stop();
	ASSERT_TRUE( prov.stopCount == 0 );
}

//	Filters methods
TEST( Provider, start_section_filter_ok ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	tuner::PSIDemuxer *demux = new tuner::PATDemuxer( 0 );
	ASSERT_TRUE( prov.startFilter( demux ) );
	prov.stopFilter( 0 );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, start_section_filter_create_fail ) {
	CreateFailProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	tuner::PSIDemuxer *demux = new tuner::PATDemuxer( 0 );
	ASSERT_FALSE( prov.startFilter( demux ) );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, start_section_filter_check_max ) {
	MaxFilterProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	tuner::PSIDemuxer *demux = new tuner::PATDemuxer( 0 );
	ASSERT_TRUE( prov.startFilter( demux ) );

	tuner::PSIDemuxer *demux1 = new tuner::PATDemuxer( 1 );
	ASSERT_FALSE( prov.startFilter( demux1 ) );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, start_section_filter_check_same_pid ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	tuner::PSIDemuxer *demux = new tuner::PATDemuxer( 0 );
	ASSERT_TRUE( prov.startFilter( demux ) );

	tuner::PSIDemuxer *demux1 = new tuner::PATDemuxer( 0 );
	ASSERT_FALSE( prov.startFilter( demux1 ) );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, start_section_filter_init_fail ) {
	FilterProvider<InitFailFilter> prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	tuner::PSIDemuxer *demux = new tuner::PATDemuxer( 0 );
	ASSERT_FALSE( prov.startFilter( demux ) );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, start_section_filter_start_fail ) {
	FilterProvider<StartFailFilter> prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	ASSERT_FALSE( prov.startFilter( new tuner::PATDemuxer( 0 ) ) );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, probe_section_filter_when_idle ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	int count=0;
	CountTestDemuxer *demux=new CountTestDemuxer(0,count);
	ASSERT_TRUE( prov.startFilter( demux ) );

	prov.probeSection( 0, "pat_nit_ver0_cni1.sec" );
	prov.stopFilter( 0 );
	prov.stop();

	ASSERT_EQ( count, 0 );
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, probe_section_filter_ok ) {
	NotifyProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	ASSERT_TRUE( prov.setNetwork( 0 ) );
	ASSERT_TRUE( prov.state() > tuner::impl::state::idle );

	TestDemuxer *demux=new TestDemuxer(0);
	ASSERT_TRUE( prov.startFilter( demux ) );
	prov.probeSection( 0, "pat_nit_ver0_cni1.sec" );

	ASSERT_TRUE( demux->started == 1 );
	ASSERT_TRUE( demux->pushed == 0 );

	prov.stopFilter( 0 );
	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, enqueue_without_filter ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start( 10 ) );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	ASSERT_TRUE( prov.setNetwork( 0 ) );
	ASSERT_TRUE( prov.state() > tuner::impl::state::idle );

	prov.probeSection( 0, "pat_nit_ver0_cni1.sec" );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, double_stop_filter ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	ASSERT_TRUE( prov.setNetwork( 0 ) );
	ASSERT_TRUE( prov.state() > tuner::impl::state::idle );

	TestDemuxer *demux = new TestDemuxer( 0 );
	ASSERT_TRUE( prov.startFilter( demux ) );

	prov.stopFilter( 0 );
	prov.stopFilter( 0 );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}

TEST( Provider, stop_invalid_filter ) {
	OkProvider prov;

	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
	ASSERT_TRUE( prov.start() );
	ASSERT_TRUE( prov.state() == tuner::impl::state::idle );

	ASSERT_TRUE( prov.setNetwork( 0 ) );
	ASSERT_TRUE( prov.state() > tuner::impl::state::idle );

	prov.stopFilter( 0 );

	prov.stop();
	ASSERT_TRUE( prov.state() == tuner::impl::state::off );
}
