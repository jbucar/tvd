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

#include "option.h"
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <string>
#include <vector>

namespace util {
namespace cfg {
namespace cmd {


namespace impl {
typedef Option *OptionPtr;
typedef const std::string & ( Option::*Getter )( void ) const;
}

class CommandLine {
public:
	CommandLine( int argc, char *argv[] );
	virtual ~CommandLine();

	bool parse();
	void registerOpt( const std::string &prop, const std::string &alias, char shortName=0 );
	void noShowDefault( const std::string &alias );

	bool isSet( const std::string &name );
	bool isRegistered( const std::string &name );
	bool isRegistered( const char name );

	const std::string desc() const;

protected:
	void doParse();

	void set( Option *opt );

	Option *getOption( const std::string &str );
	Option *searchOpt( std::vector<Option *> vec, const std::string &name, const impl::Getter &getter );
	void checkValues( const std::string &prop, const std::string &alias, char shortName );
	bool isRegistered( const std::string &name, const impl::Getter &getter);

private:
	std::vector<Option *> _opts;
	std::vector<Option *> _setOpts;
	char **_argv;
	int _argc;
};

}
}
}

