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

#include <node/util.h>
#include <mpegparser/service/extension/application/controller/controller.h>
#include <boost/signals2.hpp>
#include <vector>

namespace pvr {
namespace dvb {
	class Tuner;
}
}

namespace tvd {

class Tvd;

namespace ginga {

class Controller : public tuner::app::Controller {
public:
	Controller( Tvd *tvd, pvr::dvb::Tuner *tuner );
	virtual ~Controller();

	//	Initialization
	void enable( bool st );
	void start();
	void stop();
	void startApplication( const std::string &appID );

	//	Application notifications
	virtual void onApplicationAdded( tuner::app::Application *app );
	virtual void onApplicationRemoved( tuner::app::Application *app );
	virtual void onDownloadProgress( tuner::app::Application *app, int step, int total );

	//	Start/stop application
	virtual void start( tuner::app::RunTask *task );
	virtual void stop( tuner::app::RunTask *task, bool kill );

	//	Video operations
	virtual tuner::app::PlayerAdapter *createMainPlayerAdapter() const;

	//	Impl
	void notify( const std::string &event, const v8::Handle<v8::Object> &evt );
	void notify( tuner::app::Application *gApp, const std::string &event, const v8::Handle<v8::Object> &evt );

private:
	Tvd *_tvd;
	pvr::dvb::Tuner *_tuner;
	boost::signals2::connection _cChannelBlocked;
	bool _enabled;
	bool _isRunning;
};

}
}

