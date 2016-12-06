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

#include "main.h"
#include "assert.h"
#include "fs.h"
#include "log.h"
#include "cfg/configregistrator.h"
#include "cfg/xmlparser.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <stdio.h>

#ifdef _DEBUG
#	define DEFAULT_LOG_FILTER "all.all.info"
#else
#	define DEFAULT_LOG_FILTER "all.all.warn"
#endif

namespace util {
namespace main {

namespace bfs = boost::filesystem;

namespace impl {
	static std::string name = "";
	static std::string version = "0.0.0";
	static bool is_service=false;
}

const std::string &name() {
	return impl::name;
}

const std::string &version() {
	return impl::version;
}

bool is_service() {
	return impl::is_service;
}

void setup( const std::string &name, const std::string &version/*="0.0.0" */, bool is_service/*=false*/ ) {
	impl::name = name;
	impl::version = version;
	impl::is_service = is_service;
}

void loadConfigs() {
	std::set<std::string> cfgs;
	std::string filename = impl::name + ".cfg.xml";
	cfgs.insert( filename );
	cfgs.insert( "tool.cfg.xml" );
	std::string homeDir = fs::home();
	cfgs.insert( fs::make(homeDir, "tool.cfg.xml") );
	cfgs.insert( fs::make(homeDir, filename) );

	cfg::XmlParser parser;
	BOOST_FOREACH( std::string xml, cfgs ) {
		if (bfs::exists(xml)) {
			fprintf(stderr, "[main] Load configuration from xml file: %s\n", xml.c_str());
			bfs::path path(xml);
			parser.parse(path.string(), &cfg::get());
		}
	}
}

void init(const std::string &name, const std::string &version/*="0.0.0" */, bool is_service/*=false*/) {
	setup(name, version, is_service);

	//	Calls the callbacks that initializes the libraries
	reg::init();

	//	Load configs from xml
	loadConfigs();

	std::string filter("all.all.");
	if (cfg::get().existsValue("log.filter.all.all")) {
		filter += cfg::getValue<std::string>("log.filter.all.all");
	} else {
		filter += DEFAULT_LOG_FILTER;
	}
	cfg::get().addNode("tool").addValue("debug", "Log filters", filter.c_str());

	setupUnhandledException();

	//	Initialize log system
	log::init(cfg::getValue<std::string>("tool.debug"));
}

void fin() {
	//	Finalize log system
	log::fin();
	//	Calls the callbacks that finalizes the libraries
	reg::fin();
}

}
}
