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

  Este archivo es parte de la implementaci�n de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los t�rminos de la Licencia P�blica General Reducida GNU como es publicada por la
  Free Software Foundation, seg�n la versi�n 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANT�A; ni siquiera la garant�a impl�cita de COMERCIALIZACI�N o ADECUACI�N
  PARA ALG�N PROP�SITO PARTICULAR. Para m�s detalles, revise la Licencia P�blica
  General Reducida GNU.

    Ud. deber�a haber recibido una copia de la Licencia P�blica General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/signals2.hpp>

namespace pvr {

namespace bpt = boost::posix_time;

class Time {
public:
	Time( int offset, const std::string &countryCode );
	virtual ~Time();

	//	Getters
	virtual bool isValid() const;
	virtual void get( bpt::ptime &value );
	bpt::ptime &localTime( bpt::ptime &value ) const;
	const std::string &countryCode() const;

	//  Signals
	typedef boost::signals2::signal<void ( bool isValid )> TimeValidSignal;
	TimeValidSignal &onTimeValidChanged();

protected:
	void notify( bool isValid );

private:
	TimeValidSignal _signal;
	int _offset;
	std::string _countryCode;
};

namespace time {

void init( Time *timeSource );
void fin();
bool isInitialized();

Time *clock();

std::string asString( const bpt::ptime &val );

}

}

