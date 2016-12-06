/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "../dispatcher.h"
#include <queue>
#include <list>

struct ev_loop;
struct ev_async;

namespace util {

namespace task {
	class DispatcherImpl;
}

namespace io {
namespace ev {

namespace impl {
	struct my_timer_ev;
	struct my_io_ev;
}

class Dispatcher : public util::io::Dispatcher {
public:
	Dispatcher();
	virtual ~Dispatcher();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Run/exit io event notification loop
	virtual void run();
	virtual void exit();

	//	Dispatcher
	virtual task::Dispatcher *dispatcher() const;

	//	Timer events
	virtual id::Ident addTimer( int ms, const Callback &callback );
	virtual void stopTimer( id::Ident &id );

	//	IO events
	virtual id::Ident addIO( DESCRIPTOR fd, const Callback &callback );
	virtual void stopIO( id::Ident &id );

	//	Socket events
	virtual id::Ident addSocket( SOCKET sock, const Callback &callback );
	virtual void stopSocket( id::Ident &id );

	//	Aux
	void wakeup();

protected:
	typedef std::list<struct impl::my_io_ev *> ListOfIO;
	typedef std::list<struct impl::my_timer_ev *> ListOfTimer;

	//	Aux tasks
	void onPost();

private:
	task::DispatcherImpl *_tasks;
	struct ev_loop *_loop;
	struct ev_async *_wakeup;
	ListOfIO _io;
	ListOfTimer _timers;
};

}
}
}

