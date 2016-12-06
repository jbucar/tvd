/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "types.h"
#include "../../src/pvr.h"
#include "../../src/time.h"
#include <util/string.h>
#include <gtest/gtest.h>

class PvrTest : public testing::Test {
public:
	PvrTest() {
		_settings = NULL;
		_pvr = NULL;
	}
	virtual ~PvrTest() {}

	virtual void SetUp() {
		_settings = util::Settings::create("test", "memory");
		ASSERT_TRUE( _settings->initialize() );
		_settings->clear();

		pvr::time::init( new pvr::Time(0,"ARG") );

		_mustDeath = false;
		_pvr = new pvr::Pvr(_settings);

		_pvr->onScanChanged().connect( boost::bind(&PvrTest::onChanged,this,_1) );
	}

	virtual void TearDown() {
		if (_mustDeath) {
			ASSERT_DEATH( delete _pvr, "" );
		}
		else {
			delete _pvr;
		}

		_settings->finalize();
		delete _settings;
	}

	void onChanged(pvr::scan::Type st ) {
		_scan.push_back( st );
	}

	bool _mustDeath;
	pvr::Pvr *_pvr;
	util::Settings *_settings;
	std::vector<pvr::scan::Type> _scan;
};

class PvrTestOk : public PvrTest {
public:
	virtual void SetUp() {
		PvrTest::SetUp();
		_pvr->addTuner( createTuner() );
		ASSERT_TRUE( _pvr->initialize() );
	}

	virtual void TearDown() {
		_pvr->finalize();
		PvrTest::TearDown();
	}

	virtual pvr::Tuner *createTuner() {
		return new BasicTuner();
	}
};

template<typename T>
class PvrTestOkImpl : public PvrTestOk {
public:
	pvr::Tuner *createTuner() {
		_tuner = new T();
		return _tuner;
	}

	T *_tuner;
};

TEST_F( PvrTest, constructor ) {
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_DEATH( _pvr->startScan(), "" );
	ASSERT_DEATH( _pvr->cancelScan(), "" );
	ASSERT_DEATH( _pvr->resetConfig(), "" );
	ASSERT_DEATH( _pvr->allocPlayer(), "" );
	{
		pvr::Player *p = (pvr::Player *)0x100;
		ASSERT_DEATH( _pvr->freePlayer(p), "" );
	}
	ASSERT_TRUE( _pvr->channels() != NULL );
	ASSERT_TRUE( _pvr->session() != NULL );
	ASSERT_TRUE( _pvr->settings() != NULL );
}

TEST_F( PvrTest, initialize ) {
	ASSERT_TRUE( _pvr->initialize() );
	ASSERT_DEATH( _pvr->initialize(), "" );
	_pvr->finalize();
	_pvr->finalize();
}

TEST_F( PvrTest, reset_logos ) {
	_pvr->logos()->add( new BasicLogosProvider() );
	_pvr->initialize();
	ASSERT_TRUE( _pvr->logos()->isEnabled("dummy") ); // Enabled by default
	_pvr->logos()->enableAll(false);
	_pvr->resetConfig();
	ASSERT_TRUE( _pvr->logos()->isEnabled("dummy") );
	_pvr->finalize();
}

TEST_F( PvrTest, tuner_init_fail ) {
	_pvr->addTuner( new TunerInitFail() );
	ASSERT_FALSE( _pvr->initialize() );
}


TEST_F( PvrTest, add_tuner ) {
	_mustDeath = true;
	_pvr->addTuner( new BasicTuner() );
}

TEST_F( PvrTestOk, add_tuner ) {
	ASSERT_DEATH( _pvr->addTuner( new BasicTuner() ), "" );
}

TEST_F( PvrTest, scan_without_tuner ) {
	ASSERT_TRUE( _scan.empty() );
	_pvr->initialize();
	ASSERT_FALSE( _pvr->isScanning() );
	_pvr->startScan();
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 2 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	ASSERT_EQ( _scan[1], pvr::scan::end );
	_pvr->finalize();
}

TEST_F( PvrTestOk, reset_channels ) {
	_pvr->channels()->add(new pvr::Channel("name", "10.01", "net1"));
	_pvr->channels()->add(new pvr::Channel("name2", "10.02", "net2"));
	_pvr->resetConfig();
	ASSERT_EQ( _pvr->channels()->getAll().size(), 0 );
}

TEST_F( PvrTestOk, reset_session ) {
	_pvr->session()->timeExpiration(10000);
	_pvr->resetConfig();
	ASSERT_EQ( _pvr->session()->timeExpiration(), -1 );
}

TEST_F( PvrTestOk, reset_players ) {
	pvr::Player *player = _pvr->allocPlayer();
	player->start();
	player->showMobile(true);

	pvr::Player *player2 = _pvr->allocPlayer();
	player2->start();
	player2->showMobile(true);

	_pvr->resetConfig();

	ASSERT_FALSE(player->showMobile());
	ASSERT_FALSE(player2->showMobile());
}

TEST_F( PvrTestOk, free_player ) {
	pvr::Player *p = _pvr->allocPlayer();
	_pvr->freePlayer(p);
	ASSERT_TRUE( p == NULL );
}

TEST_F( PvrTestOk, free_player_not_alloc ) {
	pvr::Player *p = (pvr::Player *) 0x100;
	_pvr->freePlayer(p);
	ASSERT_TRUE( p == (pvr::Player *) 0x100 );
}

TEST_F( PvrTestOk, alloc_player_return_player ) {
	pvr::Player *p = _pvr->allocPlayer();
	ASSERT_FALSE(p == NULL);
	p->start();
}

class ScanTuner : public BasicTuner {
public:
	void endScan( int nets=0 ) {
		for (int i=0; i<nets; i++) {
			std::string n = util::format( "net%d", i );
			onStartNetworkScan( n );
		}
		cancelScan();
	}
};

typedef PvrTestOkImpl<ScanTuner> PvrTestScan;
TEST_F( PvrTestScan, scan_basic ) {
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_TRUE( _scan.empty() );
	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 1 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	_tuner->endScan();
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 2 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	ASSERT_EQ( _scan[1], pvr::scan::end );
}

TEST_F( PvrTestScan, scan_networks ) {
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_TRUE( _scan.empty() );

	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 1 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );

	_tuner->endScan(2);

	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 4 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	ASSERT_EQ( _scan[1], pvr::scan::network );
	ASSERT_EQ( _scan[2], pvr::scan::network );
	ASSERT_EQ( _scan[3], pvr::scan::end );
}

TEST_F( PvrTestScan, scan_cancel ) {
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_TRUE( _scan.empty() );
	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 1 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );

	_pvr->cancelScan();

	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 2 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	ASSERT_EQ( _scan[1], pvr::scan::end );
}

TEST_F( PvrTestScan, scan_twice ) {
	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_TRUE( _scan.empty() );

	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 1 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );

	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 1 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );

	_tuner->endScan();

	ASSERT_FALSE( _pvr->isScanning() );
	ASSERT_EQ( _scan.size(), 2 );
	ASSERT_EQ( _scan[0], pvr::scan::begin );
	ASSERT_EQ( _scan[1], pvr::scan::end );
}

TEST_F( PvrTestScan, reset_while_scanning ) {
	_pvr->startScan();
	ASSERT_TRUE( _pvr->isScanning() );
	_pvr->resetConfig();
	ASSERT_FALSE( _pvr->isScanning() );
}
