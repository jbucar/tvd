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
#include "frontend.h"
#include "../../../src/provider/frontend.h"

TEST( Frontend, initialization ) {
	TestProvider prov;
	ASSERT_FALSE( prov.frontend() );

	ASSERT_TRUE( prov.start() );

	tuner::Frontend *front = prov.frontend();
	ASSERT_TRUE( front!=NULL );
	prov.stop();
}

TEST( Frontend, status ) {
	TestProvider prov;
	ASSERT_TRUE( prov.start() );
	tuner::Frontend *front = prov.frontend();
	ASSERT_TRUE( front!=NULL );

	tuner::status::Type st;
	ASSERT_TRUE( front->getStatus( st ) );
	ASSERT_FALSE( st.isLocked );
	ASSERT_EQ( st.snr, 0 );
	ASSERT_EQ( st.signal, 0 );
	prov.stop();
}

TEST( Frontend, get_network_name ) {
	TestProvider prov;
	ASSERT_TRUE( prov.start() );

	ASSERT_TRUE( prov.getNetworksCount() == 10 );
	ASSERT_TRUE( prov.getNetworkName(0) == "0" );
	ASSERT_TRUE( prov.getNetworkName(1) == "1" );
	ASSERT_TRUE( prov.getNetworkName(2) == "2" );
	ASSERT_TRUE( prov.getNetworkName(3) == "3" );
	ASSERT_TRUE( prov.getNetworkName(4) == "4" );
	ASSERT_TRUE( prov.getNetworkName(5) == "5" );
	ASSERT_TRUE( prov.getNetworkName(6) == "6" );
	ASSERT_TRUE( prov.getNetworkName(7) == "7" );
	ASSERT_TRUE( prov.getNetworkName(8) == "8" );
	ASSERT_TRUE( prov.getNetworkName(9) == "9" );

	prov.stop();
}

TEST( Frontend, find_network ) {
	TestProvider prov;
	ASSERT_TRUE( prov.start() );

	ASSERT_TRUE( prov.getNetworksCount() == 10 );
	ASSERT_TRUE( prov.findNetwork("0") == 0 );
	ASSERT_TRUE( prov.findNetwork("1") == 1 );
	ASSERT_TRUE( prov.findNetwork("2") == 2 );
	ASSERT_TRUE( prov.findNetwork("3") == 3 );
	ASSERT_TRUE( prov.findNetwork("4") == 4 );
	ASSERT_TRUE( prov.findNetwork("5") == 5 );
	ASSERT_TRUE( prov.findNetwork("6") == 6 );
	ASSERT_TRUE( prov.findNetwork("7") == 7 );
	ASSERT_TRUE( prov.findNetwork("8") == 8 );
	ASSERT_TRUE( prov.findNetwork("9") == 9 );

	prov.stop();
}

