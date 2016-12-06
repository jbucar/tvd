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

#include "process.h"
#include "../util.h"
#include "../../../src/log.h"
#include "../../../src/registrator.h"
#include "../../../src/process/process.h"
#include "../../../src/main.h"
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = boost::filesystem;

void Process::SetUp( void ) {
	util::main::init("util-process-test");
}

void Process::TearDown( void ) {
	util::main::fin();
}

TEST_F( Process, default_constructor ) {
	util::Process proc(util::getTestFile("loop_echo.py"));

	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );
}

TEST_F( Process, constructor ) {
	util::Process proc(util::getTestFile("loop_echo.py"));
	proc.addToEnvironment("VAR","/home/path");
	proc.addToEnvironment("VAR2","/home/path2:/etc/path3");

	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );
}

TEST_F( Process, spawn_kill ) {
	util::Process proc(util::getTestFile("loop_echo.py"));

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );

	proc.kill(0);
	
	ASSERT_FALSE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );
}

TEST_F( Process, spawn_kil_with_timeout ) {
	util::Process proc(util::getTestFile("loop_echo.py"));

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );

	proc.kill(500);
	
	ASSERT_FALSE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );
}

TEST_F( Process, test_isRunning ) {
	util::Process proc(util::getTestFile("spawn.py"));

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );

	//	Wait process to finish
	proc.wait();

	ASSERT_FALSE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );

	ASSERT_TRUE( fs::exists( "ok.spawn" ));
	fs::remove("ok.spawn");
}

TEST_F( Process, spawn_inexistent_file ) {
	util::Process proc(util::getTestFile("wrong_file"));
	ASSERT_FALSE( proc.run() );
}

TEST_F( Process, spawn_with_params_and_env ) {
	util::Process proc(util::getTestFile("test_vars.py"));

	proc.addParam("parameter1");
	proc.addParam("parameter2");
	proc.addToEnvironment("VAR=/home/path");
	proc.addToEnvironment("VAR2=/home/path2:/etc/path3");

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

TEST_F( Process, spawn_with_params_and_env2 ) {
	util::Process proc(util::getTestFile("test_vars.py"));

	proc.addParam("parameter1");
	proc.addParam("parameter2");
	proc.addToEnvironment("VAR", "/home/path");
	proc.addToEnvironment("VAR2", "/home/path2:/etc/path3");

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

TEST_F( Process, spawn_with_params_in_constructor ) {
	Params params;
	params.push_back("parameter1");
	params.push_back("parameter2");

	util::Process proc( util::getTestFile("test_vars.py"), params );
	proc.addToEnvironment("VAR=/home/path");
	proc.addToEnvironment("VAR2=/home/path2:/etc/path3");

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

TEST_F( Process, spawn_with_params_and_env_in_constructor ) {
	Params params;
	params.push_back("parameter1");
	params.push_back("parameter2");
	Environment env;
	env["VAR"] = "/home/path";
	env["VAR2"] = "/home/path2:/etc/path3";

	util::Process proc( util::getTestFile("test_vars.py"), params, env );

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

TEST_F( Process, spawn_with_env_in_constructor ) {
	Environment env;
	env["VAR"] = "/home/path";
	env["VAR2"] = "/home/path2:/etc/path3";

	util::Process proc( util::getTestFile("test_vars.py"), env );
	proc.addParam("parameter1");
	proc.addParam("parameter2");

	ASSERT_TRUE( proc.run() );

	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

TEST_F( Process, spawn_kill_spawn ) {
	util::Process proc(util::getTestFile("test_vars.py"));

	proc.addParam("bla");
	proc.addParam("blabla");
	proc.addToEnvironment("VAR", "blabla");
	proc.addToEnvironment("VAR2", "blablabla");

	ASSERT_TRUE( proc.run() );
	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );
	
	proc.kill();
	ASSERT_FALSE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );

	proc.clearParams();
	proc.clearEnvironment();

	proc.addParam("parameter1");
	proc.addParam("parameter2");
	proc.addToEnvironment("VAR", "/home/path");
	proc.addToEnvironment("VAR2", "/home/path2:/etc/path3");

	ASSERT_TRUE( proc.run() );
	ASSERT_TRUE( proc.pid() != INVALID_PROCESS_ID );
	ASSERT_TRUE( proc.isRunning() );

	proc.wait();

	ASSERT_FALSE( proc.isRunning() );
	ASSERT_TRUE( proc.pid() == INVALID_PROCESS_ID );

	ASSERT_TRUE( fs::exists( "ok.env" ));
	fs::remove("ok.env");
	ASSERT_TRUE( fs::exists( "ok.vars" ));
	fs::remove("ok.vars");
}

