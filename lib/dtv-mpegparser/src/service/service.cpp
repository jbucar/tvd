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
#include "service.h"
#include "../demuxer/ts.h"
#include "../demuxer/descriptors.h"
#include "../demuxer/descriptors/demuxers.h"
#include "../demuxer/psi/psi.h"
#include "../demuxer/psi/nit.h"
#include "../demuxer/psi/sdt.h"
#include "../demuxer/psi/pmt.h"
#include "generated/config.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/string.h>
#include <util/cfg/configregistrator.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

REGISTER_INIT_CONFIG( tuner_service ) {
	root().addNode("service").addNode("ginga-ncl")
		.addValue("process", "Ginga process", std::string(GINGA_DEFAULT_PROCESS))
		.addValue("timeout", "Ginga timeout", 5000 );
}

namespace tuner {

/**
 * Constructor base.
 */
Service::Service()
{
	_networkID = size_t(-1);

	_pid       = TS_PID_NULL;
	_pcrPID    = TS_PID_NULL;

	_type      = service::type::dtv;
	_status    = service::status::undefined;
	_caMode    = false;
	_state     = service::state::present;
}

/**
 * Constructor de un servicio transmitido en la red indicada por @b networkID.
 * @param networkID Red en la cual se encuentra el servicio.
 * @param id Identificador del servicio.
 * @param pid Identificador del TS.
 */
Service::Service( size_t networkID, const ServiceID &id, ID pid )
	: _networkID(networkID), _id(id)
{
	_pid    = pid;
	_pcrPID = TS_PID_NULL;

	_type   = service::type::dtv;
	_status = service::status::undefined;
	_caMode = false;

	_state  = service::state::present;
}

/**
 * Constructor de copia.
 * @param other Instancia de @c Service a copiar
 */
Service::Service( const Service &other )
	: _networkID(other._networkID), _nitName(other._nitName), _id(other._id), _pid(other._pid), _pcrPID(other._pcrPID), _elements(other._elements)
{
	_name     = other._name;
	_type     = other._type;
	_provider = other._provider;
	_status   = other._status;
	_caMode   = other._caMode;
	_state    = other._state;

	for (size_t i=0; i<service::desc::last; i++) {
		_descriptors[i] = other._descriptors[i];
	}
}

/**
 * Destructor base.
 */
Service::~Service()
{
}

/**
 * @return El identificador de red.
 */
size_t Service::networkID() const {
	return _networkID;
}

/**
 * @return Los identificadores del TS, NIT y Program.
 */
const ServiceID &Service::serviceID() const {
	return _id;
}

/**
 * @return El identificador del programa.
 */
ID Service::id() const {
	return _id.programID;
}

/**
 * @return El id del servicio.
 */
ID Service::pid() const {
	return _pid;
}

/**
 * @return El tipo del servicio (digital_television, one_seg, etc).
 */
service::Type Service::type() const {
	return _type;
}

/**
 * @return El nombre del proveedor del servicio.
 */
const std::string &Service::provider() const {
	return _provider;
}

/**
 * @return El nombre del servicio.
 */
const std::string &Service::name() const {
	return _name;
}

/**
 * @return El nombre de la red en la cual se transmite el servicio.
 */
const std::string &Service::networkName() const {
	return _nitName;
}

/**
 * @return Una colección con información sobre los ES (Elementary Streams) del servicio.
 */
const std::vector<ElementaryInfo> &Service::elements() const {
	return _elements;
}

/**
 * @return El identificador del PCR.
 */
ID Service::pcrPID() const {
	return _pcrPID;
}

/**
 * @return Si el servicio esta encriptado
 */
bool Service::isScrambled() const {
	return _caMode;
}

/**
 * @return El estatus del servicio (undefined, not_running, starting, pausing, running).
 */
service::status::type Service::status() const {
	return _status;
}

/**
 * @return Una colección de descriptores correspondientes al servicio.
 */
const desc::Descriptors &Service::descriptors( service::desc::type descIndex ) const {
	DTV_ASSERT( descIndex < service::desc::last );
	return _descriptors[descIndex];
}

/**
 * @return El estado del servicio (present, complete, expired, timeout, ready, running).
 */
service::state::type Service::state() const {
	return _state;
}

/**
 * Setea el estado del servicio.
 * @param st El estado a setear.
 */
void Service::state( service::state::type st ) {
	_state = st;
}

/**
 * Muestra en el log información descriptiva del servicio.
 */
void Service::show() const {
	LINFO("Service", "Service: id=(%s), type=%02x, name=%s, provider=%s", _id.asString().c_str(), _type, _name.c_str(), _provider.c_str());
	LINFO("Service", "\t pid=%x, pcrPID=%x, elements=%d", _pid, _pcrPID, _elements.size());
	BOOST_FOREACH( const ElementaryInfo &info, _elements ) {
		LINFO( "Service", "\t\ttype=%02x, pid=%04x, Descriptors=%s",
			info.streamType, info.pid, info.descriptors.show().c_str() );
	}
	LINFO("Service", "\t Network: name=%s", _nitName.c_str());
	LINFO("Service", "\t EIT flags(status=%x, caMode=%d)", _status, _caMode);
}

//	Process tables
/**
 * Procesa la @b pmt y en caso de corresponder al servicio, actualiza la información del servicio.
 * @param pmt Tabla con información sobre un programa.
 * @return true si hubo algún cambio en la información del servicio, false en caso contrario.
 */
bool Service::process( const boost::shared_ptr<Pmt> &pmt ) {
	DTV_ASSERT( pmt->programID() == _id.programID );
	DTV_ASSERT( pmt->pid() == _pid );

	_pcrPID   = pmt->pcrPID();
	_elements = pmt->elements();
	_state    = service::state::complete;
	process( service::desc::pmt, pmt->descriptors() );

	return true;
}

/**
 * Procesa la @b nit y en caso de corresponder a la red actual, actualiza la información del servicio.
 * @param nit Tabla con información de una red.
 * @return true si hubo algún cambio en la información del servicio, false en caso contrario.
 */
bool Service::process( const boost::shared_ptr<Nit> &nit ) {
	bool changed = false;

	if (nit->actual()) {
		//	Save nitID
		_id.nitID = nit->networkID();

		//	Process NIT network descriptors
		changed |= process( service::desc::nit, nit->descriptors() );

		//	Process NIT networks
		BOOST_FOREACH( Nit::ElementaryInfo info, nit->elements() ) {
			//	Process NIT service descriptor
			changed |= process( service::desc::nit, info.descriptors );
		}
	}

	return changed;
}

void Service::expire( service::desc::type tableIndex ) {
	if (tableIndex < service::desc::last) {
		//	Remove descriptors
		_descriptors[tableIndex].clear();

		if (tableIndex == service::desc::pmt) {
			_pcrPID = TS_PID_NULL;
			_elements.clear();
		}
		else if (tableIndex == service::desc::nit) {
			_id.nitID = NIT_ID_RESERVED;
		}
		else if (tableIndex == service::desc::sdt) {
			_status = service::status::undefined;
			_caMode = false;
		}
	}
}

/**
 * Procesa la @b sdt y en caso de corresponder al TS actual, actualiza la información del servicio.
 * @param sdt Tabla con información sobre un servicio.
 * @return true si hubo algún cambio en la información del servicio, false en caso contrario.
 */
bool Service::process( const boost::shared_ptr<Sdt> &sdt ) {
	bool changed = false;

	//	SDT is for actual network?
	if (sdt->actual()) {
		//	Process SDT services
		BOOST_FOREACH( Sdt::Service srv, sdt->services() ) {
			if (srv.serviceID == _id.programID) {
				//	Update information
				_status = srv.status;
				_caMode |= srv.caMode;

				//	Process descriptors!
				changed |= process( service::desc::sdt, srv.descriptors );
			}
		}
	}

	return changed;
}

/**
 * Procesa los descriptores contenidos en la colección @b descriptors, actualizando los descriptores del servicio según corresponda.
 * @param descriptors Colección de descriptores.
 * @return true si se actualizó algún descriptor del servicio, false en caso contrario.
 */
bool Service::process( service::desc::type descIndex, const desc::Descriptors &descriptors ) {
	bool changed = false;

	if (descIndex == service::desc::nit) {
		//	Process Service List Descriptor (0x41)
		desc::ServiceListDescriptor serviceList;
		if (DESC_PARSE( descriptors, service_list, serviceList )) {
			BOOST_FOREACH( desc::Service srv, serviceList ) {
				if (srv.first == _id.programID) {
					_type = (_type == service::type::dtv) ? srv.second : _type;
					changed = true;
				}
			}
		}
	}

	if (descIndex == service::desc::sdt) {
		//	Process Service Descriptor (0x48)
		desc::ServiceDescriptor srvDesc;
		if (DESC_PARSE( descriptors, service, srvDesc )) {
			_type = (_type == service::type::dtv) ? srvDesc.type : _type;
			_name     = srvDesc.name;
			_provider = srvDesc.provider;
			changed = true;
		}
	}

	//	Save descriptors
	_descriptors[descIndex] += descriptors;

	return changed;
}

}
