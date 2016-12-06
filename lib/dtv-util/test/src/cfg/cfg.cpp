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

#include <string>
#include "../../../src/cfg/cfg.h"
#include "cfg.h"

Cfg::Cfg() {
}

Cfg::~Cfg() {
}

void Cfg::SetUp( void ) {
	util::cfg::get().addValue("intOpt", "intOptOfRoot", 20)
		.addValue("strOpt", "strOptOfRoot", std::string("str"))
		.addValue("boolOpt", "boolOptOfRoot", false)
		.addNode("child")
		.addValue("boolOpt", "boolOptOfChild", false)
		.addNode("grandson")
		.addValue("strOpt", "strOptOfGrandson", std::string("str"));
	util::cfg::get().addNode("anotherChild")
		.addValue("strOpt", "strOptOfAnotherChild", std::string("str"));
}

void Cfg::TearDown( void ) {
	util::cfg::get().clear();
}


TEST_F( Cfg, get ) {
	ASSERT_TRUE( util::cfg::get().name() == "root" );
}

TEST_F( Cfg, get_with_path ) {
	ASSERT_TRUE( util::cfg::get("child").name() == "child" );
	ASSERT_TRUE( util::cfg::get("child.grandson").name() == "grandson" );
	ASSERT_TRUE( util::cfg::get("anotherChild").name() == "anotherChild" );
}

TEST_F( Cfg, get_value ) {
	ASSERT_TRUE( util::cfg::getValue<int> ("intOpt") == 20 );
	ASSERT_FALSE( util::cfg::getValue<bool> ("child.boolOpt") );
}

TEST_F( Cfg, get_value_with_path ) {
	ASSERT_TRUE( util::cfg::getValue<std::string> ("child.grandson.strOpt") == "str" );
	ASSERT_TRUE( util::cfg::getValue<std::string> ("anotherChild.strOpt") == "str" );
}

TEST_F( Cfg, set_int_value ) {
	util::cfg::setValue("intOpt", 10);
	ASSERT_TRUE( util::cfg::getValue<int> ("intOpt") == 10 );
}

TEST_F( Cfg, set_bool_value ) {
	util::cfg::setValue("boolOpt", true);
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOpt") );
}

TEST_F( Cfg, set_str_value ) {
	util::cfg::setValue("strOpt", std::string("val"));
	ASSERT_TRUE( util::cfg::getValue<std::string> ("strOpt") == "val" );
}

TEST_F( Cfg, set_bool_value_with_path ) {
	util::cfg::setValue("child.boolOpt", true);
	ASSERT_TRUE( util::cfg::getValue<bool> ("child.boolOpt") );
}

TEST_F( Cfg, set_str_value_with_path ) {
	util::cfg::setValue("anotherChild.strOpt", std::string("val"));
	ASSERT_TRUE( util::cfg::getValue<std::string> ("anotherChild.strOpt") == "val" );
}

TEST_F( Cfg, set_str_value_with_path_deep ) {
	util::cfg::setValue("child.grandson.strOpt", std::string("val"));
	ASSERT_TRUE( util::cfg::getValue<std::string> ("child.grandson.strOpt") == "val" );
}

TEST_F( Cfg, set_bad_str_value ) {
	ASSERT_THROW( util::cfg::setValue("strOpt", 10), std::runtime_error );
	ASSERT_THROW( util::cfg::setValue("strOpt", true), std::runtime_error );
}

TEST_F( Cfg, set_bad_bool_value ) {
	ASSERT_THROW( util::cfg::setValue("boolOpt", "string"), std::runtime_error );
	ASSERT_THROW( util::cfg::setValue("boolOpt", 10), std::runtime_error );
}

TEST_F( Cfg, set_bad_int_value ) {
	ASSERT_THROW( util::cfg::setValue("intOpt", "string"), std::runtime_error );
	ASSERT_THROW( util::cfg::setValue("intOpt", true), std::runtime_error );
}
