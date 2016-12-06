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
#include "pmt.h"
#include <boost/foreach.hpp>
#include <util/log.h>

namespace tuner {

Pmt::Pmt( ID pid, Version version, ID programNumber, ID pcrPID, const desc::Descriptors &descriptors, const std::vector<ElementaryInfo> &elements )
{
	_pid         = pid;
    _version     = version;
    _programID   = programNumber;
    _pcrPID      = pcrPID;
    _descriptors = descriptors;
    _elements    = elements;
}

Pmt::~Pmt( void )
{
}

//  Getters
const std::vector<ElementaryInfo> &Pmt::elements() const {
    return _elements;
}

ID Pmt::pid() const {
	return _pid;
}

ID Pmt::programID() const {
	return _programID;
}

ID Pmt::pcrPID() const {
    return _pcrPID;
}

Version Pmt::version() const {
	return _version;
}

const desc::Descriptors &Pmt::descriptors() const {
	return _descriptors;
}

void Pmt::show() const {
	LDEBUG( "Pmt", "PMT: pid=%04x, programID=%04x, ver=%04x, pcrPID=%04x, Descriptors=%s",
		pid(), programID(), version(), pcrPID(), _descriptors.show().c_str() );

	BOOST_FOREACH( ElementaryInfo info, _elements ) {
		LDEBUG( "Pmt", "\telement: type=%02x, pid=%04x, Descriptors=%s",
			info.streamType, info.pid, info.descriptors.show().c_str() );
	}
}

}
