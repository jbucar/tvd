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

#include "../../rect.h"
#include "../../videooverlay.h"

typedef unsigned long XID;
typedef XID Pixmap;
typedef XID Window;
typedef XID Damage;
typedef struct _XDisplay Display;

namespace canvas {
namespace x11 {

class Window;

class VideoOverlay : public canvas::VideoOverlay {
public:
	VideoOverlay( Window *win, int zIndex );
	virtual ~VideoOverlay( void );

	virtual bool getDescription( VideoDescription &desc );

	virtual void draw( Pixmap dest );

protected:
	//	Instance creation
	virtual bool create();
	virtual bool createImpl( const Rect &rect );
	virtual void destroy();
	virtual void destroyImpl();

	//	Show
	virtual void show();
	virtual void hide();

	//	Pos/size
	virtual bool moveImpl( const Point &point );
	virtual bool resizeImpl( const Size &size );
	virtual bool moveResizeImpl( const Rect &rect );

	// Getters
	virtual canvas::Window *win() const;
	Display *dpy() const;
	::Window video() const;

private:
	Window *_main;
	Display *_display;
	::Window _video;
	Damage _damage;
};

}
}

