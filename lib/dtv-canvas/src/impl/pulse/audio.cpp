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

#include "audio.h"
#include "../../system.h"
#include "../../types.h"
#include <util/task/dispatcher.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <pulse/pulseaudio.h>
#include <pulse/version.h>
#include <pulse/thread-mainloop.h>
#include <pulse/ext-stream-restore.h>
#include <pulse/ext-device-manager.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {
namespace pulse {

namespace impl {

//	Enum aux
class EnumSink {
public:
	EnumSink( pa_threaded_mainloop *loop, std::vector<audio::DeviceInfo> &devs ) : _mainloop(loop), _devices(devs)
	{
	}

	void addDevice( const pa_sink_info *i ) {
		if (i && i->name) {
			LTRACE( "pulse", "Sink device: name=%s, index=%d, volume=%d", i->name, i->index, i->volume.values[0] );
			audio::DeviceInfo dev;
			dev.name = std::string(i->name);
			dev.displayName = std::string(i->description);
			dev.displayDescription = (i->active_port && i->active_port->description) ?
				std::string((i->active_port->description)).append(" (PULSEAUDIO)") :
				std::string((i->description)).append(" (PULSEAUDIO)");
			_devices.push_back( dev );
		}
		pa_threaded_mainloop_signal(_mainloop, 0);
	}

private:
	pa_threaded_mainloop *_mainloop;
	std::vector<audio::DeviceInfo> &_devices;
};

struct SinkInfo {
	std::string name;
	pa_volume_t volume;
	int mute;
};

static void enumSinkRequestCallback(pa_context * /*c*/, const pa_sink_info *i, int /*eol*/, void *userdata) {
	DTV_ASSERT(userdata);
	EnumSink *es = (EnumSink *)userdata;
	es->addDevice( i );
}

static void sinkRequestCallback(pa_context * /*c*/, const pa_sink_info *i, int /*eol*/, void *userdata) {
	DTV_ASSERT(userdata);
	SinkInfo *si = (SinkInfo *)userdata;
	if (i && i->name) {
		si->name = i->name;
		si->volume = i->volume.values[0];
		si->mute = i->mute;
	}
}

static void context_state_callback(pa_context *c, void* userdata) {
	pa_threaded_mainloop *m = (pa_threaded_mainloop *)userdata;
	DTV_ASSERT(m);

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_READY:
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		case PA_CONTEXT_TERMINATED:
			pa_threaded_mainloop_signal(m, 0);
			break;
	};
}

static void subscribe_cb( pa_context */*c*/, pa_subscription_event_type_t t, uint32_t index, void *userdata ) {
	LDEBUG( "pulse", "Sink changed: event=%d, index=%d", t, index );

	Audio *audio = static_cast<Audio*>( userdata );
	DTV_ASSERT( audio );

	if (!audio->ignoreNotifications()) {
		if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) != PA_SUBSCRIPTION_EVENT_REMOVE) {
			audio->onNotification();
		}
	} else {
		LDEBUG( "pulse", "Sink changed ignoring..." );
	}
}

}	//	namespace impl

Audio::Audio()
{
	_mainloop = NULL;
	_context = NULL;
	_oldVolume = 0;
	_oldMute = false;
	_ignoreNotifications = true;
}

Audio::~Audio()
{
	DTV_ASSERT(!_context);
	DTV_ASSERT(!_mainloop);
}

//	Initialization
bool Audio::init() {
	LDEBUG("pulse", "Init");

	//	Create and configure context
	if (!setupContext()) {
		return false;
	}

	//	Enumerate devices
	if (!enumDevices()) {
		return false;
	}

	{	//	Register Callback for Sink changes
		system()->dispatcher()->registerTarget( this, "canvas::pulse::Audio" );
		pa_context_set_subscribe_callback( _context, impl::subscribe_cb, this );
		pa_subscription_mask_t mask = PA_SUBSCRIPTION_MASK_SINK;
		pa_operation *op = pa_context_subscribe( _context, mask, NULL, this );
		if (!op) {
			LERROR( "pulse", "Failed to suscribe for sink changes" );
			fin();
			return false;
		}
		pa_operation_unref(op);
	}

	return true;
}

void Audio::fin() {
	LDEBUG("pulse", "Fin");

	system()->dispatcher()->unregisterTarget( this ); //TODO: Fix, eliminar warning del dispatcher

	//	Stop main thread
	if (_mainloop) {
		pa_threaded_mainloop_stop( _mainloop );

		if (_context) {
			pa_context_disconnect(_context);
			pa_context_unref(_context);
			_context = NULL;
		}

		pa_threaded_mainloop_free( _mainloop );
		_mainloop = NULL;
	}
}

bool Audio::open( const std::string &dev ) {
	//	Save old volume
	struct impl::SinkInfo result;
	bool res=waitForOperation(
		pa_context_get_sink_info_by_name( _context, dev.c_str(), &impl::sinkRequestCallback, &result ),
		"Get volume"
	);
	if (res && dev == result.name) {
		_oldVolume = result.volume;
		_oldMute = result.mute == 1;
		_sink = dev;
		_ignoreNotifications = false;
		return true;
	}
	else {
		LERROR( "pulse", "Invalid sink" );
		return false;
	}
}

void Audio::close() {
	DTV_ASSERT(!_sink.empty());
	_ignoreNotifications = true;
	pa_context_subscribe( _context, PA_SUBSCRIPTION_MASK_NULL, NULL, this );

	//	Restore old volume
	setVolume( _oldVolume );
	muteImpl( _oldMute );

	_oldVolume = 0;
	_oldMute = false;
	_sink = "";
}

bool Audio::setupContext() {
	//	Setup main loop
	_mainloop = pa_threaded_mainloop_new();
	if (!_mainloop) {
		LERROR( "pulse", "Failed to create main loop" );
		return false;
	}

	pa_mainloop_api *api = pa_threaded_mainloop_get_api( _mainloop );
	DTV_ASSERT(api);

	//	Setup context
	_context = pa_context_new( api, "dtv-canvas" );
	if (!_context) {
		LERROR( "pulse", "Failed to create a context" );
		pa_threaded_mainloop_free( _mainloop );
		_mainloop = NULL;
		return false;
	}

	//	Setup context state callback
	pa_context_set_state_callback( _context, impl::context_state_callback, _mainloop );
	if (pa_context_connect( _context, NULL, PA_CONTEXT_NOFAIL, NULL ) < 0) {
		LERROR( "pulse", "Failed to connect context" );
		pa_context_unref(_context);
		_context = NULL;
		pa_threaded_mainloop_free( _mainloop );
		_mainloop = NULL;
		return false;
	}

#if PA_MAJOR > 5
	pa_threaded_mainloop_set_name( _mainloop, "pa_main" );
#endif

	//	Lock and start mainloop
	pa_threaded_mainloop_lock(_mainloop);
	if (pa_threaded_mainloop_start( _mainloop ) < 0) {
		LERROR( "pulse", "Failed to connect context" );
		pa_threaded_mainloop_unlock(_mainloop);

		pa_context_unref(_context);
		_context = NULL;

		pa_threaded_mainloop_free( _mainloop );
		_mainloop = NULL;

		return false;
	}

	{	//	Wait until the context is ready
		pa_context_state_t st=pa_context_get_state(_context);
		while (st != PA_CONTEXT_READY && st != PA_CONTEXT_FAILED) {
			LTRACE( "pulse", "Wait for context ready: state=%d", st );
			pa_threaded_mainloop_wait(_mainloop);
			st=pa_context_get_state(_context);
		}
		if (st == PA_CONTEXT_FAILED) {
			LERROR( "pulse", "Wait for context ready failed: state=%d", st );
			pa_threaded_mainloop_unlock(_mainloop);
			fin();
			return false;
		}
	}

	pa_threaded_mainloop_unlock(_mainloop);

	return true;
}

bool Audio::enumDevices() {
	pa_threaded_mainloop_lock(_mainloop);

	std::vector<audio::DeviceInfo> devices;
	impl::EnumSink result(_mainloop,devices);

	bool res=waitForOperation(
		pa_context_get_sink_info_list(_context, impl::enumSinkRequestCallback, &result),
		"Enumerate audio sinks"
	);

	pa_threaded_mainloop_unlock(_mainloop);

	if (res && !devices.empty()) {
		BOOST_FOREACH( const audio::DeviceInfo &dev, devices ) {
			addDevice( dev );
		}
	}
	else {
		res = false;
	}

	return res;
}

bool Audio::supportMultipleDevices() {
    return true;
}

bool Audio::ignoreNotifications() {
	return _ignoreNotifications;
}

void Audio::standby() {
	_ignoreNotifications = true;
	canvas::Audio::standby();
}

void Audio::wakeup() {
	_ignoreNotifications = false;
	canvas::Audio::wakeup();
}

//	Audio control
bool Audio::muteImpl( bool needMute ) {
	LDEBUG( "pulse", "Set audio mute: needMute=%d", needMute );
	DTV_ASSERT(!_sink.empty());
	return waitForOperation(
		pa_context_set_sink_mute_by_name( _context, _sink.c_str(), needMute ? 1 : 0, NULL, NULL ),
		"Set mute"
	);
}

bool Audio::volumeImpl( Volume vol ) {
	DTV_ASSERT(!_sink.empty());
	return setVolume( pa_sw_volume_from_linear( translateVol( vol, 0, 1 ) ) );
}

bool Audio::channelImpl( audio::channel::type /*channel*/ ) {
	return true;
}

bool Audio::setVolume( pa_volume_t vol ) {
	LDEBUG( "pulse", "Set audio volume: vol=%d", vol );

	pa_cvolume volume;
	volume.channels = 1;
	volume.values[0] = vol;

	return waitForOperation(
		pa_context_set_sink_volume_by_name( _context, _sink.c_str(), &volume, NULL, this ),
		"Set volume"
	);
}

void Audio::onNotification() {
	system()->dispatcher()->post( this, boost::bind(&Audio::onDeviceChanged, this) );
}

void Audio::onDeviceChanged() {
	struct impl::SinkInfo result;
	bool res=waitForOperation(
		pa_context_get_sink_info_by_name( _context, _sink.c_str(), &impl::sinkRequestCallback, &result ),
		"Get volume"
	);
	if (res) {
		Volume vol = boost::math::iround( pa_sw_volume_to_linear( result.volume ) * MAX_VOLUME );
		onDeviceUpdated( vol, result.mute );
	}
}

bool Audio::waitForOperation( pa_operation *op, const char *msg/*=NULL*/ ) const {
	DTV_ASSERT(op);

	bool sucess = true;
	while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
		pa_threaded_mainloop_wait(_mainloop);

	if (pa_operation_get_state(op) != PA_OPERATION_DONE) {
		LWARN( "pulse", "Fail to complete operation: %s", msg ? msg : "Unknown" );
		sucess = false;
	}
	pa_operation_unref(op);

	return sucess;
}

}
}

