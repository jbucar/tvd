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

#include "ppapi.h"
#include "../../window.h"

namespace pp {
	class Instance;
	class PaintManager;
}

namespace canvas {

namespace cairo {
	class Surface;
}

namespace ppapi {

class System;
class PaintClient;

class Window : public canvas::Window {
public:
	explicit Window( System *sys );
	virtual ~Window();

	//	Layer methods
	virtual canvas::Surface *lockLayer();
	virtual void unlockLayer( canvas::Surface *surface );

	System *sys() const;
	void onPaint( void *data, const Rect &rect );

protected:
	virtual bool initLayer( canvas::Canvas *canvas );
	virtual void finLayer( canvas::Canvas *canvas );
	virtual Size screenSize() const;
	virtual void setSizeImpl( const Size &size );
	virtual bool supportResize() const;
	virtual void renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> &dirtyRegions );
	virtual void iconifyImpl( bool enable );

private:
	System *_sys;
	PaintClient *_client;
	pp::PaintManager *_paintMgr;
	cairo::Surface *_surface;
};

}
}

