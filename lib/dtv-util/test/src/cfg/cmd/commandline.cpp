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

#include "../../../../src/cfg/cfg.h"
#include "../../../../src/cfg/propertynode.h"
#include "../../../../src/cfg/cmd/commandline.h"
#include "../../functions.h"
#include "commandline.h"

#include <stdexcept>

CommandLine::CommandLine() {
}

CommandLine::~CommandLine() {
}

void CommandLine::SetUp( void ) {
	util::cfg::get().addValue("set","Description",false);
	util::cfg::get().addValue("boolOption","Description",false);
	util::cfg::get().addValue("intOption","Description",10);
	util::cfg::get().addNode("node");
	util::cfg::get() ("node").addValue("boolOption","Description",false);
	util::cfg::get() ("node").addValue("intOption","Description",10);
	std::string val("val");
	util::cfg::get() ("node").addValue("strOption","Description",val);
}

void CommandLine::TearDown( void ) {
	util::cfg::get().removeNode("node");
	util::cfg::get().removeProp("boolOption");
	util::cfg::get().removeProp("intOption");
	util::cfg::get().removeProp("set");
	util::deleteParams( _params );
}

TEST_F( CommandLine, parse_empty ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );
	cmd.parse();
}

TEST_F( CommandLine, register_option ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );
	cmd.registerOpt("boolOption","alias",'o');

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( cmd.isRegistered('o') );
}

TEST_F( CommandLine, register_nested_option ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );
	cmd.registerOpt("node.boolOption","alias",'o');

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( cmd.isRegistered('o') );
}

TEST_F( CommandLine, register_alias_already_registered ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);

	util::cfg::cmd::CommandLine cmd( 1, _params );
	cmd.registerOpt("boolOption","alias");
	ASSERT_THROW ( cmd.registerOpt("intOption","alias"), std::runtime_error );
}

TEST_F( CommandLine, register_property_already_registered ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);

	util::cfg::cmd::CommandLine cmd( 1, _params );
	cmd.registerOpt("boolOption","alias");
	ASSERT_THROW ( cmd.registerOpt("boolOption","anotherAlias"), std::runtime_error );
}

TEST_F( CommandLine, register_property_thats_not_in_tree ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );

	ASSERT_THROW ( cmd.registerOpt("nonOption","alias",'o'), std::runtime_error );

	ASSERT_TRUE( !cmd.isRegistered("alias") );
	ASSERT_TRUE( !cmd.isRegistered('o') );
}

TEST_F( CommandLine, register_two_options ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );

	cmd.registerOpt("boolOption","alias",'b');
	cmd.registerOpt("intOption","anotherAlias",'i');

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( cmd.isRegistered('b') );
	ASSERT_TRUE( cmd.isRegistered("anotherAlias") );
	ASSERT_TRUE( cmd.isRegistered('i') );
}

TEST_F( CommandLine, register_property_thats_already_registered_with_diferent_alias ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );

	cmd.registerOpt("boolOption","alias",'b');
	ASSERT_THROW ( cmd.registerOpt("boolOption","anotherAlias",'b'), std::runtime_error );

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( !cmd.isRegistered("anotherAlias") );
	ASSERT_TRUE( cmd.isRegistered('b') );
}

TEST_F( CommandLine, register_property_thats_already_registered_with_diferent_shortname ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );

	cmd.registerOpt("boolOption","alias",'b');
	ASSERT_THROW ( cmd.registerOpt("boolOption","alias",'i'), std::runtime_error );

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( cmd.isRegistered('b') );
	ASSERT_TRUE( !cmd.isRegistered('i') );
}

TEST_F( CommandLine, register_two_properties_with_no_shortnames ) {
	const char *argv[1] = {"programName"};
	_params = util::makeParams( argv, 1);
	util::cfg::cmd::CommandLine cmd( 1, _params );

	cmd.registerOpt("intOption","alias");
	cmd.registerOpt("boolOption","anotherAlias");

	ASSERT_TRUE( cmd.isRegistered("alias") );
	ASSERT_TRUE( cmd.isRegistered("anotherAlias") );
}

TEST_F( CommandLine, parse_short_bool_opt_without_argument ) {
	//	Case: -o
	const char *argv[2] = {"programName","-o"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("boolOption","option",'o');

	cmd.parse();
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_short_no_bool_opt_without_argument ) {
	//	Case: -o
	const char *argv[2] = {"programName","-o"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("intOption","option",'o');

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 10 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_short_no_bool_opt_with_argument ) {
	//	Case: -o arg
	const char *argv[3] = {"programName","-o","20"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );

	cmd.registerOpt("intOption","option",'o');

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 20 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_short_opt_with_bad_argument ) {
	//	Case: -o arg
	const char *argv[3] = {"programName","-o","true"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("intOption","option",'o');

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_short_opt ) {
	//	Case: -node.o
	const char *argv[2] = {"programName","-node.o"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );

	cmd.registerOpt("node.boolOption","boolOption",'o');
	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( !util::cfg::getValue<bool> ("node.boolOption") );
}

TEST_F( CommandLine, parse_long_bool_opt_without_argument ) {
	//	Case: --option
	const char *argv[2] = {"programName","--option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("boolOption","option",'o');

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_alias_named_like_property ) {
	//	Case: --option
	const char *argv[2] = {"programName","--intOption"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("boolOption","intOption",'o');

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
	ASSERT_TRUE( cmd.isSet("intOption") );
}

TEST_F( CommandLine, parse_long_no_bool_opt_without_argument ) {
	//	Case: --option
	const char *argv[2] = {"programName","--option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("intOption","option",'o');

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 10 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_long_no_bool_opt_with_argument ) {
	//	Case: --option arg
	const char *argv[3] = {"programName","--option","20"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("intOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 20 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_long_opt_with_bad_argument ) {
	//	Case: --option arg
	const char *argv[3] = {"programName","--intOption","true"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 10 );
}

TEST_F( CommandLine, parse_long_opt_with_equal_and_argument ) {
	//	Case: --option=arg
	const char *argv[2] = {"programName","--option=20"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("intOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("intOption") == 20 );
}

TEST_F( CommandLine, parse_long_opt_with_equal_and_bad_argument ) {
	//	Case: --option=arg
	const char *argv[2] = {"programName","--option=20"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( !util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_long_opt_with_equal_space_and_argument ) {
	//	Case: --option= arg
	const char *argv[3] = {"programName","--option=","true"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_long_opt_with_space_equal_and_argument ) {
	//	Case: --option =arg
	const char *argv[3] = {"programName","--option","=true"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( !util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_long_opt_with_space_equal_space_and_argument ) {
	//	Case: --option = arg
	const char *argv[4] = {"programName","--option","=","true"};
	_params = util::makeParams( argv, 4);
	util::cfg::cmd::CommandLine cmd( 4, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( !util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_long_opt_with_equal_and_no_argument ) {
	//	Case: --option=
	const char *argv[2] = {"programName","--option="};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_long_opt_with_equal_and_argument_and_argument ) {
	//	Case: --option=
	const char *argv[3] = {"programName","--option=true","false"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("boolOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("boolOption") );
}

TEST_F( CommandLine, parse_nested_bool_option_without_argument ) {
	//	Case: --option
	const char *argv[2] = {"programName","--option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.boolOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<bool> ("node.boolOption") );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_nested_no_bool_option_without_argument ) {
	//	Case: --option
	const char *argv[2] = {"programName","--option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_nested_option_with_argument ) {
	//	Case: --option arg
	const char *argv[3] = {"programName","--option","20"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("node.intOption","option");

	cmd.parse();
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 20 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_nested_option_with_bad_argument ) {
	//	Case: --option arg
	const char *argv[3] = {"programName","--option","true"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_option_with_equal_and_argument ) {
	//	Case: --option=arg
	const char *argv[2] = {"programName","--option=20"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.intOption","option");

	cmd.parse();
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 20 );
	ASSERT_TRUE( cmd.isSet("option") );
}

TEST_F( CommandLine, parse_nested_option_with_equal_and_bad_argument ) {
	//	Case: --option=arg
	const char *argv[2] = {"programName","--option=true"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_int_option_with_equal_space_and_argument ) {
	//	Case: --option= arg
	const char *argv[3] = {"programName","--option=", "20"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_str_option_with_equal_space_and_argument ) {
	//	Case: --option= arg
	const char *argv[3] = {"programName","--option=", "value"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("node.strOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<std::string> ("node.strOption") == "" );
}

TEST_F( CommandLine, parse_nested_option_with_space_equal_space_and_argument ) {
	//	Case: --option = arg
	const char *argv[4] = {"programName","--option", "=", "20"};
	_params = util::makeParams( argv, 4);
	util::cfg::cmd::CommandLine cmd( 4, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_option_with_space_equal_and_argument ) {
	//	Case: --option =arg
	const char *argv[3] = {"programName","--option","=20"};
	_params = util::makeParams( argv, 3);
	util::cfg::cmd::CommandLine cmd( 3, _params );
	cmd.registerOpt("node.intOption","option");

	ASSERT_FALSE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_int_option_with_no_argument ) {
	//	Case: --option=
	const char *argv[2] = {"programName","--option="};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.intOption","option");

	cmd.parse();
	ASSERT_TRUE( util::cfg::getValue<int> ("node.intOption") == 10 );
}

TEST_F( CommandLine, parse_nested_option_str_with_no_argument ) {
	//	Case: --option=
	const char *argv[2] = {"programName","--option="};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("node.strOption","option");

	ASSERT_TRUE( cmd.parse() );
	ASSERT_TRUE( util::cfg::getValue<std::string> ("node.strOption") == "" );
}

TEST_F( CommandLine, parse_bad_option ) {
	//	Case: option (option exists in tree)
	const char *argv[2] = {"programName","option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );
	cmd.registerOpt("intOption","option");

	ASSERT_FALSE( cmd.parse() );
}

TEST_F( CommandLine, parse_unregistered_short_option ) {
	//	Case: -o (unregistered short name)
	const char *argv[2] = {"programName","-o"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );

	ASSERT_FALSE( cmd.parse() );
}

TEST_F( CommandLine, parse_unregistered_option ) {
	//	Case: --option (not exists in tree)
	const char *argv[2] = {"programName","--option"};
	_params = util::makeParams( argv, 2);
	util::cfg::cmd::CommandLine cmd( 2, _params );

	ASSERT_FALSE( cmd.parse() );
}
