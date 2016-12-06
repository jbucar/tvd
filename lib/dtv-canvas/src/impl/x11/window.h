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

#include "../../window.h"

typedef unsigned long XID;
typedef XID Window;
typedef XID Drawable;
typedef Drawable XdbeBackBuffer;
typedef struct _XDisplay Display;
typedef union _XEvent XEvent;

namespace canvas {
namespace x11 {

class Render;

class Window : public canvas::Window {
public:
	Window( Display *display );
	virtual ~Window();

	//	Size methods
	virtual bool supportFullScreen() const;

	//	Layer methods
	virtual Surface *lockLayer();

	//	Overlays
	virtual bool supportVideoOverlay() const;

	//	Embedded
	virtual bool supportEmbedded() const;

	//	Video methods
	virtual int getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines );
	virtual void cleanup();
	virtual void *allocFrame( void **pixels );
	virtual void freeFrame( void *frame );
	virtual void renderFrame( void * );

	//	Aux event
	void onEvent( XEvent ev );

	//	Aux
	Display *dpy() const;
	::Window win() const;
	const Rect &getVideoBounds() const;
	Size canvasSize();

	//	Aux x11 extensions initialization
	bool initXCompositeExtension();
	bool initXDamageExtension();

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();

	//	Aux layer
	virtual bool initLayer( canvas::Canvas *canvas );
	virtual void finLayer( canvas::Canvas *canvas );
	virtual void renderLayerImpl( Surface *surface, const std::vector<Rect> &dirtyRegions );

	//	Aux size methods
	virtual void fullScreenImpl( bool enable );
	virtual void iconifyImpl( bool enable );
	virtual void setSizeImpl( const Size &size );
	virtual Size screenSize() const;
	void sizeHints( const Size &min, const Size &max );

	//	Aux
	Render *createRender();
	virtual VideoOverlay *createOverlayInstance( int zIndex ) const;

private:
	Display *_display;
	Render *_render;
	::Window _window;
	int _xDamageEventBase;
	bool _useVideoOverlay;
	bool _usePixmap;
};

}
}

