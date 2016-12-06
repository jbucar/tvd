/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-mpegparser implementation.

    DTV-mpegparser is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-mpegparser is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-mpegparser.

    DTV-mpegparser es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-mpegparser se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "config.h"

namespace tuner {
namespace broadcast {

namespace sat {
namespace polarization {
	enum type { horiz, vert };
}
}

class SatelliteConfig : public Config {
public:
	SatelliteConfig();
	virtual ~SatelliteConfig();

	virtual std::string name() const;
	virtual bool start( Frontend *impl );

	//	Getters
	const std::string &satellite() const;
	int diseqc() const;
	unsigned int frequency() const;
	sat::polarization::type polarization() const;
	unsigned int symbolRate() const;

	template<class Marshall>
	void serialize( Marshall &m ) {
		m.serialize( "satellite", _satellite );
		m.serialize( "diseqc", _diseqc );
		m.serialize( "frequency", _frequency );
		m.serialize( "polarization", _polarization );
		m.serialize( "rate", _symbolRate );
	}

private:
	std::string _satellite;
	int _diseqc;
	unsigned int _frequency;
	std::string _polarization;
	unsigned int _symbolRate;
};

}
}

