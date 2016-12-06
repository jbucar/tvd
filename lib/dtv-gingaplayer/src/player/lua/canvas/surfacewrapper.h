/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <canvas/size.h>

namespace canvas {
	class Surface;
}

namespace player {
namespace mcanvas {

class SurfaceWrapper {
public:
	SurfaceWrapper( canvas::Surface *surface, bool isPrimary );
	virtual ~SurfaceWrapper();

	canvas::Surface *surface();

	void setFlip( bool horizontal, bool vertical );
	bool isFlippedH() const;
	bool isFlippedV() const;

	void setScaledSize( const canvas::Size &size );
	canvas::Size getScaledSize() const;

	void replaceSurface( canvas::Surface *newSurface );

	bool needScale() const;

	void rotation( int degrees );
	int rotation() const;

	bool isPrimary() const;

private:
	canvas::Surface *_surface;
	bool _flipH;
	bool _flipV;
	canvas::Size _scaledSize;
	int _rotation;
	bool _isPrimary;

	SurfaceWrapper() {}
};

typedef SurfaceWrapper * SurfaceWrapperPtr;

}
}
