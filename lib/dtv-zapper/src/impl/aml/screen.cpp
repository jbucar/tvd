/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "screen.h"
#include <aml/aml.h>
#include <util/log.h>
#include <util/assert.h>
#include <boost/foreach.hpp>

namespace aml {

// Mode methods
struct AMMode {
	const char *mode;
	canvas::mode::type canvasMode;
};

static struct AMMode _amModes[] = {
	{ "480p",  canvas::mode::m480p_60 },
	{ "480i",  canvas::mode::m480i_60 },
	{ "576p",  canvas::mode::m576p_50 },
	{ "576i",  canvas::mode::m576i_50 },
	{ "720p",  canvas::mode::m720p_60 },
	{ "1080p", canvas::mode::m1080p_60 },
	{ "1080i", canvas::mode::m1080i_50 },
	{ NULL,    canvas::mode::LAST_MODE }
};

//	Static to
static std::vector<canvas::mode::type> _modes;

static canvas::mode::type getMode( const std::string &mode ) {
	canvas::mode::type tmp = canvas::mode::unknown;
	int i=0;
	while (_amModes[i].mode) {
		size_t pos = mode.find( "*" );
		size_t len = (pos != std::string::npos) ? mode.length()-1 : mode.length();
		if (strlen(_amModes[i].mode) == len && !strncmp( mode.c_str(), _amModes[i].mode, len )) {
			tmp = _amModes[i].canvasMode;
			break;
		}
		i++;
	}
	return tmp;
}

static void refresh_caps() {
	_modes.clear();
	std::vector<std::string> caps;
	display::getModes( caps );
	BOOST_FOREACH( const std::string &str, caps ) {
		canvas::mode::type m = getMode( str );
		if (m != canvas::mode::unknown) {
			_modes.push_back( m );
		}
	}
#ifdef _DEBUG
	BOOST_FOREACH( canvas::mode::type m, _modes ) {
		canvas::Mode t = canvas::mode::get(m);
		LDEBUG( "aml", "\tmode: %d, %s", m, t.name );
	}
#endif
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

bool Screen::init() {
	refresh_caps();
	return true;
}

void Screen::fin() {
	_modes.clear();
}

// Connetors methods
canvas::connector::type Screen::defaultConnector() const {
	if (!_modes.empty()) {
		return canvas::connector::hdmi;
	}
	else {
		return canvas::connector::composite;
	}
}

std::vector<canvas::connector::type> Screen::supportedConnectors() const {
	std::vector<canvas::connector::type> tmp;
	tmp.push_back( canvas::connector::hdmi );
	tmp.push_back( canvas::connector::composite );
	return tmp;
}

// Video aspect
canvas::aspect::type Screen::defaultAspect() const {
	return canvas::aspect::automatic;
}

std::vector<canvas::aspect::type> Screen::supportedAspects() const {
	std::vector<canvas::aspect::type> tmp;
	tmp.push_back( canvas::aspect::automatic );
	return tmp;
}

bool Screen::aspectImpl( canvas::aspect::type /*vAspect*/ ) {
	return true;
}

canvas::mode::type Screen::defaultMode( canvas::connector::type conn ) const {
	if (conn == canvas::connector::hdmi) {
		return canvas::mode::m720p_60;
	}
	else {
		return canvas::mode::m576i_50;
	}
}

std::vector<canvas::mode::type> Screen::supportedModes( canvas::connector::type conn ) const {
	if (conn == canvas::connector::hdmi) {
		refresh_caps();
		return _modes;
	}
	else {
		std::vector<canvas::mode::type> tmp;
		tmp.push_back( canvas::mode::m576i_50 );
		tmp.push_back( canvas::mode::mNTSC );
		return tmp;
	}
}

bool Screen::modeImpl( canvas::connector::type /*conn*/, canvas::mode::type vMode ) {
	std::string mode;
	switch (vMode) {
		case canvas::mode::m480p_60: {
			mode = "480p";
			break;
		}
		case canvas::mode::m480i_60: {
			mode = "480i";
			break;
		}
		case canvas::mode::m576p_50: {
			mode = "576p";
			break;
		}
		case canvas::mode::m576i_50: {    // 720x576-50i
			mode = "576i";
			break;
		}
		case canvas::mode::m720p_60: {    // 1280x720-60
			mode = "720p";
			break;
		}
		case canvas::mode::m1080i_50: {    // 1920x1080-50i
			mode = "1080i";
			break;
		}
		case canvas::mode::m1080p_60: {    // 1920x1080-60
			mode = "1080p";
			break;
		}
		default: {
			mode = "576i";
		}
	};

	LDEBUG("aml", "Set tvout display mode: vMode=%d, mode=%s", vMode, mode.c_str() );
	if (!display::setMode( mode )) {
		LERROR( "aml", "Cannot set display mode: mode=%s", mode.c_str() );
		return false;
	}

	return true;
}

}
