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

#include "controller.h"
#include "application.h"
#include "player/mediaplayeradapter.h"
#include "player/sbtvdplayeradapter.h"
#include "player/elementaryplayeradapter.h"
#include "../../middleware/middleware.h"
#include "../../mount/mount.h"
#include "../../player.h"
#include "../../zapper.h"
#include <pvr/dvb/tuner.h>
#include <mpegparser/service/extension/application/controller/spawner.h>
#include <mpegparser/service/extension/application/application.h>
#include <mpegparser/service/extension/application/extension.h>
#include <mpegparser/service/servicemanager.h>
#include <canvas/layer/layer.h>
#include <canvas/screen.h>
#include <canvas/inputmanager.h>
#include <canvas/system.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace zapper {
namespace middleware {
namespace ginga {


Controller::Controller( Zapper *zapper, pvr::dvb::Tuner *tuner )
	: _zapper(zapper), _tuner(tuner)
{
	DTV_ASSERT(zapper);
	DTV_ASSERT(tuner);

	_spawner = new tuner::app::Spawner();
	_layerMode = canvas::mode::unknown;
}

Controller::~Controller()
{
	delete _spawner;
}

void Controller::init() {
	//	Initialize base class
	initialize();

	//	Setup channel blocked
	_cChannelBlocked = player()->onChannelProtected().connect( boost::bind(&tuner::app::Controller::onChannelBlocked,this,_1) );

	//	Setup mount changes and scan for applications
	_onMountChanged = mount()->onMount().connect( boost::bind(&Controller::onMountChanged, this) );
	onMountChanged();

	//	Enable application extension
	_tuner->attach( extension() );
}

void Controller::fin() {
	//	Detach application extension to service manager
	_tuner->detach( extension() );

	//	Finalize base class
	finalize();

	_onMountChanged.disconnect();
	_cChannelBlocked.disconnect();
}

//	Start/stop application
void Controller::start( tuner::app::RunTask *task ) {
	_spawner->run( task );
}

void Controller::stop( tuner::app::RunTask *task, bool kill ) {
	_spawner->stop( task, kill );
}

//	Mount notifications
void Controller::onMountChanged() {
	LDEBUG( "Ginga", "Scan applications" );

	//	Remove all scanned applications
	extension()->removeScanned();

	//	Search applications in all mounted paths
	const std::vector<std::string> &points=mount()->mount();
	BOOST_FOREACH( const std::string &point, points) {
		LDEBUG( "Ginga", "Scan applications: path=%s", point.c_str() );
		extension()->scan( point, 5 );
	}
}

struct FindApp {
	FindApp( tuner::app::Application *app ) : _app(app) {}
	bool operator()( middleware::Application *app ) const {	return app->user() == _app; }
private:
	tuner::app::Application *_app;
};

middleware::Application *Controller::findApplication( tuner::app::Application *app ) const {
	return middleware()->findApplication( FindApp(app) );
}

void Controller::onApplicationAdded( tuner::app::Application *gApp ) {
	Application *app = new Application(this,gApp);
	app->user( gApp );
	middleware()->addApplication( app );
}

void Controller::onApplicationRemoved( tuner::app::Application *gApp ) {
	middleware::Application *app = findApplication( gApp );
	if (app) {
		middleware()->rmApplication( app->id() );
	}
}

void Controller::onDownloadProgress( tuner::app::Application *gApp, int step, int total ) {
	middleware::Application *app = findApplication( gApp );
	if (app) {
		middleware()->sendDownloadProgress( app->id(), step, total );
	}
}

//	Keys
void Controller::reserveKeys( const tuner::app::ApplicationID &id, const tuner::app::Keys &keys, int priority ) {
	LDEBUG( "Ginga", "Reserve keys!!" );

	//	Fix priority
	int keyPriority=priority+MED_INPUT_PRIORITY;
	if (keyPriority > HI_INPUT_PRIORITY) {
		keyPriority=HI_INPUT_PRIORITY-1;
	}

	//	Send to input service
	util::key::Keys copy(keys.begin(),keys.end());
	canvas::input::Callback fnc = boost::bind(&tuner::app::Extension::dispatchKey,extension(),id,_1,_2);
	zapper()->sys()->input()->reserveKeys(this, fnc, copy, keyPriority);
}

//	Video aux
tuner::app::PlayerAdapter *Controller::createMediaPlayerAdapter() const {
	return new MediaPlayerAdapter( zapper() );
}

tuner::app::PlayerAdapter *Controller::createElementaryPlayerAdapter( int id ) const {
	return new ElementaryPlayerAdapter( zapper(), id );
}

tuner::app::PlayerAdapter *Controller::createMainPlayerAdapter() const {
	return new SBTVDPlayerAdapter( zapper() );
}

//	Layer aux
struct find_app_mode {
	canvas::mode::type _mode;
	find_app_mode(canvas::mode::type mode) : _mode(mode) {}
	bool operator () ( const canvas::mode::type &m ) const {
		return m == _mode;
	}
};
canvas::mode::type Controller::calculateResolution( const std::vector<tuner::app::video::mode::type> &modes ) {
	bool found=false;
	canvas::mode::type result;
	canvas::mode::type tvOut = zapper()->sys()->screen()->mode();
	canvas::Mode tvOutMode = canvas::mode::get( tvOut );
	std::vector<canvas::mode::type> supported=layer()->supportedModes();
	std::vector<canvas::mode::type> possibles;
	int width, height;

	for (size_t i=0; i<modes.size(); i++) {
		//	Get supported application mode resolution
		tuner::app::video::mode::type m = modes[i];
		switch (m) {
			case tuner::app::video::mode::hd:
				width = 1080;
				height = 720;
				break;
			case tuner::app::video::mode::fullHD:
				width = 1920;
				height = 1080;
				break;
			default:
				width = 720;
				height = 576;
				break;
		}

		//	Check if mode is supported by the zapper
		bool isSupported=false;
		canvas::mode::type sm;
		for (size_t index=0; index<supported.size(); index++) {
			sm = supported[index];
			canvas::Mode smMode = canvas::mode::get( sm );
			if (smMode.width == width && smMode.height == height) {
				LDEBUG( "Ginga", "Mapping application resolution is supported: %d -> %d, width=%d, height=%d",
					m, sm, smMode.width, smMode.height );
				isSupported=true;
				break;
			}
		}
		if (!isSupported) {
			LWARN( "Ginga", "Application resolution mode not supported by the zapper: mode=%d", m );
			continue;
		}

		//	1) Check if mode is the same than tv out resolution
		if (width == tvOutMode.width && height == tvOutMode.height) {
			found=true;
			result=tvOut;
			break;
		}
		else {
			//	2) Collect possibles modes
			possibles.push_back( sm );
		}
	}

	if (!found) {
		canvas::mode::type defaultMode = layer()->defaultMode();
		if (possibles.size()) {
			//	Try find default into list of possibles modes
			find_app_mode finder(defaultMode);
			std::vector<canvas::mode::type>::const_iterator it=std::find_if( possibles.begin(), possibles.end(), finder );
			if (it != possibles.end()) {
				result=(*it);
			}
			else {
				//	Get the first possible mode supported by the zapper
				result=possibles[0];
			}
		}
		else {
			//	Returns default mode
			result=defaultMode;
		}
	}

	return result;
}

bool Controller::setupLayer( tuner::app::Application *app, int &w, int &h ) {
	if (_layerMode == canvas::mode::unknown) {
		//	Get list of supported modes
		std::vector<tuner::app::video::mode::type> modes = app->supportedModes();
		if (modes.empty()) {
			modes.push_back( tuner::app::video::mode::sd );
		}

		LDEBUG( "Ginga", "Setup layer resolution: modes=%d", modes.size() );

		//	Calculate resolution from supported modes
		canvas::mode::type result = calculateResolution( modes );

		//	Setup layer
		layer()->mode( result );
		_layerMode = result;
	}

	if (_layerMode != canvas::mode::unknown) {
		canvas::Mode smMode = canvas::mode::get( _layerMode );
		w = smMode.width;
		h = smMode.height;
		return true;
	}
	return false;
}

void Controller::enableLayer( bool state ) {
	layer()->enable( state );
}

//	Getters
util::Settings *Controller::settings() const {
	return zapper()->settings();
}

channel::Player *Controller::player() const {
	return zapper()->player();
}

canvas::Layer *Controller::layer() const {
	return zapper()->getLayer( layer::middleware );
}

Middleware *Controller::middleware() const {
	return zapper()->middleware();
}

mount::Mount *Controller::mount() const {
	return _zapper->mount();
}

Zapper *Controller::zapper() const {
	return _zapper;
}

}
}
}

