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
#include "mainplayeradapter.h"
#include "../tvd.h"
#include "../player.h"
#include <pvr/dvb/tuner.h>
#include <pvr/player.h>
#include <mpegparser/service/servicemanager.h>
#include <mpegparser/service/extension/application/extension.h>
#include <mpegparser/service/extension/application/application.h>
#include <mpegparser/service/extension/application/controller/runtask.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace tvd {
namespace ginga {

Controller::Controller( Tvd *tvd, pvr::dvb::Tuner *tuner )
	: _tvd(tvd), _tuner(tuner)
{
	DTV_ASSERT(tvd);
	DTV_ASSERT(tuner);
	_enabled = false;
	_isRunning = false;
}

Controller::~Controller()
{
}

void Controller::enable( bool st ) {
	if (_enabled != st) {
		LDEBUG( "tvd", "Enable ginga: enabled=%d, st=%d isRunning=%d", _enabled, st, _isRunning );
		bool oldState = _isRunning;
		stop();
		_enabled = st;
		if (oldState) {
			start();
		}
	}
}

void Controller::start() {
	DTV_ASSERT(!_isRunning);

	if (_enabled) {
		LDEBUG( "tvd", "Start ginga" );

		//	Initialize base class
		initialize();

		//	Setup channel blocked
		_cChannelBlocked = _tvd->defaultPlayer()->player()->onChannelProtected().connect( boost::bind(&tuner::app::Controller::onChannelBlocked,this,_1) );

		//	Enable application extension
		_tuner->attach( extension() );
	}
	_isRunning = true;
}

void Controller::stop() {
	if (_isRunning) {
		if (_enabled) {
			//	Disconnect from player
			_cChannelBlocked.disconnect();

			//	Detach application extension to service manager
			_tuner->detach( extension() );

			//	Finalize base class
			finalize();
		}

		_isRunning = false;
	}
}

void Controller::startApplication( const std::string &tacID ) {
	tuner::app::ApplicationID appID;
	if (_isRunning && _enabled && tuner::app::ApplicationID::makeID(tacID, appID)) {
		extension()->start( appID );
	}
	else {
		LWARN( "tvd", "Invalid application: tacID=%s", tacID.c_str() );
	}
}

//	Virtual methods
void Controller::notify( tuner::app::Application *gApp, const std::string &event, const v8::Handle<v8::Object> &evt ) {
	setValue( evt, "id", gApp->appID().asString() );
	notify( event, evt );
}

void Controller::notify( const std::string &event, const v8::Handle<v8::Object> &evt ) {
	//	Global event data
	setValue( evt, "type", event );

	v8::Handle<v8::Value> argv[2];
	argv[0] = _val("onMiddleware");
	argv[1] = evt;

	//	Call
	makeCallback(_tvd, "emit", 2, argv );
}

void Controller::onApplicationAdded( tuner::app::Application *gApp ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());

	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	{
		v8::Handle<v8::Object> app = v8::Object::New(v8::Isolate::GetCurrent());
		setValue( app, "name", gApp->name() );
		setValue( app, "autoStart", gApp->autoStart() );
		setValue( app, "language", gApp->language().code() );

		if (!gApp->icons().empty()) {
			//	Add icons
			v8::Handle<v8::Object> icons = v8::Object::New(v8::Isolate::GetCurrent());
			BOOST_FOREACH( const std::string &logo, gApp->icons() ) {
				setValue( icons, "name", logo );
			}
			setValue( app, "icons", icons );
		}

		setValue( evt, "app", app );
	}

	notify( gApp, "added", evt );
}

void Controller::onApplicationRemoved( tuner::app::Application *gApp ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	notify( gApp, "removed", evt );
}

void Controller::onDownloadProgress( tuner::app::Application *gApp, int step, int total ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());

	//	Setup progress
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	{
		v8::Handle<v8::Object> o = v8::Object::New(v8::Isolate::GetCurrent());
		setValue( o, "step", step );
		setValue( o, "total", total );

		setValue( evt, "progress", o );
	}

	notify( gApp, "download", evt );
}

//	Launch applications
void Controller::start( tuner::app::RunTask *task ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());

	//	Setup application before start
	task->app()->starting();

	//	Start application
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());

	{	//	Setup parameters
		const tuner::app::RunTask::Params &params = task->params();
		size_t n = params.size();
		v8::Handle<v8::Array> p = v8::Array::New(v8::Isolate::GetCurrent(), n);
		size_t i=0;
		BOOST_FOREACH( const tuner::app::RunTask::Params::value_type &val, params ) {
			std::string param = val.first + "=" + val.second;
			p->Set( i, _val(param) );
			i++;
		}

		setValue( evt, "parameters", p );
	}

	//	Notify upper layer
	notify( task->app(), "start", evt );

	//	Notify application started
	task->app()->started();
}

void Controller::stop( tuner::app::RunTask *task, bool kill ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	v8::Handle<v8::Object> evt = v8::Object::New(v8::Isolate::GetCurrent());
	setValue( evt, "kill", kill );
	notify( task->app(), "stop", evt );
}

//	Video operations
tuner::app::PlayerAdapter *Controller::createMainPlayerAdapter() const {
	return new MainPlayerAdapter( (Controller *)this );
}

}
}

