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

#include "../frontend.h"
#include <linux/dvb/frontend.h>

namespace tuner {
namespace dvb {

class Frontend : public broadcast::Frontend {
public:
	Frontend( const std::string &frontend );
	virtual ~Frontend();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Status/signal
	virtual bool getStatus( status::Type &st ) const;

protected:
	//	Set network
	virtual bool start( broadcast::IsdbtConfig *ch );
	virtual bool start( broadcast::SatelliteConfig *ch );

	bool set_v5( int freq );
	bool set_v3( int freq );
	bool getSignal_v5( status::Type &st ) const;
	bool getSignal_v3( status::Type &st ) const;
	util::BYTE toPorcent( int value, int min, int max ) const;
	unsigned int setupLNB( broadcast::SatelliteConfig *cfg );

private:
	std::string _frontend;
	fe_caps_t _caps;
	int _frontendFD;
	bool _useSet_v5;
	bool _useStats_v5;
	int _diseqcTimeout;
};

}
}
