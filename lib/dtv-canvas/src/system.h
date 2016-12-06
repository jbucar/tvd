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

#include "types.h"
#include "size.h"
#include <util/task/types.h>
#include <boost/thread.hpp>
#include <vector>

namespace util {
	class Settings;
	class SettingsDelegate;
}

namespace canvas {

struct CanShutdownEvent {
	typedef bool result_type;
	template<typename InputIterator>
		bool operator()(InputIterator first, InputIterator last) const {
		// If there are no slots to call, shutdown!
		if (first == last)
			return true;

		bool can = true;
		while (first != last) {
			can &= *first;
			++first;
		}

		return can;
	}
};

typedef boost::signals2::signal<bool (void), CanShutdownEvent> OnShutdownSignal;

/**
 * Es la clase principal de la librería dtv-canvas. Encapsula la funcionalidad de manejo de eventos,
 * entrada de teclado, manejo de sockets, ejecución de tareas, manejo de timers y loop principal.
 * A partir de una instancia de System se puede acceder a las instancias de Canvas, Window, Player y WebViewer.
 */
class System {
public:
	System();
	virtual ~System();

	//	Initialization
	bool initialize( Factory *factory, util::Settings *settings=NULL );
	void finalize();

	//	Run/exit system loop
	void run();
	void exit();
	OnShutdownSignal &onShutdown();

	//	Standby/wakeup
	void standby();
	void wakeup();

	//	Dispatcher
	util::task::Dispatcher *dispatcher() const;
	util::io::Dispatcher *io() const;
	void post( util::task::Target target, const util::task::Type &task );

	//	Input
	void addInput( Input *in );

	// Config
	void resetConfig();
	void loadConfig();
	void addDelegate( util::SettingsDelegate *delegate );

	//	Web viewer
	virtual WebViewer *createWebViewer( Surface * );

	//	Getters/Setters
	Screen *screen() const;
	Window *window() const;
	Canvas *canvas() const;
	Player *player() const;
	Audio *audio() const;
	InputManager *input() const;

	//	Instance creation
	static System *create( const std::string &useParam="" );

	//	Implementation
	void dispatchShutdown();
	const boost::thread::id &getThreadId() const;

protected:
	//	Types
	typedef std::vector<Input *> InputList;

	//	Initialization
	virtual bool init();
	virtual void onInitialized();
	virtual void fin();
	virtual void onFinalized();
	void cleanup();

	//	IO dispatcher methods
	virtual util::io::Dispatcher *createIO() const;
	bool setupIO();

	//	Canvas methods
	virtual Canvas *createCanvas() const;
	bool setupCanvas();

	//	Player methods
	virtual Player *createPlayer() const;
	bool setupPlayer();

	//	Screen methods
	virtual Screen *createScreen() const;
	bool setupScreen();

	//	Window methods
	virtual Window *createWindow() const;
	bool setupWindow();

	//	Audio methods
	virtual Audio *createAudio() const;
	bool setupAudio();

private:
	Factory *_factory;
	util::io::Dispatcher *_io;
	Screen *_screen;
	Window *_win;
	Canvas *_canvas;
	Player *_player;
	Audio *_audio;
	InputManager *_input;
	InputList _inputs;
	OnShutdownSignal _onShutdown;
	boost::thread::id _threadId;
	std::vector<util::SettingsDelegate *> _delegates;
};

}
