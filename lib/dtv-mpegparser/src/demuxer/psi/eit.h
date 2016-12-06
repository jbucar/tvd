/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "mjd.h"
#include "../descriptors.h"
#include <vector>

namespace tuner {

namespace eit {
	//  Types
	struct Event {
		BYTE segment;
		ID eventID;
		MJDate startTime;
		MJDate duration;
		BYTE status;
		bool caMode;
		desc::Descriptors descs;
	};
	typedef std::vector<struct Event *> Events;
}

class Eit {
public:
	Eit( Version version, ID tsID, ID nitID, ID serviceID, ID tableID, ID lastTableID, eit::Events &events );
	virtual ~Eit( void );

	//  Getters
	Version version() const;
	ID tableID() const;
	ID serviceID() const;
	ID tsID() const;
	ID nitID() const;
	bool isActual() const;
	bool isPresentFollowing() const;
	ID lastTableID() const;
	const eit::Events &events() const;
	void show() const;

private:
	Version _version;
	ID _tableID;
	ID _lastTableID;
	ID _serviceID;
	ID _tsID;
	ID _nitID;
	eit::Events _events;
};

}
