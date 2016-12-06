/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include <list>
#include <gtest/gtest.h>
#include "../../src/registrator.h"
#include "registrator.h"

static int a, b;
static bool cond = false;

//	Initialization for tests
REGISTER_INIT( init ) {
	b = 3;
	a = 0;
}

//	Register for basic test
REGISTER_INIT( init_basictest ) {
	a = 1;
}

REGISTER_FIN( init_basictest ) {
	a = 0;
}


//	Register for priority test
REGISTER_FIN( priorityOne ) {
	b -= 1;
	cond = (cond && (b == 1));
}

REGISTER_FIN( priorityOne_priorityTwo ) {
	b -= 1;
	cond = (cond && (b == 2));
}

REGISTER_FIN( priorityOne_priorityTwo_prioerityThree ) {
	cond = (b == 3);
}

Registrator::Registrator()
{
}

Registrator::~Registrator() {
}

void Registrator::SetUp( void ) {
	util::reg::init();
}

void Registrator::TearDown( void ) {
	util::reg::fin();
}


TEST_F( Registrator, basic ) {
	ASSERT_TRUE(a == 1);

	util::reg::fin();

	ASSERT_TRUE(a == 0);
}

TEST_F( Registrator, init_priority ) {
	util::reg::fin();
	ASSERT_TRUE( b == 1 );
	ASSERT_TRUE( cond );
}
