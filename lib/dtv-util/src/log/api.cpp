/*******************************************************************************

  logog is Copyright (c) 2011, Gigantic Software.

********************************************************************************

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

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

#include "statics.h"
#include "message.h"
#include "../log.h"
#include "../string.h"
#include "../cfg/configregistrator.h"
#include "generated/config.h"
#include <stdarg.h>
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

// LOG_LEVELS: none, error, warn, info, debug, trace, all
REGISTER_INIT_CONFIG( log ) {
#ifdef _DEBUG
	std::string level = "info";
#else
	std::string level = "warn";
#endif

	util::cfg::PropertyNode &logNode = root().addNode("log");
	logNode.addValue("enabled", "Logging is enabled/disabled", true);
	logNode.addNode("filter").addNode("all").addValue("all", "Level to log for all categories in this group", level );

	{	//	Output node
		util::cfg::PropertyNode &outNode = logNode.addNode("output");
		outNode.addValue("useStdOut", "Log to stdout", true );
		outNode.addValue("useStdErr", "Log to stderr", false );
		outNode.addValue("useFile", "Log to file", "" );
	}
}

namespace util {
namespace log {

static bool _logEnabled=false;

static void initialize( const std::string &level ) {
	Statics::initStatics( getLevelValue(level) );
	_logEnabled = cfg::getValue<bool>("log.enabled");
}

static void finalize( ) {
	_logEnabled = false;
	Statics::finStatics();
}

static void addOutputs() {
	{	//	stdout output
		bool useStdout = util::cfg::getValue<bool>("log.output.useStdOut");
		if (useStdout) {
			Statics::addTarget(new Cout(stdout));
		}
	}

	{	//	stderr output
		bool useStderr = util::cfg::getValue<bool>("log.output.useStdErr");
		if (useStderr) {
			Statics::addTarget(new Cout(stderr));
		}
	}

	{	//	file output
		const std::string &useFile = util::cfg::getValue<std::string>("log.output.useFile");
		if (!useFile.empty()) {
			Statics::addTarget(new LogFile(useFile));
		}
	}
}

typedef std::pair<std::string, std::string> Category;
struct Visitor {
	std::map<std::string, std::string> _categories;
	void operator()( cfg::PropertyNode *p ) {
		p->visitValues(*this);
		BOOST_FOREACH(  Category c, _categories ) {
			setLevel(p->name().c_str(), c.first.c_str(), c.second.c_str());
		}
		_categories.clear();
	}
	void operator()( cfg::PropertyValue *v ) {
		_categories[v->name()] = v->get<std::string>();
	}
};

static void loadLevels() {
	Visitor nodeVisitor;

	cfg::PropertyNode &groups = cfg::get(std::string("log.filter"));
	groups.visitNodes(nodeVisitor);
}

static const std::string &makeDescription( std::string &group, std::string &category ) {
	std::string *desc = new std::string("Level to log for ");
	if (category == "all") {
		(*desc) += "all categories in ";
		(*desc) += group;
		(*desc) += " group";
	} else {
		(*desc) += category;
		(*desc) += " category in  ";
		(*desc) += group;
		(*desc) += " group";
	}
	return (*desc);
}

static void setup( const std::string &args ) {
	std::vector<std::string> arguments;
	boost::split(arguments, args, boost::is_any_of("&"));
	BOOST_FOREACH( std::string arg, arguments ) {
		if (!arg.empty()) {
			bool badFormat = true;
			std::vector<std::string> tokens;
			boost::split(tokens, arg, boost::is_any_of("."));
			if (tokens.size() == 3) {
				std::string group = tokens[0];
				std::string category = tokens[1];
				std::string level = tokens[2];
				badFormat = group.empty() || category.empty() || level.empty();
				if (!badFormat) {
					if (!cfg::get("log.filter").existsNode(group)) {
						cfg::PropertyNode &node = cfg::get("log.filter").addNode(group);
						node.addValue(category, makeDescription(group, category), "");
					}
					if (!cfg::get(std::string("log.filter.")+group).existsValue(category)) {
						cfg::get(std::string("log.filter.")+group).addValue(category, makeDescription(group, category), "");
					}
					cfg::get(std::string("log.filter.")+group).set<std::string>(category, level);
				}
			}
			if (badFormat) {
				throw std::runtime_error( "bad format of command line debug option, format is: [group].[category].[level]");
			}
		}
	}
}

void init( const std::string &config ) {
	_logEnabled = cfg::getValue<bool>("log.enabled");
	if (_logEnabled) {
		setup( config );
		initialize( cfg::getValue<std::string>("log.filter.all.all") );
		addOutputs();
		loadLevels();
	}
}

void flush() {
	if (_logEnabled) {
		Statics::flushMessages();
	}
}

void fin() {
	if (_logEnabled) {
		finalize();
	}
}

void setLevel( const char *group, const char *category, const char *level ) {
	Statics::setLevel( group, category, getLevelValue(level) );
}

bool canLog( LOG_LEVEL_TYPE level, const char *group, const char *category ) {
	return _logEnabled && Statics::canLog(level, group, category);
}

void log( LOG_LEVEL_TYPE level, const char *group, const char *category, const char *format, ... ) {
	va_list args;
	va_start(args, format);
	std::string formatedMsg = format_va( format, args );
	va_end( args );

	Message msg(level, group, category, formatedMsg);
	Statics::transmitMessage(msg);
}

}
}

