/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "settings.h"
#include "../../../src/layer/layer.h"
#include "../../../src/settings/layer.h"
#include <util/settings/settings.h>
#include <util/assert.h>

class LayerSettingsTest : public SettingsTest {
public:
	LayerSettingsTest() {
		_layer = NULL;
		_layerSettings = NULL;
	}
protected:
	virtual bool init() {
		DTV_ASSERT( SettingsTest::init() );
		_layer = new canvas::Layer( 0 );
		_layer->initialize();
		_layerSettings = new canvas::settings::Layer( _settings, _layer );
		return _layerSettings != NULL;
	}
	virtual void fin() {
		SettingsTest::fin();
		DEL(_layerSettings);
		DEL(_layer);
	}

	const std::string keyName() const {
		return util::format( "layer.%d.transparency", _layer->id() );
	}

	canvas::Layer *_layer;
	canvas::settings::Layer *_layerSettings;
};

TEST_F( LayerSettingsTest, opacityChange ) {
	ASSERT_TRUE( _layer->opacity( 9 ) );
	int opacity;
	_settings->get( keyName(), opacity );
	ASSERT_EQ( opacity, 9);
}

TEST_F( LayerSettingsTest, opacityChange_invalid ) {
	ASSERT_TRUE( _layer->opacity( 9 ) );
	ASSERT_FALSE( _layer->opacity( 15 ) );
	int opacity;
	_settings->get( keyName(), opacity );
	ASSERT_EQ( opacity, 9);
}

TEST_F( LayerSettingsTest, onLoad ) {
	_settings->put( keyName(), 2 );
	_settings->commit();
	
	_layerSettings->load();
	
	ASSERT_EQ( _layer->opacity(), 2 );
}

TEST_F( LayerSettingsTest, onReset ) {
	_settings->put( keyName(), 2 );
	_settings->commit();
	
	_layerSettings->load();
	
	ASSERT_EQ( _layer->opacity(), 2 );
	
	_layerSettings->reset();
	
	ASSERT_EQ( _layer->opacity(), CANVAS_LAYER_DEFAULT_OPACITY );
}