/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <util/netlib/types.h>
#include <util/keydefs.h>
#include <boost/signals2.hpp>
#include <vector>

namespace util {
	class Settings;
	namespace task {
		class Dispatcher;
	}
}

namespace canvas {
	class Factory;
	class System;
	class Audio;
	class Layer;
	class LayerManager;
}

namespace pvr {
	class Pvr;
	namespace dvb {
		class Delegate;
	}
}

namespace zapper {

namespace channel {
	class Player;
}

namespace mount {
	class Mount;
}

namespace update {
	class Update;
}

namespace middleware {
	class Middleware;
}

class Time;

/**
 * Es el punto de inicio para que los servicios estén disponibles.
 */
class Zapper {
public:
	explicit Zapper( const std::string &name );
	virtual ~Zapper();

	//	Initialize methods
	bool initialize();
	void finalize();

	//	Start/stop methods
	void start();
	void stop();

	//	Loop method
	void run();
	void exit( int ret=0 );
	int exitCode();

	//	Restore to default configuration
	void resetConfig();

	//	Version
	std::string getVersion() const;
	std::string getPlatformVersion() const;

	//	Notifications
	typedef boost::signals2::signal<void (bool)> StandBySignal;
	StandBySignal &onStandby();

	//	Getters
	const std::string &name() const;
	pvr::Pvr *pvr() const;
	channel::Player *player() const;
	middleware::Middleware *middleware() const;
	mount::Mount *mount() const;
	update::Update *update() const;
	canvas::System *sys() const;
	canvas::Layer *getLayer( layer::type layer ) const;
	util::Settings *settings() const;
	util::netlib::Manager *network() const;
	util::task::Dispatcher *dispatcher() const;
	const std::string &ramDisk() const;
	const std::string &dbDir() const;

	/**
	 * Crea un objeto zapper.
	 * @param use Nombre del zapper.
	 * @return Una instancia de @c Zapper.
	 */
	static Zapper *create( const std::string &use );

protected:
	//	Hardware initialization
	virtual bool initHardware();
	virtual void finHardware();
	virtual bool initCanvas();

	//	Start/Stop
	virtual void onStart();
	virtual void onStop();

	//	Standby
	virtual bool supportStandby() const;
	virtual void doStandby();
	virtual void doWakeup();
	void standby();
	void wakeup();
	void onPowerKey( util::key::type key, bool isUp );

	//	Instance creation
	virtual canvas::Factory *createCanvas() const;
	virtual mount::Mount *createMount() const;
	virtual canvas::LayerManager *createLayer() const;
	virtual pvr::dvb::Delegate *createTunerDelegate() const;
	void finCanvas();
	bool initSettings();
	bool initMount();
	bool initNetwork();
	bool initUpdate();
	bool initMiddleware();
	void addDVB();
	bool initPvr();
	void finPvr();

	//	Enqueue task into zapper thread
	virtual util::key::type translateKey( util::key::type key );

	//	Exit code setting
	void exitCode(int exitCode);

private:
	std::string _name;
	std::string _ramDisk;
	std::string _dbDir;
	canvas::System *_sys;
	util::Settings *_settings;
	mount::Mount *_mount;
	util::netlib::Manager *_netlib;
	update::Update *_update;
	canvas::LayerManager *_layers;
	middleware::Middleware *_middleware;
	pvr::Pvr *_pvr;
	channel::Player *_player;
	int _exitCode;
	bool _initialized;
	bool _standby;
	bool _shutdown;
	StandBySignal _onStandby;
};

}

