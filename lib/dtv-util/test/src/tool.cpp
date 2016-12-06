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

#include <gtest/gtest.h>
#include "../../src/tool.h"
#include "../../src/cfg/cfg.h"
#include "../../src/cfg/cmd/commandline.h"
#include "functions.h"
#include "tool.h"
#include <string>
#include <vector>

ToolTest::ToolTest() {
}

ToolTest::~ToolTest() {
}

#define TEST_TOOL \
	for(TestTool::Results::iterator it = tool._results.begin(); it != tool._results.end(); it++ ) { \
		ASSERT_TRUE( (*it) ); \
	} \

class TestTool : public util::Tool {
public:
	TestTool() {
		_help = false;
	}

	int run( util::cfg::cmd::CommandLine &/*cmd*/ ) {
		if (!_set.empty()) {
			_results.push_back( _set == util::cfg::getValue<std::string> ("tool.set") );
		}

		if (!_ver.empty()) {
			_results.push_back( util::cfg::getValue<std::string> ("tool.version") == _ver );
		}

		if (!_b.empty()) {
			_results.push_back( util::cfg::getValue<std::string> ("tool.build") == _b );
		}

		if (_help) {
			_results.push_back( util::cfg::getValue<bool> ("tool.help") );
		}

		return 0;
	}

	std::string _set;
	std::string _ver;
	bool _help;
	//	build
	std::string _b;

	//	Aux
	typedef std::vector<bool> Results;
	Results _results;

};

TEST_F( ToolTest, constructor ) {
	util::Tool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	cfg.description = "description1";
	cfg.version = "2.0.0";
	cfg.repo_version = "0.0.1";
	tool.initialize( cfg );

	ASSERT_TRUE( tool.name() == "genericTool" );
	ASSERT_TRUE( tool.description() == "description1" );
	ASSERT_EQ( "2.0.0", tool.version() );
}

TEST_F( ToolTest, execute ) {
	util::Tool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	char **params;

	const char *argv[1] = {"programName"};
	params = util::makeParams( argv, 1 );
	ASSERT_TRUE( tool.execute( 1, params ) == 0 );
	util::deleteParams( params );
}

TEST_F( ToolTest, set_one_bool ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.help=true"};
	char **params = util::makeParams( argv, 2);

	tool._set = "tool.help=true";
	tool._help = true;
	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, set_one_str ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.version=3.0"};
	char **params = util::makeParams( argv, 2);

	tool._set = "tool.version=3.0";
	tool._ver = "3.0";
	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, set_two ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.version=3.0:tool.build=Build2398"};
	char **params = util::makeParams( argv, 2);

	tool._set = "tool.version=3.0:tool.build=Build2398";
	tool._ver = "3.0";
	tool._b = "Build2398";
	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, set_three ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.help=true:tool.version=3.0:tool.build=Build2398"};
	char **params = util::makeParams( argv, 2);

	tool._set = "tool.help=true:tool.version=3.0:tool.build=Build2398";
	tool._ver = "3.0";
	tool._b = "Build2398";
	tool._help = true;

	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, empty_set_parameters ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=:tool.help=true"};
	char **params = util::makeParams( argv, 2);

	tool._set = ":tool.help=true";
	tool._help = true;
	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, empty_set_parameters_two ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.help=true:"};
	char **params = util::makeParams( argv, 2);

	tool._set = "tool.help=true:";
	tool._help = true;
	ASSERT_TRUE( tool.execute( 2, params ) == 0 );

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, empty_set_parameters_three ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=:tool.help=true:"};
	char **params = util::makeParams( argv, 2);

	ASSERT_TRUE( tool.execute( 2, params ) == 0 );
	tool._set = ":tool.help=true:";
	tool._help = true;

	TEST_TOOL

	util::deleteParams( params );
}

TEST_F( ToolTest, empty_set_parameters_four ) {
	TestTool tool;
	util::ToolConfig cfg;
	cfg.name = "genericTool";
	tool.initialize( cfg );
	const char *argv[2] = {"programName","--set=tool.help=true::"};
	char **params = util::makeParams( argv, 2);

	ASSERT_TRUE( tool.execute( 2, params ) == 0 );
	tool._set = "tool.help=true::";
	tool._help = true;

	TEST_TOOL

	util::deleteParams( params );
}
