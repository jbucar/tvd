/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "tot.h"
#include <util/string.h>
#include <stdio.h>

#define BCD2BIN(val) (((val) & 0xF) + ((val)>>4)*10) //	Convert from BCD to Binary

namespace tuner {

MJDate::MJDate()
{
	_mjd = 0;
	_lsb = 0;
}

MJDate::MJDate( const MJDate &other )
	: _mjd(other._mjd), _lsb(other._lsb)
{
}

MJDate::MJDate( WORD mjd, DWORD lsb )
	: _mjd( mjd ), _lsb(lsb)
{
}

MJDate::~MJDate()
{
}

void MJDate::calc( int &year, int &month, int &day ) const {
	year  = static_cast<int>((_mjd-15078.2)/365.25);
	month = static_cast<int>((_mjd-14956.1-int(year*365.25))/30.6001);
	day   = _mjd-14956 - int(year*365.25) - int(month*30.6001);
	int k     = (month == 14 || month == 15) ? 1 : 0;
	year  = year+k+1900;
	month = month-1-(k*12);
}

int MJDate::year() const {
	if (_mjd) {
		int year, month, day;
		calc( year, month, day );
		return year;
	}
	return 0;
}

int MJDate::month() const {
	if (_mjd) {
		int year, month, day;
		calc( year, month, day );
		return month;
	}
	return 0;
}

int MJDate::day() const {
	if (_mjd) {
		int year, month, day;
		calc( year, month, day );
		return day;
	}
	return 0;
}

int MJDate::hours() const {
	return BCD2BIN((_lsb & 0x00FF0000) >> 16);
}

int MJDate::minutes() const {
	return BCD2BIN((_lsb & 0x0000FF00) >> 8);
}

int MJDate::seconds() const {
	return BCD2BIN(_lsb & 0x000000FF);
}

pt::ptime MJDate::get() const {
	return pt::ptime(
		boost::gregorian::date((unsigned short)year(),(unsigned short)month(),(unsigned short)day()),
		pt::time_duration( hours(), minutes(), seconds() ) );
}

pt::time_duration MJDate::getTime() const {
	return pt::time_duration( hours(), minutes(), seconds() );
}

std::string MJDate::asString() const {
	char buffer[30];

	if (_mjd) {
		snprintf( buffer, 30, "%02d/%02d/%04d %02d:%02d:%02d",
			day(), month(), year(), hours(), minutes(), seconds() );
	}
	else {
		snprintf( buffer, 30, "%02d:%02d:%02d", hours(), minutes(), seconds() );
	}
	return std::string(buffer);
}

}
