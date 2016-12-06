/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-module implementation.

    DTV-module is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-module is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-module.

    DTV-module es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-module se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "tvd.h"
#include "channels.h"
#include "session.h"
#include "player.h"
#include "logoprovider.h"
#include "delegate.h"
#include "tunerdelegate.h"
#include "io.h"
#include "ginga/controller.h"
#include "generated/config.h"
#include <pvr/dvb/time.h>
#include <pvr/dvb/tuner.h>
#include <pvr/logos/service.h>
#include <pvr/player.h>
#include <pvr/pvr.h>
#include <util/settings/settings.h>
#include <util/io/dispatcher.h>
#include <util/cfg/cfg.h>
#include <util/assert.h>
#include <util/fs.h>
#include <util/main.h>
#include <util/log.h>
#include <util/mcr.h>

// NOTE:
// NodeJs c++ modules docs: http://nodejs.org/api/addons.html
// V8 docs: https://developers.google.com/v8/

namespace tvd {

Tvd::Tvd()
{
	_delegate = NULL;
	_areaCode = -1;
	_settings = NULL;
	_pvr = NULL;
	_dispatcher = NULL;
	_channels = NULL;
	_session = NULL;
	_gingaCtrl = NULL;
}

Tvd::~Tvd()
{
}

bool Tvd::load( const v8::Handle<v8::Object> &obj ) {
	_delegate = Delegate::create();
	if (!_delegate) {
		return false;
	}

	//	Setup dispatcher
	if (!initDispatcher()) {
		return false;
	}

	//	Init settings
	if (!initSettings(obj)) {
		return false;
	}

	//	Init pvr
	if (!initPvr(obj)) {
		return false;
	}

	return true;
}

bool Tvd::initSettings( const v8::Handle<v8::Object> &obj ) {
	std::string dbDir;

	{	//	Setup database directory
		if (!check( obj, "dbDir", dbDir )) {
			LWARN("tvd", "Invalid parameters. dbDir must be a string");
			return false;
		}
		if (dbDir.empty()) {
			dbDir = util::fs::stateDir();
		}
		LINFO( "tvd", "Using DB directory=%s", dbDir.c_str() );

		//	Create if not exist
		if (!bfs::exists( dbDir )) {
			bfs::create_directories( dbDir );
		}
		else if (!bfs::is_directory( dbDir )) {
			LERROR( "tvd", "Database path is not a directory: dir=%s", dbDir.c_str() );
			return false;
		}
	}

	{	//	Setup RAM disk
		std::string ramDisk;
		if (!check( obj, "ramDisk", ramDisk )) {
			LWARN("tvd", "Invalid parameters. ramDisk must be a string");
			return false;
		}
		if (ramDisk.empty()) {
			ramDisk = util::fs::make( util::fs::stateDir(), "ramdisk" );
		}

		LINFO( "tvd", "Using RamDisk directory=%s", ramDisk.c_str() );

		//	Cleanup RAM disk
		if (!util::safeCleanDirectory( ramDisk )) {
			LERROR( "tvd", "Cannot clean RAM disk directory" );
			return false;
		}

		_ramDisk = ramDisk;
	}

	{	//	Load settings from db directory
		bfs::path tmp( dbDir );
		tmp /= util::main::name();
		std::string dbFile = tmp.string();

		_settings = util::Settings::create( dbFile, "bdb" );
		if (!_settings->initialize()) {
			DEL(_settings);
			LWARN("tvd", "Could not initialize settings");
			return false;
		}
	}

	return true;
}

bool Tvd::initDispatcher() {
	_dispatcher = new util::io::node::Dispatcher();
	if (!_dispatcher) {
		LERROR("tvd", "Cannot create io dispatcher");
		return false;
	}
	_dispatcher->embedded(true);

	if (!_dispatcher->initialize()) {
		LERROR("tvd", "Cannot initialize io dispatcher");
		DEL(_dispatcher);
		return false;
	}

	return true;
}

bool Tvd::initPvr( const v8::Handle<v8::Object> &obj ) {
	{	//	Create PVR
		std::string file;
		if (obj->Has( _val("channelsDB") )) {
			if (!check( obj, "channelsDB", file )) {
				LWARN("tvd", "Invalid parameters. channelsDB must be a string");
				return false;
			}
		}

		LDEBUG( "tvd", "Create pvr: file=%s", file.c_str() );
		_pvr = new pvr::Pvr( _settings, file );
	}

	if (!check( obj, "country", _country )) {
		LWARN("tvd", "Invalid parameters. country must be a string");
		return false;
	}

	if (!check( obj, "area", _areaCode )) {
		LWARN("tvd", "Invalid parameters. area must be a int");
		return false;
	}

	{	//	Setup time source: use system source
		int offset;
		if (!check( obj, "time_offset", offset )) {
			LWARN("tvd", "Invalid parameters. offset must be a int");
			return false;
		}

		LINFO( "tvd", "Using system time source: offset=%d, country=%s",
			offset, _country.c_str() );
		pvr::time::init( new pvr::Time( offset, _country ) );
	}

	{	//	Add tuner
		std::string prov = "dvb";
		if (obj->Has( _val("provider") )) {
			if (!check( obj, "provider", prov )) {
				LWARN("tvd", "Invalid parameters. provider must be a string");
				return false;
			}
		}

		if (prov == "dummy") {
			util::cfg::setValue(
				"tuner.provider.use",
				"tsdata"
			);
		}

		//	Add default tuner
		addTuner( obj );
	}

	return true;
}

void Tvd::addTuner( const v8::Handle<v8::Object> &obj ) {
	pvr::dvb::Tuner *tuner;

	LINFO("tvd", "Add tuner");

	{	//	Create DVB Tuner
		TunerDelegate *dlg = _delegate->createTuner(this);
		DTV_ASSERT(dlg);
		tuner = new pvr::dvb::Tuner( dlg );
	}

	{	//	Add support for ginga
		_gingaCtrl = new ginga::Controller( this, tuner );

		//	Enable Ginga.ar if necesary
		bool enable=false;
		if (check( obj, "enableGinga", enable )) {
			_gingaCtrl->enable( enable );
		}
	}

	//	Add tuner to pvr
	_pvr->addTuner( tuner );
}

bool Tvd::initialize() {
	LINFO( "tvd", "Initialize " );

	//	Initialize pvr
	if (!_pvr->initialize()) {
		finalize();
		return false;
	}

	//	Alloc default player (necesary for ginga)
	allocPlayer();

	//	Construct channels wrapper
	_channels = new Channels(_pvr->channels());

	//	Construct session wrapper
	_session = new Session(_pvr->session());

	//	Start ginga
	_gingaCtrl->start();

	//	Connect to scan changed signal
	pvr::Pvr::ScanSignal::slot_type fnc = boost::bind(&Tvd::scanChanged,this,_1,_2);
	_cScan = _pvr->onScanChanged().connect(fnc);

	return true;
}

void Tvd::finalize() {
	LINFO( "tvd", "Finalize" );

	//	Free all players
	BOOST_FOREACH( Player *p, _players ) {
		pvr::Player *pp = p->stop();
		_pvr->freePlayer(pp);
	}
	_players.clear();

	_gingaCtrl->stop();
	DEL(_gingaCtrl);

	if (_session) {
		_session->stop();
		_session = NULL;
	}

	if (_channels) {
		_channels->stop();
		_channels = NULL;
	}

	//	Disconnect from scan signal
	_cScan.disconnect();

	//	Finalize pvr
	if (_pvr) {
		_pvr->finalize();
		DEL(_pvr);
	}

	//	Finalize settings
	if (_settings) {
		_settings->finalize();
		DEL(_settings);
	}

	//	Finalize dispatcher
	if (_dispatcher) {
		_dispatcher->finalize();
		DEL(_dispatcher);
	}

	DEL(_delegate);
}

void Tvd::scanChanged( pvr::scan::Type scanState, const pvr::Pvr::ScanNetwork net ) {
	static int currentNetwork=0;
	v8::HandleScope scope(v8::Isolate::GetCurrent());

	LINFO("tvd", "Scan changed: state=%d", scanState );

	v8::Handle<v8::Value> argv[4];
	argv[0] = _val("scanChanged"); // event name
	int argc = 1;
	switch (scanState) {
		case pvr::scan::begin: {
			currentNetwork = 0;
			argv[argc++] = _val("begin");
			break;
		}
		case pvr::scan::network: {
			size_t networks = _pvr->networks();
			int porcentage = floor( (currentNetwork*100)/(double)networks );
			argv[argc++] = _val("network");
			argv[argc++] = _val(net.get().c_str());
			argv[argc++] = _val(porcentage);
			currentNetwork++;
			break;
		}
		case pvr::scan::end: {
			currentNetwork = 0;
			argv[argc++] = _val("end");
			break;
		}
	};

	//	Call
	makeCallback( this, "emit", argc, argv );
}

Player *Tvd::allocPlayer() {
	pvr::Player *p = _pvr->allocPlayer();
	if (!p) {
		LERROR( "tvd", "Cannot alloc a new pvr player" );
		return NULL;
	}

	MediaPlayer *media = _delegate->createMedia();
	if (!media) {
		LERROR( "tvd", "Cannot create a new MediaPlayer" );
		delete p;
		return NULL;
	}

	Player *nPlayer = new Player(p,media);
	_players.push_back(nPlayer);
	return nPlayer;
}

Player *Tvd::defaultPlayer() {
	DTV_ASSERT(!_players.empty());
	return _players[0];
}

void Tvd::enableGinga( bool enable ) {
	LDEBUG( "tvd", "Enable Ginga.ar middleware: enable=%d", enable );
	DTV_ASSERT(_gingaCtrl);
	_gingaCtrl->enable( enable );
}

void Tvd::runApplication( const std::string &tacID ) {
	DTV_ASSERT(_gingaCtrl);
	_gingaCtrl->startApplication( tacID );
}

void Tvd::disableApplications( bool state ) {
	DTV_ASSERT(_gingaCtrl);
	_gingaCtrl->disableApplications( state );
}

util::task::Dispatcher *Tvd::dispatcher() const {
	return _dispatcher->dispatcher();
}

const std::string &Tvd::ramDisk() const {
	return _ramDisk;
}

const std::string &Tvd::country() const {
	return _country;
}

int Tvd::areaCode() const {
	return _areaCode;
}

//	Node implementation
v8::Persistent<v8::Function> Tvd::_constructor;

#define TVD \
	Tvd *tvd = node::ObjectWrap::Unwrap<Tvd>(args.This()); \
	DTV_ASSERT(tvd);

pvr::Pvr *Tvd::pvr(const v8::FunctionCallbackInfo<v8::Value>& args) {
	TVD;
	pvr::Pvr *pvr = tvd->_pvr;
	DTV_ASSERT(pvr);
	return pvr;
}

void Tvd::Init(v8::Handle<v8::Object> exports) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, Tvd::New);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(_val("Tvd"));

	//	Prototype: Initialization
	NODE_SET_PROTOTYPE_METHOD(tpl, "load", Tvd::load);
	NODE_SET_PROTOTYPE_METHOD(tpl, "start", Tvd::start);
	NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Tvd::stop);

	//	Logos
	NODE_SET_PROTOTYPE_METHOD(tpl, "addLogoProvider", Tvd::addLogoProvider);

	//	Prototype: Middleware
	NODE_SET_PROTOTYPE_METHOD(tpl, "enableMiddleware", Tvd::enableMiddleware);
	NODE_SET_PROTOTYPE_METHOD(tpl, "runApplication", Tvd::runApplication);
	NODE_SET_PROTOTYPE_METHOD(tpl, "disableApplications", Tvd::disableApplications);

	//	Prototype: Scanning methods
	NODE_SET_PROTOTYPE_METHOD(tpl, "isScanning", Tvd::isScanning);
	NODE_SET_PROTOTYPE_METHOD(tpl, "startScan", Tvd::startScan);
	NODE_SET_PROTOTYPE_METHOD(tpl, "cancelScan", Tvd::cancelScan);

	//	Prototype: Channels getter
	NODE_SET_PROTOTYPE_METHOD(tpl, "channels", Tvd::channels);

	//	Prototype: Session getter
	NODE_SET_PROTOTYPE_METHOD(tpl, "session", Tvd::session);

	//	Prototype: Player getter
	NODE_SET_PROTOTYPE_METHOD(tpl, "defaultPlayer", Tvd::defaultPlayer);

	_constructor.Reset(isolate, tpl->GetFunction());
	exports->Set(_val("Tvd"),tpl->GetFunction());
}

void Tvd::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	DTV_ASSERT(args.IsConstructCall());

	Tvd* self = new Tvd();
	self->Wrap(args.This());

	args.GetReturnValue().Set(args.This());
}

void Tvd::load( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	LINFO("tvd", "Load");
	if (args.Length() != 1 || !args[0]->IsObject()) {
		LWARN("tvd", "Invalid parameters. You must pass config object");
		args.GetReturnValue().Set(_val(false));
		return;
	}
	v8::Handle<v8::Object> obj = args[0]->ToObject();

	TVD;
	if (!tvd->load(obj)) {
		args.GetReturnValue().Set(_val(false));
		return;
	}

	args.GetReturnValue().Set(_val(true));
}

void Tvd::start( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	LINFO("tvd", "Start");

	//	Initialize PVR
	TVD;
	if (!tvd->initialize()) {
		args.GetReturnValue().Set(_val(false));
		return;
	}

	args.GetReturnValue().Set(_val(true));
}

void Tvd::stop( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	LINFO("tvd", "Stop");

	TVD;
	tvd->finalize();

	args.GetReturnValue().SetUndefined();
}

//	Add logo provider
void Tvd::addLogoProvider( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "Add logo provider");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	if (args.Length() == 1 && args[0]->IsFunction()) {
		v8::Local<v8::Function> fnc = v8::Local<v8::Function>::Cast(args[0]);
		TVD;
		DTV_ASSERT(tvd->_settings);
		pvr(args)->logos()->add( new logos::Provider(tvd->_settings, fnc) );
	}
	args.GetReturnValue().SetUndefined();
}

//	Middleware
void Tvd::enableMiddleware( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "Enable middleware");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	if (args.Length() == 1) {
		bool state;
		if (impl::get( args[0], state )) {
			TVD;
			tvd->enableGinga( state );
		}
	}
	args.GetReturnValue().SetUndefined();
}

void Tvd::runApplication( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "Run application");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	if (args.Length() == 1) {
		std::string appID;
		if (impl::get( args[0], appID )) {
			TVD;
			tvd->runApplication( appID );
		}
	}
	args.GetReturnValue().SetUndefined();
}

void Tvd::disableApplications( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	if (args.Length() == 1) {
		bool state;
		if (impl::get( args[0], state )) {
			LINFO("tvd", "Disable applications: disable=%d", state);
			TVD;
			tvd->disableApplications( state );
		}
	}
	args.GetReturnValue().SetUndefined();
}

//	Tuner
void Tvd::isScanning( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "isScanning");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	bool result=pvr(args)->isScanning();
	args.GetReturnValue().Set(_val(result));
}

void Tvd::startScan( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "startScan");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	pvr(args)->startScan();
	args.GetReturnValue().SetUndefined();
}

void Tvd::cancelScan( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LINFO("tvd", "cancelScan");
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	pvr(args)->cancelScan();
	args.GetReturnValue().SetUndefined();
}

//	Channels getter
void Tvd::channels( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	TVD;
	DTV_ASSERT(tvd->_channels);
	args.GetReturnValue().Set(tvd->_channels->persistent());
}

//	Session getter
void Tvd::session( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	TVD;
	DTV_ASSERT(tvd->_session);
	args.GetReturnValue().Set(tvd->_session->persistent());
}

//	Player constructor
void Tvd::defaultPlayer( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	TVD;
	Player *defaultPlayer = tvd->defaultPlayer();
	DTV_ASSERT(defaultPlayer);
	args.GetReturnValue().Set(defaultPlayer->persistent());
}

//	Node modules
void onExit( void * /*arg*/ ) {
	//	Finalize util
	util::main::fin();
}

void init( v8::Handle<v8::Object> exports ) {
	util::main::init(TVDMODULE_NAME, TVDMODULE_VERSION);
	LINFO("tvdmodule", "Initialize %s(%s)", TVDMODULE_NAME, TVDMODULE_VERSION);

	Tvd::Init(exports);
	Channels::Init();
	Session::Init();
	Player::Init();

	node::AtExit( onExit, NULL );
}

}	//	namespace tvd

//	Export module
NODE_MODULE(tvdmodule, tvd::init);

