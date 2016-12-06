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

#include "tool.h"
#include "cfg/xmlparser.h"
#include "cfg/configregistrator.h"
#include "registrator.h"
#include "log.h"
#include "assert.h"
#include "generated/config.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace util {

namespace impl {

void registerOpt( cfg::cmd::CommandLine &cmd, const std::string &owner, const std::string &prop, bool visible, char sname /*= 0*/) {
	if (sname) {
		cmd.registerOpt(owner + "." + prop, prop, sname);
	} else {
		cmd.registerOpt(owner + "." + prop, prop);
	}
	if (!visible) {
		cmd.noShowDefault( prop );
	}
}

} // end impl namespace

Tool::Tool() {
}

Tool::~Tool() {
}

/**
 * Inicializa la tool.
 * @param name El nombre de la tool.
 * @param desc Una descripción.
 * @param version La versión.
 * @param build El build.
 */
void Tool::initialize( const ToolConfig &cfg ) {
	_name = cfg.name;
	_desc = cfg.description;
	_version = cfg.version;
	_build = cfg.repo_version;
	_config = cfg.config;

	//	Setup util library
	util::main::setup( _name, _version, cfg.is_service );

	//	Custom initialization
	init();
}

void Tool::init() {
}

/**
 * @return El nombre de la herramienta.
 */
const std::string &Tool::name() const {
	return _name;
}

/**
 * @return La versión.
 */
const std::string &Tool::version() const {
	return _version;
}

/**
 * @return El build en el que fue compilado.
 */
const std::string &Tool::build() const {
	return _build;
}

/**
 * @return El la configuracion con la que fue compilado.
 */
const std::string &Tool::config() const {
	return _config;
}

/**
 * @return La descripción de la herramienta.
 */
const std::string &Tool::description() const {
	return _desc;
}

void Tool::setVersion( const std::string &ver ) {
	_version = ver;
}

void Tool::printVersion() const {
	printf("%s version %s\n",
	       name().c_str(), version().c_str() );
}

void Tool::printBuild() const {
	printf("%s build %s\n",
	       name().c_str(), build().c_str() );
}

void Tool::printConfig() const {
	printf("%s\n", config().c_str() );
}

/**
 * Imprime las opciones del árbol de configuración que fueron modificadas por el usuario.
 */
void Tool::pretend() const {
	util::cfg::XmlParser parser;
	std::stringbuf *aBuf = new std::stringbuf;
	std::ostream anOstream(aBuf);
	parser.pretend(anOstream, &cfg::get());
	printf("%s",aBuf->str().c_str());
	delete aBuf;
}

/**
 * Imprime todas las opciones del árbol de configuración con sus valores por defecto.
 */
void Tool::defaults() const {
	util::cfg::XmlParser parser;
	std::stringbuf *aBuf = new std::stringbuf;
	std::ostream anOstream(aBuf);
	parser.defaults(anOstream, &cfg::get());
	printf("%s",aBuf->str().c_str());
	delete aBuf;
}

void Tool::set() const {
	std::vector<std::string> arguments;
	boost::split(arguments, cfg::getValue<std::string> ("tool.set"), boost::is_any_of(":"));
	BOOST_FOREACH( std::string arg, arguments ) {
		if (!arg.empty()) {
			if (arg.find( "=" ) != std::string::npos) {
				std::string val = arg.substr(arg.find("=")+1,arg.length());
				std::string path = arg.substr(0,arg.find("="));
				if (path.find(".") != std::string::npos) {
					std::string node = path.substr(0, path.rfind("."));
					std::string prop = path.substr(path.rfind(".")+1, path.length());
					cfg::get(node).setStr(prop, val);
				} else {
					cfg::get().setStr(path, val);
				}
			} else {
				throw std::runtime_error( "bad format of command line option: Pair of values must contain an equal symbol");
			}
		}
	}
}

void Tool::registerProperties( cfg::cmd::CommandLine &cmd ) {
	cfg::get()
		.addNode("tool")
		.addNode(name());

	impl::registerProp("tool", "help", 'h', "Display this message", false, cmd, false);
	impl::registerProp("tool", "version", 'v', "Display tool version", version(), cmd, false);
	impl::registerProp("tool", "build", 'b', "Display build information", build(), cmd, false);
	impl::registerProp("tool", "config", "Display build configuration", config(), cmd, false);
	impl::registerProp("tool", "debug", "Set the categories to log with their log level", std::string(""), cmd, false);
	impl::registerProp("tool", "defaults", "Show in xml format the configuration default values", false, cmd, false);
	impl::registerProp("tool", "pretend","Show in xml format the configuration pretend values", false, cmd, false);
	impl::registerProp("tool", "set","Sets the specified configuration values", std::string(""), cmd, false);

	//	Hidden
	impl::registerCfg("tool", "name", "The name of the tool", _name, false);
	{
		size_t initialPos = _build.find_last_of( "-" )+1;
		size_t endPos = _build.find_first_of( "\n" );
		impl::registerCfg("tool", "buildVersion", "The tool build version", _build.substr( initialPos, endPos - initialPos ), false);
	}
}

/**
 * Inicia la ejecución de la tool.
 * @param argc cantidad de argumentos.
 * @param argv argumentos.
 * @return 0 si no hubo errores
 */
int Tool::execute( int argc, char *argv[] ) {
	int exitCode=-1;
	cfg::cmd::CommandLine cmd( argc, argv );

 	try {
		util::reg::init();

		//	Create tree config
		registerProperties( cmd );

		//	Load tool configs from xml
		util::main::loadConfigs();

		//	Setup unhandled exception
		setupUnhandledException();

		//	Parse commandline
		exitCode = cmd.parse() ? execute( cmd ) : -1;

		util::reg::fin();

	} catch (const std::exception &e) {
		printf( "[Tool] Catch exception: %s\n", e.what() );
#ifdef NDEBUG
		throw;
#endif
	} catch (...) {
		printf( "[Tool] Catch unkown exception\n" );
#ifdef NDEBUG
		throw;
#endif
	}

	return exitCode;
}

int Tool::execute( cfg::cmd::CommandLine &cmd ) {
	if (cmd.isSet("set")) {
		set();
	}

	if (cmd.isSet("help")) {
		printf("%s allowed options\n%s\n", name().c_str(), cmd.desc().c_str());
	} else if (cmd.isSet("version")) {
		printVersion();
	} else if (cmd.isSet("build")) {
		printBuild();
	} else if (cmd.isSet("config")) {
		printConfig();
	} else if (cmd.isSet("pretend")) {
		pretend();
	} else if (cmd.isSet("defaults")) {
		defaults();
	} else {
		util::log::init(cfg::getValue<std::string>("tool.debug"));

		int exitCode = run( cmd );

		util::log::fin();
		return exitCode;
	}
	return 0;
}

int Tool::run( cfg::cmd::CommandLine & /*cmd*/) {
	return 0;
}

}

