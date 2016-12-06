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

#include "../../src/types.h"
#include "../../src/demuxer/psi/pmt.h"
#include "../../src/demuxer/descriptors.h"
#include <boost/thread.hpp>
#include <gtest/gtest.h>

#define CHECK_TAGS(desc,tags) test::checkTags( desc, tags, sizeof(tags)/sizeof(util::BYTE) )

namespace util {
	class Buffer;
}

namespace tuner {
	class Task;
	class Service;
	class Pmt;
}

namespace test {

std::string getTestRoot();
util::Buffer *getSection( const char *file, util::Buffer *buf=NULL );
void counter( int *count );

inline bool checkTags( const tuner::desc::Descriptors &desc, const util::BYTE *ptr, size_t size ) {
	std::vector<util::BYTE> tags;
	if (desc.getTags( tags ) != size) {
		return false;
	}

	for (size_t i=0; i<size; i++) {
		if (tags[i] != ptr[i]) {
			return false;
		}
	}

	return true;
}

namespace flags {
enum type {
	filter_process,
	task_running,
	on_task,
	connector_ev_srv,
	connector_ev_rcv,
	connector_ev_data
};
}

void clean();
void notify( flags::type flag );
void wait( flags::type flag );
void notifyCounter( int *count );

//	Services
tuner::Pmt *createPmt( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, std::vector<tuner::ElementaryInfo> elems );
void addStream( std::vector<tuner::ElementaryInfo> &elems, util::BYTE type, tuner::ID pid );
void addStream( std::vector<tuner::ElementaryInfo> &elems, util::BYTE type, tuner::ID pid, util::BYTE tag );
void addTypeB( std::vector<tuner::ElementaryInfo> &elems, tuner::ID pid, util::BYTE tag );
tuner::Service *createService( tuner::ID srvID, tuner::ID srvPID, tuner::Pmt *pmt );
tuner::Service *createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG );

}


