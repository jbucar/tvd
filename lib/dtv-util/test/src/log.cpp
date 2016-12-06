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

#include "log.h"
#include "../../src/cfg/cfg.h"
#include "../../src/cfg/xmlparser.h"
#include "../../src/registrator.h"
#include "../../src/log.h"
#include "generated/config.h"
#include <boost/filesystem.hpp>
#include <fstream>

#if LOG_USE_LOG

namespace fs = boost::filesystem;

#define doLogArgs( c )							\
	LINFO(c, "Mensaje de prueba con args: %c", 'A');		\
	LINFO(c, "Mensaje de prueba con args: %d", 1);			\
	LINFO(c, "Mensaje de prueba con args: %i", -1);			\
	LINFO(c, "Mensaje de prueba con args: %o", 0172);		\
	LINFO(c, "Mensaje de prueba con args: %0.2f", 1.255f);		\
	LINFO(c, "Mensaje de prueba con args: %0.2e", 0.0123e+2);	\
	LINFO(c, "Mensaje de prueba con args: %0.2E", 0.0123E+2);	\
	LINFO(c, "Mensaje de prueba con args: %x", 0x1f);		\
	LINFO(c, "Mensaje de prueba con args: %X", 0x1f);		\
	LINFO(c, "Mensaje de prueba con args: %s", "hola");

#define doLog( c ) 			\
	LTRACE(c, "Mensaje de prueba");	\
	LDEBUG(c, "Mensaje de prueba");	\
	LINFO(c, "Mensaje de prueba");	\
	LWARN(c, "Mensaje de prueba");	\
	LERROR(c, "Mensaje de prueba");

#define doLogGroup( g, c ) 					\
	LOG_PUBLISH( TRACE, g, c, "Mensaje de prueba" ); \
	LOG_PUBLISH( DEBUG, g, c, "Mensaje de prueba" ); \
	LOG_PUBLISH( INFO, g, c, "Mensaje de prueba" ); \
	LOG_PUBLISH( WARN, g, c, "Mensaje de prueba" ); \
	LOG_PUBLISH( ERROR, g, c, "Mensaje de prueba" );

LoggerTest::LoggerTest() {
	const char *root=getenv( "DEPOT" );
	if (!root) {
		root = "/";
	}
	fs::path path(root);
	path /= "lib";
	path /= "dtv-util";
	path /= "test";
	path /= "resources";
	path /= "test.log";
	_file = path.string();
}

void LoggerTest::SetUp( void ) {
	fs::remove(_file);
	util::reg::init();
	util::cfg::setValue("log.enabled", true);
	util::cfg::setValue("log.filter.all.all", std::string("info"));
	util::cfg::setValue("log.output.useStdOut", true);
	util::cfg::setValue("log.output.useFile", _file);
}

void LoggerTest::TearDown( void ) {
	util::log::fin();
	util::reg::fin();
	fs::remove(_file);
}

TEST_F(LoggerTest, basic) {
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );
}

TEST_F(LoggerTest, basic_without_category) {
	util::log::init();
	LDEBUG("", "Mensaje de prueba");
	LINFO("", "Mensaje de prueba");
	LWARN("", "Mensaje de prueba");
	LERROR("", "Mensaje de prueba");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );
}

TEST_F(LoggerTest, basic_args) {
	util::log::init();
	doLogArgs("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: A" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: -1" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 172" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1.25" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1.23e+00" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1.23E+00" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1f" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: 1F" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba con args: hola" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );
}

TEST_F(LoggerTest, basic_all) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("all"));
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] TRACE: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] DEBUG: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, basic_trace) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("trace"));
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] TRACE: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] DEBUG: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, basic_debug) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("debug"));
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] DEBUG: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, basic_warn) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("warn"));
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, basic_err) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("error"));
	util::log::init();
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line == "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, basic_none) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("none"));
	util::log::init();
	doLog("test");
	util::log::fin();

	ASSERT_FALSE( fs::exists( _file ));
}

TEST_F(LoggerTest, basic_disabled) {
	util::cfg::get("log").set<bool>("enabled", false);
	util::log::init();
	doLog("test");
	util::log::fin();

	ASSERT_FALSE( fs::exists( _file ));
}

TEST_F(LoggerTest, groups_and_categories) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("error"));
	util::cfg::get("log.filter").addNode("grupo1").addValue<std::string>("all", std::string("warn"));
	util::cfg::get("log.filter.grupo1").addValue<std::string>("categoria2", std::string("info"));
	util::cfg::get("log.filter").addNode("grupo2").addValue<std::string>("all", std::string("all"));

	util::log::init();
	doLog("test");
	doLogGroup( "grupo1", "categoria1");
	doLogGroup( "grupo1", "categoria2");
	doLogGroup( "grupo2", "categoria1");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo1::categoria1] WARNING: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo1::categoria1] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo1::categoria2] Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo1::categoria2] WARNING: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo1::categoria2] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo2::categoria1] TRACE: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo2::categoria1] DEBUG: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo2::categoria1] Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo2::categoria1] WARNING: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_EQ( line, "[grupo2::categoria1] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();

	util::cfg::get("log.filter").removeNode("grupo1");
	util::cfg::get("log.filter").removeNode("grupo2");
}

TEST_F(LoggerTest, flush) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("all"));
	util::log::init();

	doLog("test");
	util::log::flush();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] TRACE: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] DEBUG: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] ERROR: Mensaje de prueba" );

	util::log::fin();

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, all_group_specific_category) {
	util::cfg::get("log.filter.all").set<std::string>("all", std::string("none"));
	util::cfg::get("log.filter.all").addValue<std::string>("test", std::string("all"));

	util::log::init();
	doLog("cat");
	doLog("test");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] TRACE: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] DEBUG: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] WARNING: Mensaje de prueba" );
	std::getline(ifile, line);
	ASSERT_EQ( line, "[util::test] ERROR: Mensaje de prueba" );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

TEST_F(LoggerTest, command_line_debug_must_override_cfg_settings) {
	util::cfg::setValue("log.filter.all.all", std::string("debug"));

	util::log::init("util.test.debug&all.all.info");
	doLog("test");
	doLog("must_log_info");
	util::log::fin();

	std::string line;
	std::ifstream ifile(_file.c_str());
	ASSERT_TRUE( ifile.is_open() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 15) == "Log started at:");

	std::getline(ifile, line);
	ASSERT_STREQ( "[util::test] DEBUG: Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::test] Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::test] WARNING: Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::test] ERROR: Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::must_log_info] Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::must_log_info] WARNING: Mensaje de prueba", line.c_str() );
	std::getline(ifile, line);
	ASSERT_STREQ( "[util::must_log_info] ERROR: Mensaje de prueba", line.c_str() );

	std::getline(ifile, line);
	ASSERT_TRUE( line.substr(0, 13) == "Log ended at:");

	std::getline(ifile, line);
	ASSERT_TRUE( line.empty() );
	ASSERT_TRUE( ifile.eof() );

	ifile.close();
}

#endif
