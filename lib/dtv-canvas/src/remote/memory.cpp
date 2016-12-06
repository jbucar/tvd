/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "memory.h"
#include "generated/config.h"
#if CANVAS_REMOTE_USE_SHARED
#	include "impl/shared/servermemory.h"
#	include "impl/shared/clientmemory.h"
#endif
#if CANVAS_REMOTE_USE_LOCAL
#	include "impl/local/servermemory.h"
#	include "impl/local/clientmemory.h"
#endif
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

namespace canvas {
namespace remote {

Memory *Memory::createServer( const std::string &name, size_t size ) {
	Memory *mem = NULL;
	std::string use;

#if CANVAS_REMOTE_USE_SHARED
	mem = new shared::ServerMemory( name, size );
	use = "shared";
#elif CANVAS_REMOTE_USE_LOCAL
	mem = new local::ServerMemory( name, size );
	use = "local";
#else
	UNUSED(name);
	UNUSED(size);
#endif

	if (!mem) {
		LERROR("ClientMemory", "No valid implementation specified for creating server memory");
		DTV_ASSERT(mem);
	}

	LINFO("ClientMemory", "Creating server memory: use=%s", use.c_str() );

	return mem;
}

Memory *Memory::createClient( const std::string &name ) {
	Memory *mem = NULL;
	std::string use;

#if CANVAS_REMOTE_USE_SHARED
	mem = new shared::ClientMemory( name );
#elif CANVAS_REMOTE_USE_LOCAL
	mem = new local::ClientMemory( name );
#else
	UNUSED(name);
#endif

	if (!mem) {
		LERROR("Memory", "No valid implementation specified for creating client memory");
		DTV_ASSERT(mem);
	}
	LINFO("ClientMemory", "Creating client memory: use=%s", use.c_str() );

	return mem;
}

Memory::Memory( const std::string &name )
	: _name(name)
{
}

Memory::~Memory()
{
}

bool Memory::initialize() {
	return true;
}

void Memory::finalize() {
}

std::string Memory::name() const {
	return _name;
}

}
}
