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

#include "surfaceproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include <util/assert.h>
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>

#define CALL(m,p)	\
	if (!m.empty()) { m( p ); }

namespace player {

/**
 * Constructor base.
 * @param canvas Instancia de la clase @b canvas::Canvas.
 */
SurfaceProperties::SurfaceProperties( canvas::Canvas *canvas )
	: _canvas(canvas), _bounds(0,0,-1,-1)
{
	_surface = NULL;
}

/**
 * Desctructor base.
 */
SurfaceProperties::~SurfaceProperties()
{
	DTV_ASSERT(!_surface);
}

/**
 * Setea el callback a llamar cuando cambia el tamaño/posición de la superficie de renderizado.
 * @param callback El callback a setear.
 */
void SurfaceProperties::onBoundsChanged( const OnBoundsChanged &callback ) {
	_onBoundsChanged = callback;
}

/**
 * Setea el callback a llamar cuando cambia el tamaño de la superficie de renderizado.
 * @param callback El callback a setear.
 */
void SurfaceProperties::onSizeChanged( const OnSizeChanged &callback ) {
	_onSizeChanged = callback;
}

/**
 * Setea el callback a llamar cuando cambia la posición de la superficie de renderizado.
 * @param callback El callback a setear.
 */
void SurfaceProperties::onPositionChanged( const OnPositionChanged &callback ) {
	_onPositionChanged = callback;
}

/**
 * @return El Canvas con el cual se creo el SurfaceProperties.
 */
canvas::Canvas *SurfaceProperties::canvas() {
	return _canvas;
}

/**
 * Crea la superficie de renderizado.
 * @return true si pudo crear la superficie correctamente, false en caso contrario.
 */
bool SurfaceProperties::createSurface() {
	canvas::Rect tmp(_bounds);
	calculateBounds(tmp);
	return createSurface( tmp );
}

/**
 * Destruye la superficie de renderizado.
 */
void SurfaceProperties::destroy() {
	canvas()->destroy( _surface );
}

bool SurfaceProperties::createSurface( const canvas::Rect &rect ) {
	LDEBUG("SurfaceProperties", "Creating surface: pos=(%d,%d), size=(%d,%d)", rect.x, rect.y, rect.w, rect.h);
	
	_surface = canvas()->createSurface( rect );
	if (_surface) {
		return true;
	}
	return false;
}

/**
 * @return La superficie de renderizado.
 */
canvas::Surface *SurfaceProperties::surface() const {
	DTV_ASSERT(_surface);
	return _surface;
}

/**
 * Registra las propiedades de renderizado en el player.
 * @param player El Player al cual se le agregarán las propiedades de renderizado.
 */
void SurfaceProperties::registerProperties( Player *player ) {
	{	//	Add bounds
		PropertyImpl<canvas::Rect> *prop=new PropertyImpl<canvas::Rect>( true, _bounds );
		prop->setCheck( boost::bind(&check::bounds,canvas(),_1) );
		prop->setApply(	boost::bind(&SurfaceProperties::applyBounds,this,prop) );
		player->addProperty( property::type::bounds, prop );
	}
}

void SurfaceProperties::applyBounds(PropertyImpl<canvas::Rect> *prop) {
	bool sizeChanged=false;
	bool posChanged=false;
	
	//	Get new bounds
	canvas::Rect newBounds(_bounds);
	calculateBounds(newBounds);
	
	LDEBUG("SurfaceProperties", "apply bounds: (%d,%d,%d,%d)", newBounds.x, newBounds.y, newBounds.w, newBounds.h);

	const canvas::Point newPoint( newBounds );
	{	//	Compare location
		const canvas::Point &curPoint=surface()->getLocation();
		if (newPoint != curPoint) {
			surface()->setLocation( newPoint );
			posChanged = true;
		}
	}

	const canvas::Size newSize( newBounds );
	{	//	Compare size
		const canvas::Size &curSize=surface()->getSize();
		if (newSize != curSize) {
			//	Resize surface
			surface()->resize( newSize );
			sizeChanged = true;
		}
	}

	//	Notify to dependents
	if (sizeChanged && posChanged) {
		CALL(_onBoundsChanged,newBounds);
	}
	else if (sizeChanged) {
		CALL(_onSizeChanged,newSize);
	}
	else if (posChanged) {
		CALL(_onPositionChanged,newPoint);
	}

	//	Refresh if necesary
	prop->setNeedResfresh(sizeChanged);
}

void SurfaceProperties::calculateBounds( canvas::Rect &/*bounds*/ ) {
}

const canvas::Rect &SurfaceProperties::bounds() const {
	return _bounds;
}

}
