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
#include "../../../util.h"
#include "../../../../../src/service/extension/application/applicationid.h"

TEST( ApplicationID, default_constructor ) {
	tuner::app::ApplicationID app;

	ASSERT_TRUE( app.orgID() == 0 );
	ASSERT_TRUE( app.appID() == 0 );
	ASSERT_TRUE( app.asString() == "0.0" );
}

TEST( ApplicationID, constructor ) {
	tuner::app::ApplicationID app( 1, 1 );

	ASSERT_TRUE( app.orgID() == 1 );
	ASSERT_TRUE( app.appID() == 1 );
	ASSERT_TRUE( app.asString() == "1.1" );
}

TEST( ApplicationID, makeID ) {
	tuner::app::ApplicationID app;
	const std::string dotID("10.1");

	ASSERT_TRUE( app.orgID() != 10 );
	ASSERT_TRUE( app.appID() != 1 );
	ASSERT_TRUE( tuner::app::ApplicationID::makeID( dotID, app ) );
	ASSERT_TRUE( app.orgID() == 10 );
	ASSERT_TRUE( app.appID() == 1 );
	ASSERT_TRUE( app.asString() == "10.1" );
}

TEST( ApplicationID, makeID_fail ) {
	tuner::app::ApplicationID app;
	const std::string dotID("101");
	ASSERT_FALSE( tuner::app::ApplicationID::makeID( dotID, app ) );
}

TEST( ApplicationID, makeID_fail2 ) {
	tuner::app::ApplicationID app;
	const std::string dotID("101.");
	ASSERT_FALSE( tuner::app::ApplicationID::makeID( dotID, app ) );
}

TEST( ApplicationID, makeID_fail3 ) {
	tuner::app::ApplicationID app;
	const std::string dotID(".101");
	ASSERT_FALSE( tuner::app::ApplicationID::makeID( dotID, app ) );
}

TEST( ApplicationID, compare_fail ) {
	tuner::app::ApplicationID app1( 1, 1 );
	tuner::app::ApplicationID app2( 2, 2 );

	ASSERT_TRUE( app1.orgID() == 1 );
	ASSERT_TRUE( app1.appID() == 1 );
	ASSERT_TRUE( app1.asString() == "1.1" );

	ASSERT_TRUE( app2.orgID() == 2 );
	ASSERT_TRUE( app2.appID() == 2 );
	ASSERT_TRUE( app2.asString() == "2.2" );

	ASSERT_FALSE( app1 == app2 );
}

TEST( ApplicationID, compare_ok ) {
	tuner::app::ApplicationID app1( 1, 1 );
	tuner::app::ApplicationID app2( 1, 1 );

	ASSERT_TRUE( app1.orgID() == 1 );
	ASSERT_TRUE( app1.appID() == 1 );
	ASSERT_TRUE( app1.asString() == "1.1" );

	ASSERT_TRUE( app2.orgID() == 1 );
	ASSERT_TRUE( app2.appID() == 1 );
	ASSERT_TRUE( app2.asString() == "1.1" );

	ASSERT_TRUE( app1 == app2 );
}

