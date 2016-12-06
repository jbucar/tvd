/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "../../include/layout/LayoutRegion.h"
#include "../util/functions.h"
#include <gingaplayer/property/types.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/math/special_functions/round.hpp>
#include <iostream>

namespace ppt = player::property::type;

#define get( var, _var, method ) \
int var; \
if (ncl_util::isPercentualValue( _var )) { \
	var = boost::math::iround( float(_parent->method()) * ncl_util::getPercentualValue( _var ) - 0.1 ); \
} else { \
	var = ncl_util::getPixelValue( _var ); \
} \

#define getPositionProperty( _property, positionMethod, sizeMethod, _opositeProperty, _sizeProperty) \
int property; \
if (!_haveParent) { \
	property = ncl_util::getPixelValue( _property ); \
} else { \
	property = _parent->positionMethod(); \
	if ( _property.empty() && !_opositeProperty.empty() && !_sizeProperty.empty()) { \
		get( opositeValue, _opositeProperty, sizeMethod ); \
		property += _parent->sizeMethod() - sizeMethod() - opositeValue; \
	} else { \
		get( myValue, _property, sizeMethod ); \
		property += myValue; \
	} \
} \
return property; \


#define getSizeProperty( _property, positionMethod1, positionMethod2, sizeMethod ) \
int sizeProperty = 0; \
if (!_haveParent) { \
	sizeProperty = ncl_util::getPixelValue( _property ) - positionMethod1() - positionMethod2(); \
} else if (_property.empty()) { \
	sizeProperty = _parent->sizeMethod() - (positionMethod1() - _parent->positionMethod1()) - (positionMethod2() - _parent->positionMethod2()); \
} else { \
	get( myValue, _property, sizeMethod ); \
	sizeProperty = myValue; \
} \
return sizeProperty; \

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
LayoutRegion::LayoutRegion( const std::string &id ) :
		Entity( id ) {

	_parent = NULL;
	_haveParent = false;
	_width = "";
	_left = "";
	_right = "";
	_height = "";
	_top = "";
	_bottom = "";

	_zIndex = -1;
	_title = "";
}

LayoutRegion::~LayoutRegion() {
	if (_parent != NULL) {
		_parent->removeRegion( this );
		_parent = NULL;
	}

	BOOST_FOREACH( const LayoutRegions::value_type & child, _childs )
	{
		delete child.second;
		//child.first = NULL;
	}
	_childs.clear();

}

void LayoutRegion::addRegion( LayoutRegion* region ) {
	std::string id = region->getId();
	if (!_childs.count( id )) {
		region->setParent( this );
		_childs[id] = region;
	}
}

void LayoutRegion::removeRegion( LayoutRegion* region ) {
	std::string id = region->getId();
	/*if (_childs.count( id )) {
	 delete _childs[id];
	 _childs[id] = NULL;
	 }*/
}

/**
 * Crea otra región con exactamente las mismas propiedades.
 * @return Una nueva instancia de @c LayoutRegion.
 */
LayoutRegion* LayoutRegion::cloneRegion() {

	LayoutRegion *newRegion = new LayoutRegion( getId() );
	newRegion->setParent( _parent );

	newRegion->setTop( _top );
	newRegion->setLeft( _left );
	newRegion->setRight( _right );
	newRegion->setBottom( _bottom );
	newRegion->setWidth( _width );
	newRegion->setHeight( _height );
	newRegion->setZIndex( getZIndex() );
	newRegion->setTitle( getTitle() );

	BOOST_FOREACH( const LayoutRegions::value_type & child, _childs )
	{
		LayoutRegion *childRegion = child.second->cloneRegion();
		newRegion->addRegion( childRegion );
	}

	return newRegion;
}

/**
 * Setea una propiedad a una región.
 * @param name Un string con el nombre de la propiedad.
 * @param value Un string con el valor a setear.
 * @return True en caso de haber aplicado la propiedad, false de lo contrario.
 */
bool LayoutRegion::setProperty( const std::string &name, const std::string &value ) {

	ppt::Type property = player::property::getProperty( name.c_str() );
	bool result = true;

	switch ( property ) {
		case ppt::left: {
			if (ncl_util::isValidNumericValue( value )) {
				_left = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::right: {
			if (ncl_util::isValidNumericValue( value )) {
				_right = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::top: {
			if (ncl_util::isValidNumericValue( value )) {
				_top = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::bottom: {
			if (ncl_util::isValidNumericValue( value )) {
				_bottom = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::width: {
			if (ncl_util::isValidNumericValue( value )) {
				_width = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::height: {
			if (ncl_util::isValidNumericValue( value )) {
				_height = value;
			} else {
				result = false;
			}
			break;
		}
		case ppt::zIndex: {
			try {
				int zvalue = ncl_util::getPixelValue( value, true );
				result = (zvalue >= 0) && (zvalue <= 255);
				if (result) {
					setZIndex( zvalue );
				}
			} catch (boost::bad_lexical_cast &) {
				result = false;
			}
			break;
		}
		case ppt::title: {
			setTitle( value );
			break;
		}
		case ppt::size: {
			std::vector<std::string> tokens;
			boost::split( tokens, value, boost::is_any_of( "," ) );
			if (tokens.size() == 2) {
				result = setProperty( "width", tokens[0] );
				result = result && setProperty( "height", tokens[1] );
			}
			break;
		}
		case ppt::location: {
			std::vector<std::string> tokens;
			boost::split( tokens, value, boost::is_any_of( "," ) );
			if (tokens.size() == 2) {
				result = setProperty( "left", tokens[0] );
				result = result && setProperty( "top", tokens[1] );
			}
			break;
		}
		case ppt::bounds: {
			std::vector<std::string> tokens;
			boost::split( tokens, value, boost::is_any_of( "," ) );
			if (tokens.size() == 4) {
				result = setProperty( "width", tokens[2] );
				result = result && setProperty( "left", tokens[0] );
				result = result && setProperty( "height", tokens[3] );
				result = result && setProperty( "top", tokens[1] );
			}
			break;
		}
		default: {
			result = false;
			break;
		}
	}

	return result;
}

LayoutRegion* LayoutRegion::getRegionRecursively( const std::string &id ) {
	LayoutRegion* region = NULL;

	if (getId() == id) {
		region = this;
	} else if (_childs.count( id ) != 0) {
		region = _childs[id];
	} else {
		BOOST_FOREACH ( const LayoutRegions::value_type & child, _childs )
		{
			region = child.second->getRegionRecursively( id );
			if (region != NULL) {
				break;
			}
		}
	}
	return region;

}

void LayoutRegion::setDevice( Device &device ) {
	if (device.name != _device.name || device.number != _device.number) {
		_device = device;
	}
}

Device &LayoutRegion::getDevice() {
	if (_haveParent) {
		return _parent->getDevice();
	}
	return _device;
}

void LayoutRegion::setParent( LayoutRegion* parent ) {
	if ( !_haveParent && parent != NULL ) {
		_parent = parent;
		_haveParent = true;
	}
}

int LayoutRegion::getRight() {
	getPositionProperty( _right, getRight, getWidth, _left, _width );
}

int LayoutRegion::getLeft() {
	getPositionProperty( _left, getLeft, getWidth, _right, _width );
}

int LayoutRegion::getTop() {
	getPositionProperty( _top, getTop, getHeight, _bottom, _height );
}

int LayoutRegion::getBottom() {
	getPositionProperty( _bottom, getBottom, getHeight, _top, _height );
}

int LayoutRegion::getWidth() {
	getSizeProperty( _width, getLeft, getRight, getWidth );
}

int LayoutRegion::getHeight() {
	getSizeProperty( _height, getTop, getBottom, getHeight );
}

int LayoutRegion::getZIndex() {
	if (_zIndex >= 0) {
		return _zIndex;
	} else {
		if (_haveParent) {
			return _parent->getZIndex();
		} else {
			return 0;
		}
	}
}

const std::string &LayoutRegion::getTitle() {
	return _title;
}

void LayoutRegion::setLeft( const std::string &value ) {
	_left = value;
}

void LayoutRegion::setTop( const std::string &value ) {
	_top = value;
}

void LayoutRegion::setWidth( const std::string &value ) {
	_width = value;
}

void LayoutRegion::setHeight( const std::string &value ) {
	_height = value;
}

void LayoutRegion::setBottom( const std::string &value ) {
	_bottom = value;
}

void LayoutRegion::setRight( const std::string &value ) {
	_right = value;
}

void LayoutRegion::setZIndex( int value ) {
	_zIndex = value;
}

void LayoutRegion::setTitle( const std::string &value ) {
	_title = value;
}

}
}
}
}
}
