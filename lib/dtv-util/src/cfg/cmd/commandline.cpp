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

#include "commandline.h"
#include "../cfg.h"
#include "../../log.h"
#include "../../string.h"
#include "../../assert.h"
#include <stdexcept>
#include <stdio.h>

#define MIN_OPT_DESC 55

namespace util {
namespace cfg {
namespace cmd {
namespace impl {

struct Finder {
	std::string _name;
	Getter _getter;
	Finder( const std::string &name, const Getter &getter ) : _name(name) {
		_getter = getter;
	}
	bool operator()( const OptionPtr &opt ) const {
		return boost::bind(_getter, opt) () == _name;
	}
};

static inline std::vector<OptionPtr>::iterator find( std::vector<OptionPtr> &opts, const std::string &name, const Getter &getter ) {
	return std::find_if( opts.begin(), opts.end(), Finder(name, getter) );
}

}

CommandLine::CommandLine( int argc, char *argv[] ) {
	_argc = argc;
	_argv = argv;
}

CommandLine::~CommandLine() {
	CLEAN_ALL(Option *, _opts);
}

bool CommandLine::parse() {
	bool result = true;
	try {
		doParse();
	} catch (const std::runtime_error & e) {
		printf("%s \n", e.what());
		result = false;
	}
	return result;
}

void CommandLine::doParse() {
	int cur = 1;
	Option *opt;
	while (cur < _argc) {
		DTV_ASSERT(_argv[cur]);
		std::string str(_argv[cur]);
		if (!( str[0] == '-' )) {
			throw std::runtime_error( "bad format of command line option: Options must begin with single or double hyphen " );
		}
		opt = getOption(str.substr(0, str.find( "=" )));
		if (opt) {
			set(opt);
			if (( str[1] == '-' ) && ( str.find( "=" ) != std::string::npos )) {
				opt->set(str.substr(str.find( "=" )+1));
			} else {
				//	Parse argument if any.
				if (( cur + 1 < _argc ) && (( *_argv[cur + 1] ) != '-' )) {
					opt->set(_argv[cur+1]);
					cur++;
				} else {
					opt->set();
				}
			}
		} else {
			throw std::runtime_error( "It is not an option known: " + str.substr(0, str.find( "=" )));
		}
		cur++;
	}
}

Option *CommandLine::getOption( const std::string &name ) {
	Option *opt = NULL;
	if (name[1] == '-') {
		opt = searchOpt(_opts, name.substr(2,name.length()),&Option::alias);
	} else {
		opt = searchOpt(_opts, name.substr(1,name.length()),&Option::snamestr);
	}
	return opt;
}

Option *CommandLine::searchOpt( std::vector<Option *> vec, const std::string &name, const impl::Getter &getter ) {
	Option *opt = NULL;
	std::vector<Option *>::iterator it = impl::find(vec, name, getter);
	if (it != vec.end()) {
		opt = ( *it );
	}
	return opt;
}

void CommandLine::set( Option *opt ) {
	_setOpts.push_back( opt );
}

void CommandLine::registerOpt( const std::string &prop, const std::string &alias, char shortName ) {
	checkValues(prop, alias, shortName);
	Option *opt = new Option(prop,alias);
	if (opt->exists()) {
		if (shortName) {
			opt->setsname(shortName);
		}
		_opts.push_back(opt);
	} else {
		delete opt;
		throw std::runtime_error("The option is not a config value: " + opt->property());
	}
}

void CommandLine::noShowDefault( const std::string &alias ) {
	Option *opt = searchOpt(_opts, alias, &Option::alias);
	if (opt) {
		opt->noShowDefault();
	} else {
		throw std::runtime_error("The property is not registered: " + alias);
	}
}

void CommandLine::checkValues( const std::string &prop, const std::string &alias, char shortName ) {
	std::string sname(1,shortName);
	if (isRegistered(prop, &Option::property)) {
		throw std::runtime_error("The property is already registered: " + prop);
	} else if (isRegistered(alias, &Option::alias)) {
		throw std::runtime_error("The option's name is already is use: " + alias);
	} else if (isRegistered(sname, &Option::snamestr)) {
		throw std::runtime_error("The short name is already is use: " + sname);
	}
}

bool CommandLine::isRegistered( const std::string &name, const impl::Getter &getter ) {
	return ( searchOpt(_opts, name, getter) != NULL );
}

bool CommandLine::isRegistered( const std::string &name) {
	return isRegistered( name, &Option::alias );
}

bool CommandLine::isRegistered( const char name ) {
	return isRegistered( std::string(1,name), &Option::snamestr );
}

bool CommandLine::isSet( const std::string &name) {
	return ( searchOpt(_setOpts, name, &Option::alias) != NULL );
}

const std::string CommandLine::desc() const {
	std::string result;
	size_t maxLine = MIN_OPT_DESC;

	BOOST_FOREACH( Option *opt, _opts ) {
		// Get the max description
		size_t len = opt->cmdDesc(1).length();
		if (maxLine < len) {
			maxLine = len;
		}
	}

	BOOST_FOREACH( Option *opt, _opts ) {
		std::string aux = format("%%-%ds", maxLine + 8);
		aux = format(aux.c_str(), opt->cmdDesc(maxLine).c_str());
		result += aux;
		result += opt->desc().c_str();
		result += "\n";
	}
	return result;
}

}
}
}
