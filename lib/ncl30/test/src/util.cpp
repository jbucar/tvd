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
#include "../../src/util/functions.h"
#include <gtest/gtest.h>
#include <iostream>

TEST( Types, time_in_seconds ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "4s", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 4000 );
}

TEST( Types, time_in_seconds_fraction ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "4.5s", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 4500 );
}

TEST( Types, time_in_seconds_2 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "4sec", seconds );
	ASSERT_FALSE( result );
}

// TEST( Types, time_in_seconds_3 ) {
// 	util::DWORD seconds;
// 	bool result = ncl_util::parseTimeString( "4e", seconds );
// 	ASSERT_FALSE( result );
// }

TEST( Types, time_in_seconds_without_s ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "4", seconds );
	ASSERT_TRUE( result );
}

TEST( Types, time_in_seconds_invalid2 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "pepe", seconds );
	ASSERT_FALSE( result );
}

TEST( Types, time_in_seconds_invalid3 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "pepes", seconds );
	ASSERT_FALSE( result );
}

TEST( Types, time_in_hours ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "0:0:4", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 4000 );
}

TEST( Types, time_in_hours2 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "0:1:4", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 64000 );
}

TEST( Types, time_in_hours3 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "1:1:4", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 3664000 );
}

TEST( Types, time_in_hours_fraction ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "0:0:4.5", seconds );
	ASSERT_TRUE( result );
	ASSERT_TRUE( seconds == 4500 );
}

TEST( Types, time_in_hours_invalid ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "0:0pepe", seconds );
	ASSERT_FALSE( result );
}

TEST( Types, time_in_hours_invalid2 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "0:0pepe:saraza", seconds );
	ASSERT_FALSE( result );
}

TEST( Types, time_in_hours_invalid3 ) {
	util::DWORD seconds;
	bool result = ncl_util::parseTimeString( "1194:1:0", seconds );
	ASSERT_FALSE( result );
}

TEST( Types, is_valid_numeric ) {
	bool result = ncl_util::isValidNumericValue( "0" );
	ASSERT_TRUE( result );
}

TEST( Types, is_valid_numeric2 ) {
	bool result = ncl_util::isValidNumericValue( "0%" );
	ASSERT_TRUE( result );
}

TEST( Types, is_valid_numeric3 ) {
	bool result = ncl_util::isValidNumericValue( "100%" );
	ASSERT_TRUE( result );
}

TEST( Types, is_valid_numeric4 ) {
	bool result = ncl_util::isValidNumericValue( "500" );
	ASSERT_TRUE( result );
}

TEST( Types, is_valid_numeric_error ) {
	bool result = ncl_util::isValidNumericValue( "pepe" );
	ASSERT_FALSE( result );
}

TEST( Types, is_valid_numeric_error2 ) {
	bool result = ncl_util::isValidNumericValue( "10pepe" );
	ASSERT_FALSE( result );
}

TEST( Types, is_valid_numeric_error3 ) {
	bool result = ncl_util::isValidNumericValue( "-1" );
	ASSERT_FALSE( result );
}

TEST( Types, is_valid_numeric_error4 ) {
	bool result = ncl_util::isValidNumericValue( "-1%" );
	ASSERT_FALSE( result );
}

TEST( Types, percentual_value ) {
	float result = ncl_util::getPercentualValue( "50%" );
	ASSERT_TRUE( result == 0.5 );
}

TEST( Types, percentual_value2 ) {
	float result = ncl_util::getPercentualValue( "500%" );
	ASSERT_TRUE( result == 1 );
}

TEST( Types, percentual_value3 ) {
	float result = ncl_util::getPercentualValue( "-1%" );
	ASSERT_TRUE( result == 0 );
}

TEST( Types, pixel_value ) {
	int result = ncl_util::getPixelValue( "500px" );
	ASSERT_TRUE( result == 500 );
}

TEST( Types, pixel_value2 ) {
	int result = ncl_util::getPixelValue( "500" );
	ASSERT_TRUE( result == 500 );
}

TEST( Types, pixel_value3 ) {
	int result = ncl_util::getPixelValue( "0" );
	ASSERT_TRUE( result == 0 );
}

TEST( Types, pixel_value4 ) {
	int result = ncl_util::getPixelValue( "-500" );
	ASSERT_TRUE( result == -500 );
}

