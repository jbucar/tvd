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

typedef struct _GtkWidget GtkWidget;
typedef union  _GdkEvent GdkEvent;

namespace canvas {

namespace cairo {
	class Surface;
}

namespace gtk {

class Window : public canvas::Window {
public:
	Window();
	virtual ~Window();

	//	Size methods
	virtual bool supportFullScreen() const;

	//	Overlays
	virtual bool supportVideoOverlay() const;

	//	Layer methods
	virtual Surface *lockLayer();

	//	Aux
	void draw( GtkWidget *da, const Rect &rect );
	GtkWidget *widget() const;
	GtkWidget *layout() const;

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();

	//	Layer methods
	virtual bool initLayer( Canvas *canvas );
	virtual void finLayer( Canvas *canvas );
	virtual void renderLayerImpl( Surface *surface, const std::vector<Rect> &dirtyRegions );

	// 	Size
	virtual Size screenSize() const;
	virtual void setSizeImpl( const Size &size );
	virtual void fullScreenImpl( bool enable );
	virtual void iconifyImpl( bool enable );

	//	Overlays
	virtual canvas::VideoOverlay *createOverlayInstance( int zIndex ) const;

private:
	GtkWidget *_window;
	GtkWidget *_layout;
	cairo::Surface *_surface;
};

}
}
