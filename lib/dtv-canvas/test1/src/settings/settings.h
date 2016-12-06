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

#pragma once

#include <util/settings/settings.h>
#include <util/string.h>
#include <util/mcr.h>
#include <util/log.h>
#include <util/assert.h>
#include <gtest/gtest.h>

class SettingsTest : public testing::Test {
public:
	SettingsTest() {
		_settings = NULL;
	}
	virtual ~SettingsTest() {}

protected:
	virtual bool init() {
		util::log::flush();
		_settings = util::Settings::create( "audio_settings_test", "memory" );
		DTV_ASSERT( _settings != NULL );
		return _settings->initialize();
	}
	virtual void fin() {
		util::log::flush();
		_settings->finalize();
		DEL(_settings);
	}
	virtual void SetUp() {
		DTV_ASSERT( init() );
	}
	virtual void TearDown() {
		if (_settings) {
			_settings->clear();
			fin();
		}
	}

	util::Settings *_settings;
};