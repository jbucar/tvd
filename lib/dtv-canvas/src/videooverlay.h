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

#include "types.h"
#include "rect.h"
#include <string>

namespace canvas {

class Window;

namespace vos {	//	video overlay scale method

enum type {
	always,
	notFullScreen
};

}	//	namespace vos

/**
 * Representa una superficie utilizada para renderizar video.
 */
class VideoOverlay {
public:
	explicit VideoOverlay( int zIndex );
	virtual ~VideoOverlay();

	//	Initialization
	bool initialize( const Rect &rect );
	void finalize();

	bool move( const Point &point );
	bool resize( const Size &size );
	bool moveResize( const Rect &rect );
	void iconify( bool enable );
	void updateBounds();
	Rect windowBounds() const;
	void scale( vos::type method );

	void zIndex( int );
	int zIndex() const;

	virtual bool getDescription( VideoDescription &desc );

protected:
	virtual bool create()=0;
	virtual void destroy()=0;

	virtual void show()=0;
	virtual void hide()=0;

	//	Aux size bounds methods
	bool move();
	virtual bool moveImpl( const Point &point )=0;
	bool resize();
	virtual bool resizeImpl( const Size &size )=0;
	bool moveResize();
	virtual bool moveResizeImpl( const Rect &rect );

	virtual Window *win() const=0;

	//	Aux
	bool isFullScreen() const;
	bool scale() const;
	bool setup();

private:
	int _zIndex;
	Rect _rect;
	vos::type _scale;
};

}

