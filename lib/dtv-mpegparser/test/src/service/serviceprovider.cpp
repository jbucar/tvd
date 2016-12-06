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
#include "../provider/provider.h"
#include "../../../src/service/serviceprovider.h"
#include "../../../src/demuxer/psi/patdemuxer.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/pmt.h"
#include "../../../src/resourcemanager.h"
#include <boost/foreach.hpp>


//	TODO: Actualizar una PMT, que se encuentra junto a otras PMT en un mismo PID!!!
//	TODO: Probar timeout de una PMT, que tiene compartido el pid con otras ....

class ServiceProviderTest : public testing::Test {
public:
	ServiceProviderTest() { _ex = NULL; }
	virtual ~ServiceProviderTest() {}

	virtual void SetUp() {
		//	Create provider
		ASSERT_TRUE( _prov.start() );

		//	Create service provider and check basic getters
		tuner::ResourceManager *resMgr = new tuner::ResourceManager( "/tmp/test", 100, 100, 100 );
		_ser = new tuner::ServiceProvider( &_prov, resMgr );
		ASSERT_TRUE( _ser->resMgr() == resMgr );
		ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
		ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );
		ASSERT_FALSE( _ser->isRunning() );

		_ex = new OkExtension();
	}

	virtual void TearDown() {
		_prov.stop();
		delete _ser;
	}

	void start() {
		_ser->attach( _ex );

		ASSERT_TRUE( _prov.setNetwork( 0 ) );

		//	Start service provider
		_ser->initialize();
		_ser->start();
		ASSERT_TRUE( _ser->isRunning() );
	}

	bool status( const ReadyStatus &v, int count=0 ) {
		bool result=false;

		if (v.size() == static_cast<size_t>(count*2)) {
			if (count > 0) {
				result=true;
				for (int i=0; i<count; i++) {
					if (v[i*2] != true || v[(i*2)+1] != false) {
						result=false;
						break;
					}
				}
			}
			else {
				result=true;
			}
		}

		return result;
	}

	bool status( const ServicesStatus &s, int services=0, int count=0 ) {
		bool result=false;

		if (s.size() == static_cast<size_t>(services)) {
			result=true;
			ServicesStatus::const_iterator it;
			for (it=s.begin(); it!=s.end(); ++it) {
				result=status( (*it).second, count );
				if (!result) {
					break;
				}
			}
		}
		return result;
	}

protected:
	OkExtension *_ex;
	NotifyProvider _prov;
	tuner::ServiceProvider *_ser;
};

TEST_F( ServiceProviderTest, basic ) {
}

TEST_F( ServiceProviderTest, initialize_finalize ) {
	_ser->attach( _ex );
	_ser->initialize();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, stop_without_start ) {
	_ser->stop();
}

TEST_F( ServiceProviderTest, start_basic ) {
	//	Start service provider
	start();
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Stop service provider
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, start_twice ) {
	//	Start service provider
	start();
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Try start again
	ASSERT_DEATH( _ser->start(), "" );
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Stop service provider
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, stop_twice ) {
	//	Start service provider
	start();
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Stop
	_ser->stop();
	_ser->stop();

	_ser->finalize();
}

TEST_F( ServiceProviderTest, ready_basic ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	ASSERT_TRUE( _ser->currentTS() == 0x73b );

	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _ser->currentNit() == 0x73b );

	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	ASSERT_FALSE( _ser->isRunning() );
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, detach_after_ready_basic ) {
	OkExtension *ext2 = new OkExtension();
	_ser->attach( ext2 );

	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	ASSERT_TRUE( _ser->currentTS() == 0x73b );

	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _ser->currentNit() == 0x73b );

	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Disable ext2
	_ser->detach( ext2 );

	//	Enable ext2
	_ser->attach( ext2 );

	//	Stop
	_ser->stop();
	_ser->finalize();

	ASSERT_FALSE( _ser->isRunning() );
	ASSERT_TRUE( _ser->currentTS() == INVALID_TS_ID );
	ASSERT_TRUE( _ser->currentNit() == NIT_ID_RESERVED );

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );

	//	Check extensions
	ASSERT_TRUE( status( ext2->_init, 2 ) );
	ASSERT_TRUE( status( ext2->_start, 2 ) );
	ASSERT_TRUE( status( ext2->_ready, 2 ) );
	ASSERT_TRUE( status( ext2->_srvReady ) );
	ASSERT_TRUE( status( ext2->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_services ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, timeout_pat_when_ready ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	_prov.probeTimeout( 0 );

	//	Re-send
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );


	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, timeout_nit_when_ready ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	_prov.probeTimeout( 0x10 );

	//	Re-send
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_services_other_order ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_services_other_order2 ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_some_services ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	//_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 4, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_without_nit ) {
	//	Start service provider
	start();

	//	Send PAT without NIT
	_prov.probeSection( 0, "pat_snit_ver0_cni1.sec" );
	_prov.probeTimeout( 0x10 );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	ASSERT_TRUE( _ser->currentTS() == 0x73b );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, ready_without_sdt ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, not_ready_pat_timeout ) {
	//	Start service provider
	start();
	_prov.probeTimeout( 0 );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, not_ready_nit_timeout ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeTimeout( 0x10 );
	// _prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 0 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 0, 0 ) );
}

TEST_F( ServiceProviderTest, not_ready_sdt_timeout ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeTimeout( 0x11 );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
}

TEST_F( ServiceProviderTest, not_ready_pmt_timeout ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeTimeout( 0x407 );
	//_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 4, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, expire_pat_without_running_services ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Send PAT updated
	_prov.probeSection( 0, "tc_cinco_pat2_cni0.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, update_pat_without_running_services ) {
	//	Start service provider
	start();

	//	Send PAT
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	//	Send PMTs
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send PAT updated
	_prov.probeSection( 0, "tc_cinco_pat2.sec" );
	//	Send NIT
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	//	Send SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, run_service_nothing ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, run_service_before_provider_ready ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, run_service_after_provider_ready ) {
	//	Start service provider
	start();

	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	//	Start service
	_ser->startService( 0xe760 );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
}

TEST_F( ServiceProviderTest, run_service_before_service_ready ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 1 ) );
	ASSERT_TRUE( _ex->_srvStart[0xe760].size() == 2 );
}

TEST_F( ServiceProviderTest, run_twice_service ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	_ser->stopService( 0xe760 );

	//	Update service so can run again if enqueued ...
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd12.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );

	ASSERT_TRUE( _ex->_srvReady.size() == 5 );
	ASSERT_TRUE( status( _ex->_srvReady[0xe760], 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe761], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe762], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe763], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe764], 1 ) );

	ASSERT_TRUE( status( _ex->_srvStart, 1, 1 ) );
	ASSERT_TRUE( _ex->_srvStart[0xe760].size() == 2 );
}

TEST_F( ServiceProviderTest, run_service_after_service_ready ) {
	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Start service
	_ser->startService( 0xe760 );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 1 ) );
	ASSERT_TRUE( _ex->_srvStart[0xe760].size() == 2 );
}

class StartExtension : public OkExtension {
public:
	StartExtension( tuner::ID srvID ) : _srvID(srvID) {}

	//	Service provider
	virtual void onServiceReady( tuner::Service *srv, bool ready ) {
		OkExtension::onServiceReady( srv, ready );
		if (ready && srv->id() == _srvID) {
			srvMgr()->startService( _srvID );
		}
	}

	tuner::ID _srvID;
};

TEST_F( ServiceProviderTest, run_service_before_ready_and_in_extension_on_ready ) {
	//	Start service provider
	start();

	StartExtension *ext = new StartExtension( 0xe760 );
	_ser->attach( ext );

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 1 ) );
	ASSERT_TRUE( _ex->_srvStart[0xe760].size() == 2 );
}

TEST_F( ServiceProviderTest, run_multiple_service_after_service_ready ) {
	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Start service
	_ser->startService( 0xe760 );
	_ser->startService( 0xe761 );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 2, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart[0xe760], 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart[0xe761], 1 ) );
}

TEST_F( ServiceProviderTest, update_pat_with_running_services ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of PAT
	_prov.probeSection( 0, "tc_cinco_pat2.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 2 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 2 ) );
	ASSERT_FALSE( _ex->_srvStart[0xe760].empty() );
}

TEST_F( ServiceProviderTest, update_nit_with_running_services ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of NIT
	_prov.probeSection( 0x10, "tc_cinco_nit2.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 2 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 2 ) );
	ASSERT_FALSE( _ex->_srvStart[0xe760].empty() );
}

TEST_F( ServiceProviderTest, update_sdt_with_running_services ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of SDT
	_prov.probeSection( 0x11, "tc_cinco_sdt2.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 2 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 2 ) );
	ASSERT_FALSE( _ex->_srvStart[0xe760].empty() );
}

TEST_F( ServiceProviderTest, update_sdt_and_nit_with_running_services ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of NIT and SDT
	_prov.probeSection( 0x10, "tc_cinco_nit2.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt2.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 3 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 3 ) );
	ASSERT_TRUE( status( _ex->_srvStart, 1, 3 ) );
	ASSERT_FALSE( _ex->_srvStart[0xe760].empty() );
}

TEST_F( ServiceProviderTest, update_service_without_running ) {
	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of PMT
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd12.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );

	ASSERT_TRUE( _ex->_srvReady.size() == 5 );
	ASSERT_TRUE( status( _ex->_srvReady[0xe760], 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe761], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe762], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe763], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe764], 1 ) );

	ASSERT_TRUE( status( _ex->_srvStart ) );
}

class CheckExpiredExtension : public OkExtension {
public:
	CheckExpiredExtension( tuner::ID srvID ) : _srvID(srvID), srvReady(NULL), result(true) {}
	virtual ~CheckExpiredExtension() {
		delete srvReady;
	}

	//	Service provider
	virtual void onServiceReady( tuner::Service *srv, bool ready ) {
		OkExtension::onServiceReady( srv, ready );
		if (srv->id() == _srvID) {
			if (ready) {
				delete srvReady;
				srvReady = new tuner::Service(*srv);
			}
			else {
				result &= srvReady->networkID() == srv->networkID();
				result &= srvReady->networkName() == srv->networkName();
				result &= srvReady->serviceID() == srv->serviceID();
				result &= srvReady->pid() == srv->pid();
				result &= srvReady->pcrPID() == srv->pcrPID();
				result &= srvReady->elements().size() == srv->elements().size();
				for (int t=tuner::service::desc::pmt; t< tuner::service::desc::last; t++) {
					result &= srvReady->descriptors( (tuner::service::desc::type)t ).length() == srv->descriptors( tuner::service::desc::type(t) ).length();
				}
				result &= srvReady->type() == srv->type();
				result &= srvReady->provider() == srv->provider();
				result &= srvReady->name() == srv->name();
				result &= srvReady->status() == srv->status();
				result &= srvReady->isScrambled() == srv->isScrambled();
				result &= srvReady->state() != srv->state();
			}
		}
	}

	tuner::ID _srvID;
	tuner::Service *srvReady;
	bool result;
};

TEST_F( ServiceProviderTest, update_service_running ) {
	CheckExpiredExtension *ext = new CheckExpiredExtension( 0xe760 );
	_ser->attach( ext );

	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of PMT
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd12.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );

	ASSERT_TRUE( _ex->_srvReady.size() == 5 );
	ASSERT_TRUE( status( _ex->_srvReady[0xe760], 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe761], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe762], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe763], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe764], 1 ) );

	ASSERT_TRUE( _ex->_srvStart.size() == 1 );
	ASSERT_TRUE( status( _ex->_srvStart[0xe760], 2 ) );

	//	Check no changes in service data
	ASSERT_TRUE( ext->result );
}


TEST_F( ServiceProviderTest, update_two_service_one_running ) {
	//	Start service provider
	start();

	//	Start service
	_ser->startService( 0xe760 );

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	//	Send a updated version of PMT
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd12.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd22.sec" );

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );

	ASSERT_TRUE( _ex->_srvReady.size() == 5 );
	ASSERT_TRUE( status( _ex->_srvReady[0xe760], 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe761], 2 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe762], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe763], 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady[0xe764], 1 ) );

	ASSERT_TRUE( _ex->_srvStart.size() == 1 );
	ASSERT_TRUE( status( _ex->_srvStart[0xe760], 2 ) );
}

namespace test_scan {

static std::vector<tuner::ID> services;
static int done=0;
boost::mutex mutex;
boost::condition_variable cWakeup;

static void processService( tuner::Service *srv ) {
	printf( "[test_scan] Scan service: serviceID=%04x\n", srv->id() );
	mutex.lock();
	ASSERT_TRUE( done == 0 );
	mutex.unlock();
	services.push_back( srv->id() );
}

static void endScan() {
	printf( "[test_scan] End scan: done=%d\n", done );
	mutex.lock();
	done++;
	mutex.unlock();
	cWakeup.notify_all();
}

static void wait_end_scan() {
	boost::unique_lock<boost::mutex> lock( mutex );
	while (!done) {
		cWakeup.wait( lock );
		printf( "[test_scan] Wait end scan: done=%d\n", done );
	}
	printf( "[test_scan] Wait end scan end\n" );
}

static void init( OkExtension *ext, tuner::ServiceProvider *ser ) {
	done=0;
	ext->notifyOnServiceReady( boost::bind(&test_scan::processService,_1) );
	ser->onEndScan( boost::bind(&test_scan::endScan) );
	services.clear();
}

}

TEST_F( ServiceProviderTest, scan ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 5 );
}

TEST_F( ServiceProviderTest, scan_other_order ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 5 );
}

TEST_F( ServiceProviderTest, scan_invalid_pat ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send invalid pat
	_prov.probeSection( 0, "tc_cinco_pmt_sd11.sec" );
	test::notify( test::flags::filter_process );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 0 ) );
	ASSERT_TRUE( test_scan::services.size() == 0 );
}

TEST_F( ServiceProviderTest, scan_pat_timeout ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Timeout PAT
	_prov.probeTimeout( 0 );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready    ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 0 );
}

TEST_F( ServiceProviderTest, scan_double_pat_timeout ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Timeout PAT
	_prov.probeTimeout( 0 );
	_prov.probeTimeout( 0 );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready    ) );
	ASSERT_TRUE( status( _ex->_srvReady ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 0 );
}

TEST_F( ServiceProviderTest, scan_nit_timeout ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeTimeout( 0x10 );
	//	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 5, 1 ) );
	ASSERT_TRUE( test_scan::services.size() == 5 );
}

TEST_F( ServiceProviderTest, scan_pmt0_timeout ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );
	_prov.probeTimeout( 0x407 );
	//_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 4, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 4 );
}

TEST_F( ServiceProviderTest, scan_two_pmt_timeout ) {
	test_scan::init( _ex, _ser );

	//	Start service provider
	start();

	//	Send tc_cinco -> ready && 5 services ready
	_prov.probeSection( 0, "tc_cinco_pat1.sec" );
	_prov.probeSection( 0x10, "tc_cinco_nit1.sec" );
	_prov.probeSection( 0x11, "tc_cinco_sdt1.sec" );

	_prov.probeSection( 0x407, "tc_cinco_pmt_sd11.sec" );
	_prov.probeTimeout( 0x408 );
	//_prov.probeSection( 0x408, "tc_cinco_pmt_sd21.sec" );
	_prov.probeSection( 0x409, "tc_cinco_pmt_sd31.sec" );
	_prov.probeTimeout( 0x40a );
	//_prov.probeSection( 0x40a, "tc_cinco_pmt_sd41.sec" );
	_prov.probeSection( 0x40b, "tc_cinco_pmt_sd51.sec" );

	test_scan::wait_end_scan();

	//	Wait to stop
	_ser->stop();
	_ser->finalize();

	//	Check extensions
	ASSERT_TRUE( test_scan::done == 1 );
	ASSERT_TRUE( status( _ex->_init, 1 ) );
	ASSERT_TRUE( status( _ex->_start, 1 ) );
	ASSERT_TRUE( status( _ex->_ready, 1 ) );
	ASSERT_TRUE( status( _ex->_srvReady, 3, 1 ) );
	ASSERT_TRUE( status( _ex->_srvStart ) );
	ASSERT_TRUE( test_scan::services.size() == 3 );
}

