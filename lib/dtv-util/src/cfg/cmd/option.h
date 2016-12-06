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

#include <string>

namespace util {
namespace cfg {
namespace cmd {

class Option {
public:
	Option( const std::string &prop, const std::string &alias="" );
	virtual ~Option();

	void setsname( char sname );
	void noShowDefault();
	void showDefault();

	// Queries
	char sname() const;
	const std::string &snamestr() const;
	const std::string &property() const;
	const std::string &alias() const;
	const std::string def() const;
	const std::string desc() const;
	const std::string cmdDesc( size_t maxSize=0 ) const;
	bool hasShortName() const;
	bool exists() const;

	virtual void set();
	virtual void set( const std::string &str );

private:
	std::string _path;
	std::string _alias;
	std::string _sname;
	bool _showDef;
};

}
}
}

