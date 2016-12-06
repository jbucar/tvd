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

#include "serviceid.h"
#include "../demuxer/types.h"
#include <boost/function.hpp>
#include <vector>

namespace tuner {

class Pmt;
class Nit;
class Sdt;

/**
 * La clase Service representa un servicio (canal).
 */
class Service {
public:
	Service( const Service &other );
	Service( size_t network, const ServiceID &id, ID pid );
	virtual ~Service();

	//	Network getters
	size_t networkID() const;
	const std::string &networkName() const;

	//	Service getters
	const ServiceID &serviceID() const;
	ID id() const;
	ID pid() const;
	ID pcrPID() const;
	const std::vector<ElementaryInfo> &elements() const;
	const desc::Descriptors &descriptors( service::desc::type descIndex ) const;

	//	SDT/NIT parsed parameters
	service::Type type() const;
	const std::string &provider() const;
	const std::string &name() const;
	service::status::type status() const;
	bool isScrambled() const;

	//	Implementation
	void state( service::state::type );
	service::state::type state() const;

	//	Process of Tables
	bool process( const boost::shared_ptr<Pmt> &pmt );
	bool process( const boost::shared_ptr<Nit> &nit );
	bool process( const boost::shared_ptr<Sdt> &sdt );
	void expire( service::desc::type tableIndex );

	//	Debug
	void show() const;

private:
	//	Network parameters
	size_t _networkID;     //	Provider network ID
	std::string _nitName;  //	Network name

	//	Basic service parameters
	ServiceID _id;         //	TS/NIT/Program ID
	ID _pid;               //	PID of service
	ID _pcrPID;            //	PCR pid
	std::vector<ElementaryInfo> _elements;

	//	Descriptors
	bool process( service::desc::type descIndex, const desc::Descriptors &descriptors );
	desc::Descriptors _descriptors[service::desc::last];

	//	SDT/NIT parameters
	std::string _name;     //	Name of service
	service::Type _type;   //	Type of service
	std::string _provider; //	Name of provider
	service::status::type _status;
	bool _caMode;
	service::state::type _state;

	Service();
};

}
