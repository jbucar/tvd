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
#include "../../../src/screen.h"
#include "../../../src/settings/screen.h"
#include <util/settings/settings.h>
#include <util/assert.h>

class ScreenSettings : public canvas::Screen {
public:
	virtual canvas::aspect::type defaultAspect() const { return canvas::aspect::a16_9; }
	virtual std::vector<canvas::aspect::type> supportedAspects() const {
		std::vector<canvas::aspect::type> tmp;
		tmp.push_back( canvas::aspect::a4_3 );
		tmp.push_back( canvas::aspect::a16_9 );
		return tmp;
	}
};

class ScreenSettingsTest : public SettingsTest {
public:
	ScreenSettingsTest() {
		_screen = NULL;
		_screenSettings = NULL;
	}
protected:
	virtual bool init() {
		DTV_ASSERT( SettingsTest::init() );
		_screen = new ScreenSettings();
		_screen->initialize();
		_screenSettings = new canvas::settings::Screen( _settings, _screen );
		return _screenSettings != NULL;
	}
	virtual void fin() {
		SettingsTest::fin();
		DEL(_screenSettings);
		DEL(_screen);
	}

	ScreenSettings *_screen;
	canvas::settings::Screen *_screenSettings;
};

TEST_F( ScreenSettingsTest, modeChange ) {
	ASSERT_TRUE( _screen->mode( canvas::connector::hdmi, canvas::mode::m1080i_60 ) );
	int mode, connector;
	_settings->get( "screen.resolution.mode", mode );
	_settings->get( "screen.resolution.conn", connector );
	ASSERT_EQ( mode, canvas::mode::m1080i_60 );
	ASSERT_EQ( connector, canvas::connector::hdmi );
}

TEST_F( ScreenSettingsTest, modeChange_invalid ) {
	ASSERT_TRUE( _screen->mode( canvas::connector::hdmi, canvas::mode::m1080i_60 ) );
	ASSERT_FALSE( _screen->mode( canvas::connector::svideo, canvas::mode::m720p_60 ) );
	int mode, connector;
	_settings->get( "screen.resolution.mode", mode );
	_settings->get( "screen.resolution.conn", connector );
	ASSERT_EQ( mode, canvas::mode::m1080i_60 );
	ASSERT_EQ( connector, canvas::connector::hdmi );
}

TEST_F( ScreenSettingsTest, aspectChange ) {
	ASSERT_TRUE( _screen->aspect( canvas::aspect::a4_3 ) );
	int aspect;
	_settings->get( "screen.aspect", aspect );
	ASSERT_EQ( aspect, canvas::aspect::a4_3 );
}

TEST_F( ScreenSettingsTest, aspectChange_invalid ) {
	ASSERT_TRUE( _screen->aspect( canvas::aspect::a4_3 ) );
	ASSERT_FALSE( _screen->aspect( canvas::aspect::a14_9 ) );
	int aspect;
	_settings->get( "screen.aspect", aspect );
	ASSERT_EQ( aspect, canvas::aspect::a4_3 );
}

TEST_F( ScreenSettingsTest, onLoad ) {
	_settings->put( "screen.aspect", canvas::aspect::a4_3 );
	_settings->put( "screen.resolution.mode", canvas::mode::m1080i_60 );
	_settings->put( "screen.resolution.conn", canvas::connector::hdmi );
	
	_screenSettings->load();
	
	ASSERT_EQ( _screen->aspect(), canvas::aspect::a4_3 );
	ASSERT_EQ( _screen->connector(), canvas::connector::hdmi );
	ASSERT_EQ( _screen->mode(), canvas::mode::m1080i_60 );
}

TEST_F( ScreenSettingsTest, onReset ) {
	_settings->put( "screen.aspect", canvas::aspect::a4_3 );
	_settings->put( "screen.resolution.mode", canvas::mode::m1080i_60 );
	_settings->put( "screen.resolution.conn", canvas::connector::hdmi );
	
	_screenSettings->load();
	
	ASSERT_EQ( _screen->aspect(), canvas::aspect::a4_3 );
	ASSERT_EQ( _screen->connector(), canvas::connector::hdmi );
	ASSERT_EQ( _screen->mode(), canvas::mode::m1080i_60 );
	
	_screenSettings->reset();
	
	ASSERT_EQ( _screen->aspect(), _screen->defaultAspect() );
	ASSERT_EQ( _screen->connector(), _screen->defaultConnector() );
	ASSERT_EQ( _screen->mode(), _screen->defaultMode(  _screen->defaultConnector() ) );
}