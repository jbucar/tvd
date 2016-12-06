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

#include "../../src/show.h"
#include "../../src/parental/control.h"
#include <gtest/gtest.h>

namespace bpt=boost::posix_time;

TEST( Show, default_constructor ) {
	pvr::Show show;
	ASSERT_EQ( show.name(), "" );
	ASSERT_EQ( show.description(), "" );
	ASSERT_EQ( show.category(), pvr::category::unknown );
	ASSERT_TRUE( show.parentalControl() != NULL );
}

TEST( Show, basic_constructor ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration( 0, 0, 0 ) );
	bpt::ptime end( boost::gregorian::date(2013,1,1), bpt::time_duration( 0, 0, 0 ) );
	pvr::bpt::time_period r(start,end);

	pvr::Show show( "name1", "desc1", r );
	ASSERT_EQ( show.name(), "name1" );
	ASSERT_EQ( show.description(), "desc1" );
	ASSERT_EQ( show.timeRange(), r );
	ASSERT_EQ( show.category(), pvr::category::unknown );
	ASSERT_TRUE( show.parentalControl() != NULL );
}

TEST( Show, category ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration( 0, 0, 0 ) );
	bpt::ptime end( boost::gregorian::date(2013,1,1), bpt::time_duration( 0, 0, 0 ) );
	pvr::bpt::time_period r(start,end);

	pvr::Show show( "name1", "desc1", r );
	show.category( pvr::category::news );
	ASSERT_EQ( show.category(), pvr::category::news );
}

TEST( Show, parental ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration( 0, 0, 0 ) );
	bpt::ptime end( boost::gregorian::date(2013,1,1), bpt::time_duration( 0, 0, 0 ) );
	pvr::bpt::time_period r(start,end);

	pvr::Show show( "name1", "desc1", r );

	pvr::parental::Control *ctrl = new pvr::parental::WithoutControl();
	show.parentalControl( ctrl );
	ASSERT_EQ( show.parentalControl(), ctrl );
}

TEST( Show, percentage_param_mayor_range_100 ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration(0,0,0) );
	bpt::ptime end( boost::gregorian::date(2012,1,2), bpt::time_duration(0,0,0) );
	bpt::time_period r(start,end);
	pvr::Show show( "name1", "desc1", r );
	bpt::ptime now = bpt::microsec_clock::local_time();
	ASSERT_EQ( show.percentage( now ), 100 );
}

TEST( Show, percentage_param_menor_range_0 ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration(0,0,0) );
	bpt::ptime end( boost::gregorian::date(2012,1,2), bpt::time_duration(0,0,0) );
	bpt::time_period r(start,end);
	pvr::Show show( "name1", "desc1", r );
	bpt::ptime old( boost::gregorian::date(2010,1,1), bpt::time_duration(0,0,0) );
	ASSERT_EQ( show.percentage( old ), 0 );
}

TEST( Show, percentage_param_invalid_0 ) {
	bpt::ptime start( boost::gregorian::date(2012,1,1), bpt::time_duration(0,0,0) );
	bpt::ptime end( boost::gregorian::date(2012,1,2), bpt::time_duration(0,0,0) );
	bpt::time_period r(start,end);
	pvr::Show show( "name1", "desc1", r );
	bpt::ptime invalid;
	ASSERT_EQ( show.percentage( invalid ), 0 );
}

TEST( Show, percentage_range_invalid_0 ) {
	bpt::ptime start;
	bpt::ptime end;
	bpt::time_period r(start,end);
	pvr::Show show( "name1", "desc1", r );
	bpt::ptime now = bpt::microsec_clock::local_time();
	ASSERT_EQ( show.percentage( now ), 0 );
}


