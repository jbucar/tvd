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

#include "types.h"
#include <gtest/gtest.h>

TEST( Logos, basic ) {
	pvr::logos::Service srv;
	ASSERT_TRUE( srv.start() );
	srv.stop();
}

TEST( Logos, add_basic_provider ) {
	pvr::logos::Service srv;
	srv.add( new BasicLogosProvider() );
	ASSERT_TRUE( srv.start() );
	ASSERT_TRUE( srv.isEnabled("dummy") );
	srv.stop();
}

TEST( Logos, find_without_provider_and_without_start ) {
	pvr::logos::Service srv;
	pvr::Channel *ch = (pvr::Channel *)10;
	const std::string res=srv.find( ch );
	ASSERT_TRUE( res.empty() );
}

TEST( Logos, find_with_provider_and_without_start ) {
	pvr::logos::Service srv;
	srv.add( new BasicLogosProvider() );
	pvr::Channel *ch = (pvr::Channel *)10;
	const std::string res=srv.find( ch );
	ASSERT_TRUE( res.empty() );
}

TEST( Logos, find_basic ) {
	pvr::logos::Service srv;
	srv.add( new BasicLogosProvider() );
	srv.start();
	pvr::Channel *ch = (pvr::Channel *)10;
	const std::string res=srv.find( ch );
	ASSERT_EQ( res, "pepe" );
	srv.stop();
}

TEST( Logos, find_whitout_ch ) {
	pvr::logos::Service srv;
	srv.add( new BasicLogosProvider() );
	srv.start();
	const std::string res=srv.find( NULL );
	ASSERT_EQ( res, "" );
	srv.stop();
}

TEST( Logos, find_not_found ) {
	pvr::logos::Service srv;
	srv.add( new NotFoundLogosProvider() );
	srv.start();
	pvr::Channel *ch = (pvr::Channel *)10;
	const std::string res=srv.find( ch );
	ASSERT_TRUE( res.empty() );
	srv.stop();
}

TEST( Logos, find_with_2_provider_check_priority ) {
	pvr::logos::Service srv;
	srv.add( new ParamLogosProvider("pepe1",1) );
	srv.add( new ParamLogosProvider("pepe2",2) );
	srv.start();
	pvr::Channel *ch = (pvr::Channel *)10;
	const std::string res=srv.find( ch );
	ASSERT_EQ( res, "pepe2" );
	srv.stop();
}

