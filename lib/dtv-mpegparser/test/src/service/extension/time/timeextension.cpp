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
#include "timeextension.h"

void print_ptime( ptime p ){
  printf("\n");
  printf("<<< ");
  printf("%i",p.date().day().as_number());
  printf("-");
  printf("%i",p.date().month().as_number());
  printf("-");  
  //printf("%i",p.date().year());
  printf(" ");
  
  printf("%i",p.time_of_day().hours());
  printf(":");
  printf("%i",p.time_of_day().minutes());
  printf(":");
  printf("%i",p.time_of_day().seconds());
  printf(" >>> \n");  
}

TEST_F( TimeExtensionTest, init ) {
	start( new TimeService( 0, 5000, 5000 ) );
}

TEST_F( TimeExtensionTest, basic ) {
	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );
	probe( "tot_ver0.sec" );
	ptime time( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );
	ASSERT_TRUE( ext->set() == time );
}

TEST_F( TimeExtensionTest, basic_period ) {
	ptime time( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );

	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "tot_ver0.sec" );
	probe( "tot_ver0.sec" );
	ASSERT_TRUE( ext->set() == time );
	ASSERT_TRUE( ext->count() == 1 );
}

TEST_F( TimeExtensionTest, basic_period_2 ) {
	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "tot_ver0.sec" );
	probe( "tot_ver0.sec" );
	probe( "tot_ver0_plus_one_hour.sec" );	
	probe( "tot_ver0_plus_one_hour.sec" );	
	
	ASSERT_TRUE( ext->count() == 2 );
}

TEST_F(TimeExtensionTest, forward_long_period ) {
  	ptime time( date(2038,1,18), hours(12)+minutes(00)+seconds(00) );

	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );
	
	probe( "long_period_ok.sec" );
	ASSERT_TRUE( ext->set() == time );
}

TEST_F(TimeExtensionTest, forward_long_period2 ) {
  	ptime time( date(2038,1,19), hours(12)+minutes(00)+seconds(00) );

	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "long_period_fail.sec" );
	
	ASSERT_TRUE( ext->set() == time );
}

TEST_F( TimeExtensionTest, backward_long_period ) {
	ptime time( date(1939,12,31), hours(12)+minutes(00)+seconds(00) );

	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );
	
	probe( "period_before.sec" );
	ASSERT_TRUE( ext->set() == time );
}

TEST_F( TimeExtensionTest, DISABLED_forward_and_backwars_period ) {
  
	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );
	
	probe( "long_period_ok.sec" );
	ptime time1( date(2038,1,18), hours(12)+minutes(00)+seconds(00) );	
	ASSERT_TRUE( ext->set() == time1 );
	
	probe( "period_before.sec" );
	ptime time2( date(1939,12,31), hours(12)+minutes(00)+seconds(00) );
	ASSERT_FALSE( ext->set() == time2 );
}

TEST_F( TimeExtensionTest, backward_and_forward_period ) {
	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "period_before.sec" );
	ptime time2( date(1939,12,31), hours(12)+minutes(00)+seconds(00) );
	ASSERT_TRUE( ext->set() == time2 );

	probe( "tot_ver0.sec" );
	ptime time1( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );
	ASSERT_TRUE( ext->set() == time1 );
	
}

TEST_F( TimeExtensionTest, two_seconds_forward_period ) {
	TimeService *ext = new TimeService( 0, 5000, 5000 );
	start( ext );
	mgr()->ready( true );
	
	ptime time( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );
	probe( "tot_ver0.sec" );
	ASSERT_TRUE( ext->set() == time );
	
	probe( "tot_ver0_plus_two_seconds.sec" );
	ASSERT_TRUE( ext->set() == time );
	
	ptime time2( date(2010,9,20), hours(16)+minutes(44)+seconds(21) );	
	probe( "tot_ver0_plus_one_hour.sec" );
	ASSERT_TRUE( ext->set() == time2 );
}


TEST_F( TimeExtensionTest, elapsed_period_ok ) {
	ptime time1( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );
	ptime time2( date(2010,9,20), hours(15)+minutes(44)+seconds(31) );	

	TimeService *ext = new TimeService( 0, 500, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "tot_ver0.sec" );
	probe( "tot_ver0.sec" )	;
	ASSERT_TRUE( ext->count() == 1 );	
	ASSERT_TRUE( ext->set() == time1 );	
	probe( "tot_ver0_shift_10s.sec" );
	ASSERT_TRUE( ext->set() == time2 );
	ASSERT_TRUE( ext->count() == 2 );
}

TEST_F( TimeExtensionTest, elapsed_period_fail ) {
	ptime time1( date(2010,9,20), hours(15)+minutes(44)+seconds(21) );
	ptime time2( date(2010,9,20), hours(15)+minutes(44)+seconds(31) );	

	TimeService *ext = new TimeService( 0, 20000, 5000 );
	start( ext );
	mgr()->ready( true );

	probe( "tot_ver0.sec" );
	probe( "tot_ver0.sec" )	;
	ASSERT_TRUE( ext->count() == 1 );	
	ASSERT_TRUE( ext->set() == time1 );
	probe( "tot_ver0_shift_10s.sec" );
	ASSERT_TRUE( ext->set() == time1 );
	ASSERT_TRUE( ext->count() == 1 );
}

//	TODO: Implement descriptor
