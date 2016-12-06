/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "layer.h"
#include <util/log.h>
#include <boost/math/special_functions/round.hpp>

#define FIRST_OPACITY 155
#define INVALID_OPACITY -1

namespace canvas {

Layer::Layer( int id, int zIndex/*=0*/, bool enabled/*=true*/ )
{
	_id = id;
	_zIndex = zIndex;
	_opacity = INVALID_OPACITY;
	_enable = enabled;
}

Layer::~Layer()
{
}

int Layer::id() const {
	return _id;
}

int Layer::zIndex() const {
	return _zIndex;
}

bool Layer::initialize() {
	return true;
}

void Layer::finalize() {
}

mode::type Layer::defaultMode() const {
	return mode::m576p_50;
}

const std::vector<mode::type> Layer::supportedModes() const {
	std::vector<mode::type> tmp;
	tmp.push_back( mode::m576p_50 );	//	SD
	tmp.push_back( mode::m720p_60 );	//	HD
	tmp.push_back( mode::m1080p_60 );	//	Full-HD
	return tmp;
}

void Layer::mode( mode::type /*mode*/ ) {
}

void Layer::enable( bool state ) {
	if (state != _enable) {
		_enable = state;
		setEnable( state );
		_enableChanged( state );
	}
}

void Layer::setEnable( bool /*state*/ ) {
}

Layer::EnabledChanged &Layer::onEnableChanged() {
	return _enableChanged;
}

void Layer::clear() {
}

void Layer::flush() {
}

//	Color key
bool Layer::colorKey( util::BYTE /*r*/, util::BYTE /*g*/, util::BYTE /*b*/ ) {
	return true;
}

bool Layer::opacity( int value ) {
	bool res=false;
	if (_opacity != value) {
		if(value<CANVAS_LAYER_MIN_OPACITY || CANVAS_LAYER_MAX_OPACITY<value) {
			LWARN( "Layer", "Opacity value out of range: value=%d", value );
			return false;
		}

		//	Transform to 255 alpha value
		util::BYTE alpha;
		if (value == CANVAS_LAYER_MAX_OPACITY) {
			alpha = 255;
		}
		else {
			int range = 255 - FIRST_OPACITY;
			alpha = boost::math::iround( ((value * range) / (double)CANVAS_LAYER_MAX_OPACITY) + FIRST_OPACITY );
		}
		if (setAlpha( alpha )) {
			_opacity = value;
			_onOpacityChanged( value );
			return true;
		}
		else {
			LWARN( "Layer", "Error in set layer opacity: value=%d", value );
		}
	}
	return res;
}

bool Layer::setAlpha( util::BYTE /*alpha*/ ) {
	return true;
}

int Layer::opacity() const {
	return _opacity;
}

Layer::OpacityChanged &Layer::onOpacityChanged() {
	return _onOpacityChanged;
}

Surface *Layer::getSurface() {
	return NULL;
}

}
