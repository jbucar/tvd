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

#include "application.h"
#include "controller.h"
#include <mpegparser/service/extension/application/application.h>
#include <mpegparser/service/extension/application/extension.h>
#include <util/assert.h>

namespace zapper {
namespace middleware {
namespace ginga {

Application::Application( Controller *ctrl, tuner::app::Application *app )
	: middleware::Application( app->name() )
{
	DTV_ASSERT(app);
	DTV_ASSERT(ctrl);

	_app = app;
	_ctrl = ctrl;

	if (!app->icons().empty()) {
		icon( app->icons()[0] );
	}
}

Application::~Application()
{
}

bool Application::start() {
	_ctrl->extension()->start( _app->appID() );
	return true;
}

void Application::stop() {
	_ctrl->extension()->stop( _app->appID() );
}

bool Application::isRunning() const {
	return _app->isRunning();
}

bool Application::isAttachedToChannel() const {
	return _app->isBoundToService();
}

}
}
}

