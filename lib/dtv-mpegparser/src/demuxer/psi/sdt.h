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

#include "../../service/types.h"
#include "../descriptors.h"
#include <vector>

namespace tuner {

class Sdt {
public:
	typedef struct {
		ID   serviceID;
		bool eitSchedule;
		bool eitPresentFollowing;
		service::status::type status;
		bool caMode;
		desc::Descriptors descriptors;
	} Service;
	
    Sdt( Version version, ID tsID, ID nitID, bool actual, std::vector<Service> &services );
    virtual ~Sdt( void );

    //  Getters
	Version version() const;
	ID tsID() const;
	ID nitID() const;
	bool actual() const;
	const std::vector<Service> &services() const;
	void show() const;

private:
	Version _version;
	ID      _tsID;
	ID      _nitID;
	bool    _actual;
	std::vector<Service> _services;	
};

}
