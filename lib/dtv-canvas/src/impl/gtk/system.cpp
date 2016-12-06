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

#include "system.h"
#include "dispatcher.h"
#include "window.h"
#if CANVAS_HTML_USE_GTK
#include "webviewer.h"
#endif
#include "videooverlay.h"
#include "../cairo/canvas.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

namespace canvas {
namespace gtk {

namespace impl {

static System *sys = NULL;

System *getSystem() {
	DTV_ASSERT(impl::sys);
	return impl::sys;
}

}	//	namespace impl


System::System()
{
	impl::sys = this;
}

System::~System()
{
	impl::sys = NULL;
}

//	Instance creation
util::io::Dispatcher *System::createIO() const {
	return new Dispatcher();
}

canvas::WebViewer *System::createWebViewer( Surface *surface ) {
#if CANVAS_HTML_USE_GTK
	return new WebViewer( (canvas::System *)this, surface );
#else
	return canvas::System::createWebViewer( surface );
#endif
}

canvas::Canvas *System::createCanvas() const {
	return new cairo::Canvas();
}

canvas::Window *System::createWindow() const {
	return new Window();
}
	 
void System::onWindowDestroyed() {
	dispatchShutdown();
}

}
}
