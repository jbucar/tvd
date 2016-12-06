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

#include "option.h"
#include "../cfg.h"
#include <string>

namespace util {
namespace cfg {
namespace cmd {

Option::Option( const std::string &path, const std::string &alias)
	: _path(path), _alias(alias), _sname("") {
	_showDef = true;
}

Option::~Option() {
}

const std::string &Option::property() const {
	return _path;
}

const std::string &Option::alias() const {
	return _alias;
}

const std::string Option::def() const {
	std::string val("");
	if (_showDef) {
		val = cfg::get().defaultAsString(_path);
	}
	return val;
}

char Option::sname() const {
	return _sname[0];
}

bool Option::hasShortName() const {
	return !_sname.empty();
}

const std::string &Option::snamestr() const {
	return _sname;
}

void Option::setsname( char sname ) {
	_sname = std::string(1,sname);
}

void Option::noShowDefault() {
	_showDef = false;
}

void Option::showDefault() {
	_showDef = true;
}

void Option::set( const std::string &str ) {
	if ( cfg::get().type(_path) == typeid( std::string ) || !str.empty()) {
		cfg::get().setStr(_path, str);
	} else if (str.empty()) {
		set();
	}
}

void Option::set() {
	if (cfg::get().type(_path) == typeid( bool )) {
		bool bval = cfg::getValue<bool> (_path);
		cfg::setValue(_path, !bval);
	}
}

const std::string Option::desc() const {
	return cfg::get().desc(_path);
}

bool Option::exists() const {
	return cfg::get().existsValue(_path);
}

const std::string Option::cmdDesc( size_t maxSize /*=0*/ ) const {
	std::string cmddesc = "  ";

	if (hasShortName()) {
		cmddesc += "-" + snamestr() + ", ";
	}
	cmddesc += "--" + alias();

	if (!(def().empty())) {
		std::string defstr = "=";
		defstr += def();
		size_t size = defstr.length() + cmddesc.length();
		if (maxSize) {
			if (maxSize > size) {
				cmddesc += defstr;
			} else {
				cmddesc += "={...}";
			}
		} else {
			cmddesc += def();
		}
	}

	return cmddesc;
}

}
}
}
