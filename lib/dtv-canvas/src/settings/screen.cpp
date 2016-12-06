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
#include "../screen.h"
#include <util/settings/settings.h>
#include <util/assert.h>
#include <boost/lexical_cast.hpp>

#define SCREEN_ASPECT_KEY "screen.aspect"
#define SCREEN_CONN_KEY   "screen.resolution.conn"
#define SCREEN_MODE_KEY   "screen.resolution.mode"

namespace canvas {
namespace settings {

Screen::Screen( util::Settings *settings, canvas::Screen *screen )
	: util::SettingsDelegate(settings), _screen(screen)
{
	DTV_ASSERT(screen);
	_onAspectChanged = _screen->onAspectChanged().connect( boost::bind( &Screen::onAspectChanged, this, _1 ) );
	_onModeChanged = _screen->onModeChanged().connect( boost::bind( &Screen::onModeChanged, this, _1, _2 ) );
}

Screen::~Screen()
{
}

void Screen::load() {
	{	//	Mode
		int tmpConn = _screen->defaultConnector();
		settings()->get( SCREEN_CONN_KEY, tmpConn );
		connector::type conn = (connector::type)tmpConn;

		int mode = _screen->defaultMode( conn );
		settings()->get( SCREEN_MODE_KEY, mode );
		_screen->mode( conn, (mode::type)mode );
	}

	{	//	Aspect
		int aspect = _screen->defaultAspect();
		settings()->get( SCREEN_ASPECT_KEY, aspect );
		_screen->aspect( (aspect::type)aspect );
	}
}

void Screen::reset() {
	{	//	Video display resolution
		connector::type conn = _screen->defaultConnector();
		mode::type mode = _screen->defaultMode( conn );
		_screen->mode( conn, mode );
	}

	//	Aspect ratio
	_screen->aspect( _screen->defaultAspect() );
}

void Screen::onAspectChanged( aspect::type val ) {
	settings()->put( SCREEN_ASPECT_KEY, val );
	settings()->commit();
}

void Screen::onModeChanged( connector::type conn, mode::type val ) {
	settings()->put( SCREEN_CONN_KEY, conn );
	settings()->put( SCREEN_MODE_KEY, (int)val );
	settings()->commit();
}

}
}
