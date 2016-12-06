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
#include "../time.h"

namespace pvr {
namespace dvb {

Control::Control( util::BYTE content, tuner::desc::parental::age::type age )
	: _content(content), _age(age)
{
}

Control::~Control()
{
}

bool Control::hasSex() const {
	return _content & tuner::desc::parental::content::sex;
}

bool Control::hasViolence() const {
	return _content & tuner::desc::parental::content::violence;
}

bool Control::hasDrugs() const {
	return _content & tuner::desc::parental::content::drugs;
}

int Control::age() const {
	int age = -1;
	switch (_age) {
		case tuner::desc::parental::age::none:
			age = 0;
			break;
		case tuner::desc::parental::age::year10:
			age = 10;
			break;
		case tuner::desc::parental::age::year12:
			age = 12;
			break;
		case tuner::desc::parental::age::year14:
			age = 14;
			break;
		case tuner::desc::parental::age::year16:
			age = 16;
			break;
		case tuner::desc::parental::age::year18:
			age = 18;
			break;
		default:
			age = -1;
			break;
	}
	return age;
}

Control *Control::parse( const tuner::desc::Descriptors &descs ) {
	tuner::desc::parental::age::type age;
	util::BYTE content;

	tuner::desc::parental::parse(
		descs,
		time::clock()->countryCode(),
		age,
		content
	);

	if (age != tuner::desc::parental::age::none) {
		return new Control( content, age );
	}

	return NULL;
}

}
}

