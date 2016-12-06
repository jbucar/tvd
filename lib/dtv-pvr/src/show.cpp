/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "show.h"
#include "parental/control.h"
#include "time.h"
#include <util/mcr.h>

namespace pvr {

Show::Show()
	: _timeRange( bpt::ptime(), bpt::ptime() ), _category(category::unknown), _parental(NULL)
{
}

Show::Show( const std::string &name, const std::string &desc, const bpt::time_period &range )
	: _name(name), _description(desc), _timeRange(range), _category(category::unknown), _parental(NULL)
{
}

Show::~Show()
{
	delete _parental;
}

//	Getters
const std::string &Show::name() const {
	return _name;
}

void Show::name( const std::string &name ) {
	_name = name;
}

const std::string &Show::description() const {
	return _description;
}

void Show::description( const std::string &desc ) {
	_description = desc;
}

const bpt::time_period &Show::timeRange() const {
	return _timeRange;
}

void Show::timeRange( const bpt::time_period &range ) {
	_timeRange = range;
}

//	Category
category::Type Show::category() const {
	return _category;
}

void Show::category( category::Type cat ) {
	_category = cat;
}

//	Parental
parental::Control *Show::parentalControl() const {
	return _parental ? _parental : parental::defaultControl();
}

void Show::parentalControl( parental::Control *parental ) {
	DEL(_parental);
	_parental = parental;
}

//	Methods
int Show::percentage() const {
	bpt::ptime now;
	time::clock()->get( now );
	return percentage( now );
}

int Show::percentage( const bpt::ptime &time ) const {
	int value;
	if (!time.is_special() && !_timeRange.is_null()) {
		bpt::time_period tp = bpt::time_period( _timeRange.begin(), time );
		if (!tp.is_null() && _timeRange.intersects(tp)) {
			bpt::time_period intersect = _timeRange.intersection( tp );
			long secs = intersect.length().total_seconds();
			value = secs * 100 / _timeRange.length().total_seconds();
		}
		else if (_timeRange.begin() > time) {
			value = 0;
		}
		else {
			value = 100;
		}
	}
	else {
		value=0;
	}
	return value;
}

}

