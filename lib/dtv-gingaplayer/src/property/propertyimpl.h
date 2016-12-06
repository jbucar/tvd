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

#include "property.h"
#include "check.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace player {

template<typename T>
class PropertyImpl : public Property {
public:
	PropertyImpl( bool needRefresh, T &value )
		: _value(value), _needRefresh(needRefresh), _check(boost::bind(&check::always<T>,_1)) {}
	virtual ~PropertyImpl() {}

	virtual bool canStart() const { return _check(_value); }
	virtual bool needRefresh() const { return _needRefresh; }
	virtual bool assign( const T &value ) {
		bool result=false;
		if (check(value) && _value != value) {
			_value = value;
			markModified();
			result=true;
		}
		return result;
	}

	//	Getters and setters
	typedef boost::function<bool (const T &value )> Method;
	typedef boost::function<void (void)> VoidMethod;
	void setCheck( const Method &method ) { BOOST_ASSERT( !method.empty() ); _check = method; }
	void setApply( const VoidMethod &method ) { BOOST_ASSERT( !method.empty() ); _apply = method; }
	void setNeedResfresh(bool needRefresh) { _needRefresh = needRefresh; }

protected:
	virtual void applyChanges() {
		if (!_apply.empty()) {
			_apply();
		}
	}

	bool check( const T &value ) {
		if (!_check(value)) {
			throw std::runtime_error( "Invalid value" );
		}
		return true;
	}

private:
	T &_value;
	bool _needRefresh;
	Method _check;
	VoidMethod _apply;

	PropertyImpl() {}
	PropertyImpl &operator=( const PropertyImpl & /*other*/ ) { return *this; }
};

template<typename T>
class MultiPropertyImpl : public PropertyImpl<T> {
public:
	MultiPropertyImpl( bool &propertyIsApplied, bool needRefresh, T &value )
		: PropertyImpl<T>( needRefresh, value ), _propertyIsApplied(propertyIsApplied) {}
	virtual ~MultiPropertyImpl() {}

protected:
	virtual void markModified() {
		PropertyImpl<T>::markModified();
		_propertyIsApplied=false;
	}

	virtual void applyChanges() {
		if (!_propertyIsApplied) {
			PropertyImpl<T>::applyChanges();
			_propertyIsApplied=true;
		}
	}

private:
	bool &_propertyIsApplied;
};


}
