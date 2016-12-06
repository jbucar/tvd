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

#include <mpegparser/service/extension/application/controller/controller.h>
#include <canvas/types.h>
#include <boost/signals2.hpp>
#include <vector>

namespace util {
	class Settings;
}

namespace tuner {
	namespace app {
		class Spawner;
		class ApplicationExtension;
	}
}

namespace canvas {
	class System;
	class Layer;
}

namespace pvr {
namespace dvb {
	class Tuner;
}
}

namespace zapper {

class Zapper;

namespace mount {
	class Mount;
}

namespace channel {
	class Player;
}

namespace middleware {

class Application;
class Middleware;

namespace ginga {

class Controller : public tuner::app::Controller {
public:
	Controller( Zapper *zapper, pvr::dvb::Tuner *tuner );
	virtual ~Controller();

	//	Zapper initialization
	void init();
	void fin();

	//	Zapper methods
	util::Settings *settings() const;

	//	Application notifications
	virtual void onApplicationAdded( tuner::app::Application *app );
	virtual void onApplicationRemoved( tuner::app::Application *app );
	virtual void onDownloadProgress( tuner::app::Application *app, int step, int total );

	//	Application keys
	virtual void reserveKeys( const tuner::app::ApplicationID &id, const tuner::app::Keys &keys, int priority );

	//	Graphic layer
	virtual bool setupLayer( tuner::app::Application *app, int &width, int &heigth );
	virtual void enableLayer( bool st );

	//	Start/stop application
	virtual void start( tuner::app::RunTask *task );
	virtual void stop( tuner::app::RunTask *task, bool kill );

protected:
	//	Video aux
	virtual tuner::app::PlayerAdapter *createMediaPlayerAdapter() const;
	virtual tuner::app::PlayerAdapter *createElementaryPlayerAdapter( int id ) const;
	virtual tuner::app::PlayerAdapter *createMainPlayerAdapter() const;

	//	Graphic layer aux
	canvas::mode::type calculateResolution( const std::vector<tuner::app::video::mode::type> &modes );

	//	Mount notifications
	void onMountChanged();

	//	Aux applications
	Application *findApplication( tuner::app::Application *app ) const;

	//	Getters
	channel::Player *player() const;
	canvas::Layer *layer() const;
	Middleware *middleware() const;
	mount::Mount *mount() const;
	Zapper *zapper() const;

private:
	Zapper *_zapper;
	pvr::dvb::Tuner *_tuner;
	boost::signals2::connection _cChannelBlocked, _onMountChanged;
	tuner::app::Spawner *_spawner;
	canvas::mode::type _layerMode;
};

}
}
}

