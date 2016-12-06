/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "screen.h"
#include "../../src/types.h"
#include "../../src/system.h"
#include "../../src/screen.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/signals2.hpp>

ScreenTest::ScreenTest()
{
	_scr = NULL;
}

ScreenTest::~ScreenTest()
{
	DTV_ASSERT(!_scr);
}

bool ScreenTest::init() {
	util::log::flush();
	_scr = createScreen();
	return _scr != NULL;
}

void ScreenTest::fin() {
	DEL(_scr);
	util::log::flush();
}

canvas::Screen *ScreenTest::createScreen() {
	return canvas::Screen::create();
}

void ScreenTest::SetUp() {
	init();
	ASSERT_TRUE( _scr->initialize() );
}

void ScreenTest::TearDown() {
	_scr->finalize();
	fin();
}


TEST_F( ScreenFailInitializationTest, initialize ) {
	ASSERT_FALSE( _scr->isOn() );
}

// turnOn
TEST_F( ScreenOkTest, turnon_ok ) {
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_TRUE( _scr->isOn() );
}

TEST_F( ScreenOkTest, double_turnon ) {
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_TRUE( _scr->isOn() );
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_TRUE( _scr->isOn() );
}

TEST_F( ScreenFailTurnOnTest, turnon_fail ) {
	ASSERT_FALSE( _scr->turnOn() );
	ASSERT_FALSE( _scr->isOn() );
}

// turnOff
TEST_F( ScreenOkTest, turnoff_ok ) {
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_TRUE( _scr->turnOff() );
	ASSERT_FALSE( _scr->isOn() );
}

TEST_F( ScreenOkTest, turnoff_double ) {
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_TRUE( _scr->turnOff() );
	ASSERT_FALSE( _scr->isOn() );
	ASSERT_TRUE( _scr->turnOff() );
	ASSERT_FALSE( _scr->isOn() );
}

TEST_F( ScreenFailTurnOffTest, turnoff_fail ) {
	ASSERT_TRUE( _scr->turnOn() );
	ASSERT_FALSE( _scr->turnOff() );
	ASSERT_TRUE( _scr->isOn() );
}

//Aspect
//-------------------------------------------------------------------------------
TEST_F( ScreenOkTest, base_default_aspect ) {
	ASSERT_EQ( _scr->defaultAspect(), _scr->aspect() );
}

TEST_F( ScreenOkTest, set_aspect ) {
	canvas::aspect::type newAspect = canvas::aspect::LAST_ASPECT;
	_scr->onAspectChanged().connect( boost::bind( &ScreenOkTest::onAspectChanged, this, &newAspect, _1 ) );
	ASSERT_EQ( _scr->aspect(), canvas::aspect::a16_9 );
	ASSERT_TRUE( _scr->aspect( canvas::aspect::a4_3 ) );
	ASSERT_EQ( _scr->aspect(), canvas::aspect::a4_3 );
	ASSERT_EQ( newAspect, _scr->aspect() );
}

//fail signal second time
TEST_F( ScreenOkTest, doble_set_aspect ) {
	ASSERT_TRUE( _scr->aspect( canvas::aspect::a4_3 ) );
	canvas::aspect::type newAspect = canvas::aspect::LAST_ASPECT;
	_scr->onAspectChanged().connect( boost::bind( &ScreenOkTest::onAspectChanged, this, &newAspect, _1 ) );
	ASSERT_TRUE( _scr->aspect( canvas::aspect::a4_3 ) );
	ASSERT_EQ( newAspect, canvas::aspect::LAST_ASPECT );
}

//screenFailSetAspectTest
TEST_F( ScreenFailSetTest, set_aspect_fail ) {
	canvas::aspect::type newAspect = canvas::aspect::LAST_ASPECT;
	canvas::aspect::type currentAspect = _scr->aspect();
	_scr->onAspectChanged().connect( boost::bind( &ScreenOkTest::onAspectChanged, this, &newAspect, _1 ) );
	ASSERT_FALSE( _scr->aspect( canvas::aspect::a4_3 ) );
	ASSERT_EQ( _scr->aspect(), currentAspect );
	ASSERT_EQ( newAspect, canvas::aspect::LAST_ASPECT );
}

//unsuported screen aspect
TEST_F( ScreenUnsuportedTest, set_unsupported_aspect ) {
	ASSERT_EQ( _scr->aspect(), _scr->defaultAspect() );
	ASSERT_FALSE( _scr->aspect( canvas::aspect::a4_3 ) );
	ASSERT_EQ( _scr->aspect(), _scr->defaultAspect() );
}
//Aspect
//-------------------------------------------------------------------------------

//Modes
//-------------------------------------------------------------------------------
TEST_F( ScreenOkTest, base_default_mode ) {
	ASSERT_EQ( _scr->defaultConnector(), _scr->connector() );
	ASSERT_EQ( _scr->defaultMode( _scr->defaultConnector()), _scr->mode() );
}

//setMode ok
TEST_F( ScreenOkTest, set_mode ) {
	canvas::mode::type newMode = canvas::mode::LAST_MODE;
	canvas::connector::type activeConnector = _scr->connector();
	_scr->onModeChanged().connect( boost::bind( &ScreenOkTest::onModeChanged, this, &newMode, _2 ) );
	ASSERT_EQ( _scr->mode(), canvas::mode::m576i_50 );
	ASSERT_TRUE( _scr->mode(_scr->connector(), canvas::mode::m1080i_60 ) );
	ASSERT_EQ( _scr->connector(), activeConnector );
	ASSERT_EQ( _scr->mode(), canvas::mode::m1080i_60 );
	ASSERT_EQ( newMode, _scr->mode() );
}

//setMode ok, other connector
TEST_F( ScreenOkTest, set_mode_other_connector ) {
	canvas::mode::type newMode = canvas::mode::LAST_MODE;
	canvas::connector::type newConnector = canvas::connector::hdmi;
	_scr->onModeChanged().connect( boost::bind( &ScreenOkTest::onModeChanged, this, &newMode, _2 ) );
	ASSERT_EQ( _scr->mode(), canvas::mode::m576i_50 );
	ASSERT_TRUE( _scr->mode( newConnector, canvas::mode::m1080i_60 ) );
	ASSERT_EQ( _scr->connector(), newConnector );
	ASSERT_EQ( _scr->mode(), canvas::mode::m1080i_60 );
	ASSERT_EQ( newMode, _scr->mode() );
}

//fail signal second time
TEST_F( ScreenOkTest, doble_set_mode ) {
	ASSERT_TRUE( _scr->mode( _scr->connector(), canvas::mode::m1080i_60 ) );
	canvas::mode::type newMode = canvas::mode::LAST_MODE;
	_scr->onModeChanged().connect( boost::bind( &ScreenOkTest::onModeChanged, this, &newMode, _2 ) );
	ASSERT_TRUE( _scr->mode( _scr->connector(), canvas::mode::m1080i_60 ) );
	ASSERT_EQ( newMode, canvas::mode::LAST_MODE );
}

//screenFailSetMode
TEST_F( ScreenFailSetTest, set_mode_fail ) {
	canvas::mode::type newMode = canvas::mode::LAST_MODE;
	canvas::mode::type currentMode = _scr->mode();
	canvas::connector::type activeConnector = _scr->connector();
	_scr->onModeChanged().connect( boost::bind( &ScreenOkTest::onModeChanged, this, &newMode, _2 ) );
	ASSERT_FALSE( _scr->mode(_scr->connector(), canvas::mode::m576i_50 ) );
	ASSERT_EQ( _scr->connector(), activeConnector );
	ASSERT_EQ( _scr->mode(), currentMode );
	ASSERT_EQ( newMode, canvas::mode::LAST_MODE );
}

// //unsuported screen mode
TEST_F( ScreenUnsuportedTest, set_unsupported_mode ) {
	ASSERT_EQ( _scr->mode(), _scr->defaultMode( _scr->connector() ) );
	ASSERT_FALSE( _scr->mode( _scr->connector(), canvas::mode::m1080i_60 ) );
	ASSERT_EQ( _scr->mode(), _scr->defaultMode( _scr->connector() ) );
}

//Modes
//-------------------------------------------------------------------------------
