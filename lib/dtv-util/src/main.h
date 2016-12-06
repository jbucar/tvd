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

#pragma once

#include <set>
#include <string>
#if defined(WIN32) || defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

namespace util {

struct ToolConfigS {
	std::string name;
	std::string description;
	std::string version;
	std::string repo_version;
	std::string config;
	bool is_service;
};
typedef struct ToolConfigS ToolConfig;

namespace main {

const std::string &name();
const std::string &version();
bool is_service();
int run_tvd_tool( int argc, char *argv[] );

void setup( const std::string &name, const std::string &version="0.0.0", bool is_service=false );
void loadConfigs();

// Initialize util library for users that do not extend tool.
// This function call setup and loadConfigs internally.
void init( const std::string &name, const std::string &version="0.0.0", bool is_service=false );
void fin();

}	//	namespace main
}	//	namespace util

#define DEFINE_TOOL_PROC(cls,prj)	\
	namespace util { namespace main { \
	int run_tvd_tool( int argc, char *argv[] ) { \
		cls obj; \
		util::ToolConfig cfg; \
		cfg.name = prj ## _NAME; \
		cfg.description = prj ## _DESCRIPTION; \
		cfg.version = prj ## _VERSION; \
		cfg.repo_version = prj ## _REPO_VERSION; \
		cfg.config = prj ## _CONFIG; \
		cfg.is_service = prj ## _IS_SERVICE; \
		obj.initialize( cfg ); \
		return obj.execute(argc, argv); \
	}}}

//	Define main entry point
#if defined(ANDROID) || defined(__APPLE__) || defined(__DTV_PPAPI__)
#	define CREATE_TOOL(cls,prj) DEFINE_TOOL_PROC(cls,prj)
#elif defined(WIN32) || defined(_WIN32)
#	define CREATE_TOOL(cls,prj) \
		DEFINE_TOOL_PROC(cls,prj) \
		int main( int argc, char *argv[] ) { return util::main::run_tvd_tool(argc,argv); } \
		int WINAPI WinMain(	HINSTANCE, HINSTANCE, LPTSTR, int ) { main(__argc,__argv); }
#else
#	define CREATE_TOOL(cls,prj) \
		DEFINE_TOOL_PROC(cls,prj) \
		int main( int argc, char *argv[] ) { return util::main::run_tvd_tool(argc,argv); }
#endif

