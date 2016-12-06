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

#include "layer.h"
#include "../../src/layer/layer.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/signals2.hpp>

LayerTest::LayerTest()
{
	_layer = NULL;
	_nOpacitySignal = 0;
	_nEnableSignal = 0;
}

LayerTest::~LayerTest()
{
	DTV_ASSERT(!_layer);
}

bool LayerTest::init() {
	util::log::flush();
	_layer = new canvas::Layer( 0 );
	return _layer != NULL;
}

void LayerTest::fin() {
	DEL(_layer);
	util::log::flush();
}

void LayerTest::SetUp() {
	init();
	_onOpacityChanged= _layer->onOpacityChanged().connect( boost::bind( &LayerTest::onOpacityChanged, this, _1 ) );
	_onEnableChanged= _layer->onEnableChanged().connect( boost::bind( &LayerTest::onEnableChanged, this, _1 ) );
}

void LayerTest::TearDown() {
	_layer->finalize();
	_onOpacityChanged.disconnect();
	_onEnableChanged.disconnect();
	fin();
}

TEST_F( LayerTest, initialize ) {
	ASSERT_TRUE( _layer->initialize() );
}

TEST_F( LayerTest, opacity ) {
	ASSERT_TRUE( _layer->opacity( 9 ) );
	ASSERT_EQ( _nOpacitySignal, 1 );
}

TEST_F( LayerTest, opacity_max ) {
	ASSERT_TRUE( _layer->opacity( CANVAS_LAYER_MAX_OPACITY ) );
	ASSERT_EQ( _nOpacitySignal, 1 );
}

TEST_F( LayerTest, opacity_min ) {
	ASSERT_TRUE( _layer->opacity( CANVAS_LAYER_MIN_OPACITY ) );
	ASSERT_EQ( _nOpacitySignal, 1 );
}

TEST_F( LayerTest, opacity_multiple ) {
	ASSERT_TRUE( _layer->opacity( CANVAS_LAYER_MIN_OPACITY ) );
	ASSERT_TRUE( _layer->opacity( 9 ) );
	ASSERT_EQ( _nOpacitySignal, 2 );
}

TEST_F( LayerTest, opacity_twice ) {
	ASSERT_TRUE( _layer->opacity( 9 ) );
	ASSERT_FALSE( _layer->opacity( 9 ) );
	ASSERT_EQ( _nOpacitySignal, 1 );
}

TEST_F( LayerTest, opacity_invalid ) {
	ASSERT_FALSE( _layer->opacity( CANVAS_LAYER_MAX_OPACITY * 2 ) );
	ASSERT_EQ( _nOpacitySignal, 0 );
}

TEST_F( LayerTest, opacity_invalid2 ) {
	ASSERT_FALSE( _layer->opacity( CANVAS_LAYER_MIN_OPACITY - 1 ) );
	ASSERT_EQ( _nOpacitySignal, 0 );
}

TEST_F( LayerTest, enable ) {
	_layer->enable( false );
	ASSERT_EQ( _nEnableSignal, 1 );
}

TEST_F( LayerTest, enable_twice ) {
	_layer->enable( false );
	_layer->enable( false );
	ASSERT_EQ( _nEnableSignal, 1 );
}

TEST_F( LayerTest, enable_multiple ) {
	_layer->enable( false );
	_layer->enable( true );
	ASSERT_EQ( _nEnableSignal, 2 );
}