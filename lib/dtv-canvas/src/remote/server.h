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

#pragma once

#include "../size.h"
#include "../rect.h"
#include <boost/thread/thread.hpp>
#include <vector>

//	Stored in linux: /dev/shm/
#define CANVAS_SHARED_MEMORY "canvas_remote_memory"
#define CANVAS_SHARED_DIRTY  "canvas_remote_dirty"
#define CANVAS_DIRTY_SIZE    (sizeof(canvas::Rect)*1024)

namespace util {
namespace task {
	class Dispatcher;
}
}

namespace canvas {
namespace remote {

class Memory;

class Server {
public:
	Server();
	virtual ~Server();

	bool initialize( const canvas::Size &size, int zIndex );
	void finalize();

	const canvas::Size &size() const;
	virtual void enable( bool val )=0;

protected:
	//	Operations
	virtual bool init( int zIndex );
	virtual void fin();
	virtual void render( const std::vector<canvas::Rect> &dirtyRegions, void *mem, size_t memSize )=0;

	//	Getters
	virtual util::task::Dispatcher *disp() const=0;

	void renderTask();
	void renderThread();
	void cleanIPC();

private:
	canvas::Size _size;
	boost::thread _thread;
	bool _exit;
	Memory *_bitmapMem; // Image for surface
	Memory *_dirtyMem; // Dirty regions
};

}
}
