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

#include <canvas/point.h>
#include <canvas/size.h>
#include <canvas/rect.h>
#include <canvas/color.h>
#include <boost/function.hpp>
#include <string>

namespace canvas {
	class Canvas;
	class Surface;
}

namespace player {

class Player;

template<typename T> class PropertyImpl;

/**
 * La clase SurfaceProperties añade a un Player la capacidad de renderizado
 * y control de la superficie de renderizado.
 */
class SurfaceProperties {
public:
	SurfaceProperties( canvas::Canvas *canvas );
	virtual ~SurfaceProperties();

	bool createSurface();
	void destroy();
	void registerProperties( Player *player );

	canvas::Surface *surface() const;

	//	Events
	typedef boost::function<void (const canvas::Rect &)> OnBoundsChanged;
	void onBoundsChanged( const OnBoundsChanged &callback );
	
	typedef boost::function<void (const canvas::Size &)> OnSizeChanged;
	void onSizeChanged( const OnSizeChanged &callback );

	typedef boost::function<void (const canvas::Point &)> OnPositionChanged;
	void onPositionChanged( const OnPositionChanged &callback );

protected:
	//	Properties
	void applyBounds(PropertyImpl<canvas::Rect> *prop);
	void applyZIndex();

	canvas::Canvas *canvas();
	bool createSurface( const canvas::Rect &rect );

	virtual void calculateBounds( canvas::Rect &size );
	const canvas::Rect &bounds() const;

private:
	canvas::Canvas *_canvas;
	canvas::Surface *_surface;
	canvas::Rect _bounds;
	OnSizeChanged _onSizeChanged;
	OnPositionChanged _onPositionChanged;
	OnBoundsChanged _onBoundsChanged;
};

}
