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
#include "../../../../src/cfg/cfg.h"
#include "../../../../src/cfg/cmd/option.h"
#include "option.h"

Option::Option() {
}

Option::~Option() {
}

void Option::SetUp( void ) {
	util::cfg::get()
		.addValue("boolOption","desc",false)
		.addValue("strOption","desc",std::string("val"))
		.addValue("intOption","desc",10);
}

void Option::TearDown( void ) {
	util::cfg::get().removeProp("boolOption");
	util::cfg::get().removeProp("strOption");
	util::cfg::get().removeProp("intOption");
}



TEST_F( Option, constructor_one_arg ) {
	util::cfg::cmd::Option option("option");

	ASSERT_TRUE( option.property() == "option" );
	ASSERT_TRUE( !option.sname() );
}

TEST_F( Option, constructor_two_arg ) {
	util::cfg::cmd::Option option("node.option","alias");

	ASSERT_TRUE( option.property() == "node.option" );
	ASSERT_TRUE( option.alias() == "alias" );
	ASSERT_TRUE( !option.sname() );
}

TEST_F( Option, setsname ) {
	util::cfg::cmd::Option option("option","alias");
	option.setsname('o');

	ASSERT_TRUE( option.property() == "option" );
	ASSERT_TRUE( option.alias() == "alias" );
	ASSERT_TRUE( option.sname() == 'o' );
}

TEST_F( Option, def ) {
	util::cfg::cmd::Option option("boolOption");

	ASSERT_TRUE( option.def() == util::cfg::get().defaultAsString("boolOption") );
	ASSERT_TRUE( option.def().length() == 5 );
}

TEST_F( Option, showDefault ) {
	util::cfg::cmd::Option option("boolOption");
	ASSERT_TRUE( option.def() == util::cfg::get().defaultAsString("boolOption") );
	option.noShowDefault();
	ASSERT_TRUE( option.def() == "" );
	option.showDefault();
	ASSERT_TRUE( option.def() == util::cfg::get().defaultAsString("boolOption") );
}

TEST_F( Option, toggle_bool_option ) {
	util::cfg::cmd::Option option("boolOption");

	option.set();
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( Option, toggle_not_bool_option ) {
	util::cfg::cmd::Option option("strOption");

	option.set();
	ASSERT_TRUE( util::cfg::getValue<std::string> ("strOption") == "val" );
}

TEST_F( Option, set_option ) {
	util::cfg::cmd::Option option("strOption");

	option.set("str");
	ASSERT_TRUE( util::cfg::getValue<std::string> ("strOption") == "str" );
}

TEST_F( Option, set_str_option_with_empty_value ) {
	util::cfg::cmd::Option option("strOption");

	option.set("");
	ASSERT_TRUE( util::cfg::getValue<std::string> ("strOption") == "" );
}

TEST_F( Option, set_int_option_with_empty_value ) {
	util::cfg::cmd::Option option("intOption");

	option.set("");
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 10 );
}

TEST_F( Option, set_bool_option_with_empty_value ) {
	util::cfg::cmd::Option option("boolOption");

	option.set("");
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") == true );
}
