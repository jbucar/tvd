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

#pragma once

#include "../../types.h"
#include "../../rect.h"
#include <vector>

typedef unsigned long XID;
typedef XID Window;
typedef XID Pixmap;
typedef struct _XDisplay Display;

namespace canvas {
namespace x11 {

class Window;

class Render {
public:
	explicit Render( Window *win );
	virtual ~Render();

	//	Initialization
	bool initialize();
	void finalize();

	//	Layer methods
	virtual bool initLayer( Canvas *canvas )=0;
	virtual void finLayer( Canvas *canvas )=0;	
	virtual Surface *lockLayer()=0;
	virtual void blit( Surface *surface, const std::vector<Rect> &dirtyRegions, Pixmap dest )=0;

	//	Overlay methods
	virtual bool supportVideoOverlay() const;

	//	Video methods
	void restartRender();
	virtual int getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines );
	virtual void cleanup();
	virtual void *allocFrame( void **pixels );
	virtual void freeFrame( void *frame );
	virtual void renderFrame( void * );

	virtual void *visualInfo( Display *dpy );

protected:
	//	Aux initialization
	virtual bool init();
	virtual void fin();

	//	Aux render initialization
	virtual bool initRender();
	virtual void finRender();

	//	Getters
	Window *window() const;
	Display *display() const;
	::Window win() const;
	const Size &size() const;
	const Rect &videoBounds() const;

	bool isFullScreen() const;
	Size canvasSize();

private:
	Window *_window;
	bool _renderInitialized;
	
	Render() {}
};

}
}

