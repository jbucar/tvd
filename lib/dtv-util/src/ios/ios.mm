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

#import <UIKit/UIKit.h>

#include "ios.h"
#include "customappdelegate.h"
#include "customview.h"
#include "../assert.h"
#include "../log.h"

namespace util {
namespace ios {

namespace impl {
	static CustomView *view = NULL;
	OnKeyCallback onKeyCallback;
}

void setView( void *view ) {
	impl::view = (CustomView*) view;
}

const char *internalDataPath() {
	return [[[NSBundle mainBundle] resourcePath] UTF8String];
}

void initLayer( util::DWORD *pixels, int w, int h ) {
	DTV_ASSERT( impl::view );
	[impl::view initLayer: pixels width: w height: h ];
}

void renderLayer() {
	[impl::view render];
}

void screenSize( int &width, int &height ) {
	CGRect screenRect = [[UIScreen mainScreen] applicationFrame];
	width = screenRect.size.width;
	height = screenRect.size.height;
}

void dispatchKey( util::key::type key, bool up ) {
	if (!impl::onKeyCallback.empty()) {
		impl::onKeyCallback( key, up );
	} else {
		LWARN("ios", "OnKeyCallback is empty.");
	}
}

void setInputCallback( const OnKeyCallback &onKeyCallback ) {
	impl::onKeyCallback = onKeyCallback;
}

}
}

int main( int argc, char *argv[] ) {
	@autoreleasepool {
		return UIApplicationMain(argc, argv, nil, NSStringFromClass([CustomAppDelegate class]));
	}
}
