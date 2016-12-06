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
#include <gtest/gtest.h>

class TunerTest : public testing::Test {
public:
	virtual void SetUp() {
		_tuner = createTuner();
		ASSERT_TRUE(_tuner);
		ASSERT_TRUE(_tuner->initialize(&_tunerDelegate) );
	}

	virtual void TearDown() {
		_tuner->finalize();
		delete _tuner;
	}

	virtual pvr::Tuner *createTuner() {
		return new BasicTuner;
	}

	pvr::Tuner *_tuner;
	BasicTunerDelegateImpl _tunerDelegate;
};

template<typename T>
class TunerTestImpl : public TunerTest {
protected:
	virtual pvr::Tuner *createTuner() {
		return new T();
	}
};


TEST( Tuner, default_constructor ) {
	BasicTuner tuner;
	ASSERT_EQ( tuner.name(), "prueba" );
	ASSERT_FALSE( tuner.isInitialized() );
}

TEST( Tuner, init_ok ) {
	BasicTuner tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_TRUE( tuner.initialize(&delegate) );
	ASSERT_TRUE( tuner.isInitialized() );
	tuner.finalize();
}

TEST( Tuner, init_fail ) {
	TunerInitFail tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_FALSE( tuner.initialize(&delegate) );
	ASSERT_FALSE( tuner.isInitialized() );
	tuner.finalize();
}

TEST( Tuner, status_with_initialize ) {
	BasicTuner tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_TRUE( tuner.initialize(&delegate) );
	int sig, quality;
	tuner.status( sig, quality );
	ASSERT_EQ( sig, 10 );
	ASSERT_EQ( quality, 10 );
	tuner.finalize();
}

TEST( Tuner, status_without_initialize ) {
	BasicTuner tuner;
	int sig, quality;
	tuner.status( sig, quality );
	ASSERT_EQ( sig, -1 );
	ASSERT_EQ( quality, -1 );
}

TEST( Tuner, status_fail ) {
	TunerStatusFail tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_TRUE( tuner.initialize(&delegate) );

	int sig, quality;
	tuner.status( sig, quality );
	ASSERT_EQ( sig, -1 );
	ASSERT_EQ( quality, -1 );

	tuner.finalize();
}

TEST( Tuner, findLogo_without_initialize ) {
	BasicTuner tuner;
	ASSERT_EQ( tuner.findLogo( NULL ), "" );
	tuner.finalize();
}

TEST( Tuner, findLogo_ok ) {
	pvr::logos::Service logos;
	logos.add( new BasicLogosProvider() );
	logos.start();

	util::Settings *settings = util::Settings::create("test", "memory");
	settings->initialize();
	pvr::parental::Session session(settings);

	pvr::Channels chs( new pvr::channels::Persister(), &session );
	BasicTuner tuner;
	BasicTunerDelegate delegate(&logos,&chs);
	ASSERT_TRUE( tuner.initialize(&delegate) );
	pvr::Channel *ch = (pvr::Channel *)10;
	ASSERT_EQ( tuner.findLogo( ch ), "pepe" );
	tuner.finalize();
	logos.stop();

	{
		settings->finalize();
		delete settings;
	}
}

TEST_F( TunerTest, alloc_free_player ) {
	pvr::TunerPlayer *p = _tuner->reservePlayer();
	ASSERT_TRUE(p ? true : false);
	_tuner->freePlayer(p);
}

TEST_F( TunerTest, alloc_free_alloc_player ) {
	pvr::TunerPlayer *p = _tuner->reservePlayer();
	ASSERT_TRUE(p ? true : false);
	_tuner->freePlayer(p);
	pvr::TunerPlayer *otro = _tuner->reservePlayer();
	ASSERT_EQ( otro, p );
	_tuner->freePlayer(otro);
}

TEST( Tuner, alloc_free_without_initialize ) {
	BasicTuner tuner;
	ASSERT_DEATH( tuner.reservePlayer(), "" );
	ASSERT_DEATH( tuner.freePlayer(NULL), "" );
}

class TunerMaxAlloc : public BasicTuner {
protected:
	virtual int maxPlayers() const { return 1; }
};

typedef TunerTestImpl<TunerMaxAlloc> TunerTestMaxAlloc;
TEST_F( TunerTestMaxAlloc, alloc_max ) {
	pvr::TunerPlayer *p = _tuner->reservePlayer();
	ASSERT_TRUE(p ? true : false);

	pvr::TunerPlayer *p1 = _tuner->reservePlayer();
	ASSERT_FALSE(p1 ? true : false);

	_tuner->freePlayer( p );

	p1 = _tuner->reservePlayer();
	ASSERT_EQ(p1,p);

	_tuner->freePlayer( p );
}

typedef TunerTestImpl<TunerCantReserve> TunerTestAllocFail;
TEST_F( TunerTestAllocFail, alloc_fail ) {
	pvr::TunerPlayer *p = _tuner->reservePlayer();
	ASSERT_FALSE(p ? true : false);
}

typedef TunerTestImpl<BasicTuner> TunerTestBasic;
TEST_F( TunerTestBasic, scan ) {
	ASSERT_FALSE( _tuner->isScanning() );
	_tuner->startScan();
	ASSERT_TRUE( _tuner->isScanning() );
	_tuner->cancelScan();
	ASSERT_FALSE( _tuner->isScanning() );
}

TEST_F( TunerTestBasic, scan_on_finalize ) {
	ASSERT_FALSE( _tuner->isScanning() );
	_tuner->startScan();
	ASSERT_TRUE( _tuner->isScanning() );
	_tuner->finalize();
	ASSERT_FALSE( _tuner->isScanning() );
}

