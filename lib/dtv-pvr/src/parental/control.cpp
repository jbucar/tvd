/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

	DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

	DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

	DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

	DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

	Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "control.h"
#include <boost/lexical_cast.hpp>

namespace pvr {
namespace parental {

namespace impl {
	static WithoutControl defaultControl;
}

Control::Control()
{
}

Control::~Control()
{
}

bool Control::operator==( const Control &control ) const {
	return hasSex() == control.hasSex() &&
		hasViolence() == control.hasViolence() &&
		hasDrugs() == control.hasDrugs() &&
		age() == control.age();
}

Control *defaultControl() {
	return &impl::defaultControl;
}

std::string Control::getAgeName() const {
	int age = this->age();
	std::string tmp("Invalid");

	if (age >= 0 && age < 10) {
		tmp = "ATP";
	} else if (age >= 10 && age < 12) {
		tmp = "+10";
	} else if (age >= 12 && age < 14) {
		tmp = "+12";
	} else if (age >= 14 && age < 16) {
		tmp = "+14";
	} else if (age >= 16 && age < 18) {
		tmp = "+16";
	} else if (age >= 18) {
		tmp = "+18";
	}
	else {
		tmp = "+" + boost::lexical_cast<std::string>(age);
	}

	return tmp;
}

std::string Control::getContentName() const {
	std::string tmp;

	if (hasDrugs()) {
		tmp += "D";
	}

	if (hasViolence()) {
		tmp += "V";
	}

	if (hasSex()) {
		tmp += "S";
	}

	return tmp;
}

}
}

