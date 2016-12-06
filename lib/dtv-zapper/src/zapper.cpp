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

#include "generated/config.h"
#include "zapper.h"
#include "player.h"
#include "logos/networkprovider.h"
#include "middleware/middleware.h"
#include "mount/mount.h"
#if ZAPPER_MOUNT_USE_MDEV
#	include "mount/mdev/mount.h"
#endif
#include "update/update.h"
#include "update/network/provider.h"
#include "dvb/update/provider.h"
#include "dvb/ginga/provider.h"
#include "dvb/delegate.h"
#include <pvr/dvb/time.h>
#include <pvr/dvb/tuner.h>
#include <pvr/logos/service.h>
#include <pvr/parental/session.h>
#include <pvr/pvr.h>
#include <canvas/inputmanager.h>
#include <canvas/audio.h>
#include <canvas/layer/layermanager.h>
#include <canvas/layer/surface.h>
#include <canvas/settings/layer.h>
#include <canvas/window.h>
#include <canvas/screen.h>
#include <canvas/remote/layer.h>
#include <canvas/system.h>
#include <canvas/factory.h>
#include <mpegparser/service/extension/time/timeextension.h>
#include <util/netlib/manager.h>
#include <util/settings/settings.h>
#include <util/cfg/cfg.h>
#include <util/cfg/configregistrator.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/string.h>
#include <util/main.h>
#include <util/fs.h>
#include <boost/filesystem.hpp>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

namespace zapper {

namespace fs = boost::filesystem;

namespace impl {

static std::string getDefaultPlatformVersion() {
	std::string version("default_00");
	std::ifstream file (PLATFORM_VERSION_FILE);
	if (file.is_open()){
		getline( file, version );
		file.close();
	}
	return version;
}

}

REGISTER_INIT_CONFIG( zapper ) {
	util::cfg::PropertyNode &node = root().addNode( "zapper" );
	node.addValue( "countryCode", "ISO country code", "ARG" );
	node.addValue( "areaCode", "Area code", -1 );
	node.addValue( "canShutdown", "Set if zapper can shutdown", CAN_SHUTDOWN ? true : false );
	node.addValue( "ramDisk", "RAM disk path", "" );
	node.addValue( "db", "Database path", "" );
	node.addValue( "channelsDB", "File to save channels or empty if binary", "" );
	node.addValue( "imagesPath", "Images path", "" );
	node.addValue( "platformVersion", "Zapper platform version string", impl::getDefaultPlatformVersion() );

	{	//	Add update node
		util::cfg::PropertyNode &update = node.addNode( "update" );
		update.addValue( "useOTA", "Enable Over the Air updates", true );
		update.addValue( "useNetwork", "Enable network updates", true );
	}

	{	//	Add logos node
#if ZAPPER_LOGOS_USE_TS || ZAPPER_LOGOS_USE_NETWORK
		util::cfg::PropertyNode &logos = node.addNode( "logos" );
#endif
#if ZAPPER_LOGOS_USE_TS
		logos.addValue( "useOTA", "Enable Over the Air logos", true );
#endif
#if ZAPPER_LOGOS_USE_NETWORK
		logos.addValue( "useNetwork", "Enable network logos", true );
#endif
	}

	{	//	Add dvb node
		util::cfg::PropertyNode &mpeg = node.addNode( "mpeg" );
		mpeg.addValue( "spec", "MPEG specification to use", "ABNT" );
		mpeg.addValue( "useGinga", "Enable Ginga middleware", true );
		mpeg.addValue( "useEPG", "Enable EPG", true );
		mpeg.addValue( "useEWBS", "Enable EWBS", true );
		mpeg.addValue( "player", "MPEG player to use", MPEG_PLAYER );
		mpeg.addValue( "cc", "MPEG player to use", true );
		node.addValue( "showOneSeg", "Show one seg", false );	//	TODO
		mpeg.addValue( "otaMakerId", "Zapper OTA Maker ID", OTA_MAKER_ID );
		mpeg.addValue( "otaModelId", "Zapper OTA Model ID", OTA_MODEL_ID );
		mpeg.addValue( "otaVersion", "Zapper OTA Version", OTA_VERSION );
	}

	{	//	Add time node
		util::cfg::PropertyNode &time = node.addNode( "time" );
		time.addValue( "source", "Time source to use (ts|sys)", TIME_SOURCE );
		time.addValue( "offset", "Time offset from UTC", 0 );
	}
}

/**
 * Construye un objeto Zapper.
 * @param name Nombre del zapper.
 * @note Para instanciar un objeto Zapper debe utilizarse el método @c create.
 */
Zapper::Zapper( const std::string &name )
	: _name(name)
{
	_sys = NULL;
	_settings = NULL;
	_mount = NULL;
	_netlib = NULL;
	_update = NULL;
	_layers = NULL;
	_middleware = NULL;
	_pvr = NULL;
	_player = NULL;

	_initialized = false;
	_standby = false;
	_shutdown = util::cfg::getValue<bool>("zapper.canShutdown");
	_exitCode = 0;
}

/**
 * Destruye a la instancia de la clase @c Zapper.
 */
Zapper::~Zapper()
{
	DTV_ASSERT(!_initialized);
}

/**
 * Inicializa al objeto zapper. Llamar a éste método más de una vez no tendrá ningún efecto a menos que se finalice al
 * objeto antes.
 * @return True si se ha logrado inicializar con éxito al objeto Zapper, false caso contrario.
 */
bool Zapper::initialize() {
	if (!_initialized) {
		LDEBUG( "Zapper", "Initialize" );
		_exitCode = -1;

		//	Setup zapper title
		util::cfg::setValue( "gui.window.title", _name );

		//	Initialize hardware
		if (!initHardware()) {
			return false;
		}

		//	Setup database/RAM disk
		if (!initSettings()) {
			return false;
		}

		//	Create and setup system canvas
		if (!initCanvas()) {
			return false;
		}

		//	Setup mount manager
		if (!initMount()) {
			return false;
		}

		//	Setup network manager
		if (!initNetwork()) {
			return false;
		}

		//	Setup update manager
		if (!initUpdate()) {
			return false;
		}

		//	Setup middleware
		if (!initMiddleware()) {
			return false;
		}

		//	Initialize PVR
		if (!initPvr()) {
			return false;
		}

		_exitCode = 0;
		_standby = false;
		_initialized = true;
	}
	return _initialized;
}

/**
 * Finaliza al objeto zapper.
 */
void Zapper::finalize() {
	LDEBUG( "Zapper", "Finalize: initialized=%d", _initialized );
	if (_initialized) {
		_initialized = false;

		//	Finalize PVR
		finPvr();

		//	Finalize middleware
		DEL(_middleware);

		//	Finalize mount manager
		_mount->finalize();
		DEL(_mount);

		//	Finalize update manager
		DEL(_update);

		//	Finalize network manager
		_netlib->finalize();
		DEL(_netlib);

		//	Finalize system canvas
		finCanvas();

		//	Save settings
		_settings->finalize();
		DEL(_settings);

		//	Finalize hardware
		finHardware();
	}
}

/**
 * @return Nombre del objeto Zapper.
 */
const std::string &Zapper::name() const {
	return _name;
}

/**
 * Vuelve a la configuración de todos los servicios y plugins a su valor por defecto.
 */
void Zapper::resetConfig() {
	LDEBUG( "Zapper", "Reset stored configuration" );
	_pvr->resetConfig();
	_sys->resetConfig();
	_update->resetConfig();
	_middleware->resetConfig();
}

/**
 * Inicia al objeto Zapper. El objeto debe haber sido inicializado antes de llamar a éste método.
 */
void Zapper::start() {
	LDEBUG( "Zapper", "Start" );

	//	Start plugin manager
	DTV_ASSERT(_initialized);

	//	Load canvas configuration
	sys()->loadConfig();

	//	Start update service
	update()->start();

	//	Start middleware
	middleware()->start();

	//	Start player
	player()->start();

	//	Start custom
	onStart();
}

/**
 * Detiene al objeto Zapper, ésto implica que los servicios dejarán de correr.
 * @note El objeto debe haber sido inicializado antes de llamar a éste método.
 */
void Zapper::stop() {
	LDEBUG( "Zapper", "Stop" );
	DTV_ASSERT(_initialized);

	//	Stop custom
	onStop();

	//	Stop player
	player()->stop();

	//	Stop middleware
	middleware()->stop();

	//	Stop update service
	update()->stop();
}

void Zapper::onStart() {
}

void Zapper::onStop() {
}

/**
 * Inicia al loop principal del @c canvas::System.
 */
void Zapper::run() {
	//	Run canvas loop
	_sys->run();
}

/**
 * Si la opción shutdown ha sido activada el loop principal se termina.
 */
void Zapper::exit( int ret/*=0*/ ) {
	if (_shutdown || ret > 0) {
		_exitCode = ret;
		_sys->exit();
	}
}

void Zapper::exitCode(int exitCode) {
	_exitCode = exitCode;
}

int Zapper::exitCode() {
	return _exitCode;
}

bool Zapper::initCanvas() {
	//	Create canvas factory
	canvas::Factory *factory = createCanvas();
	if (!factory) {
		LWARN( "Zapper", "Cannot create canvas factory" );
		return false;
	}

	//	Create system instance
	_sys = factory->createSystem();
	if (!_sys) {
		delete factory;
		LWARN( "Zapper", "Cannot create system canvas" );
		return false;
	}

	//	Initialize
	if (!_sys->initialize(factory, _settings)) {
		DEL(_sys);
		return false;
	}

	//	Setup input manager
	_sys->input()->onKeyTranslator().connect( boost::bind(&Zapper::translateKey,this,_1) );
	if (supportStandby()) {
		util::key::Keys keys;
		keys.insert( util::key::power );
		keys.insert( util::key::f12 );
		sys()->input()->reserveKeys( this, boost::bind(&Zapper::onPowerKey,this,_1,_2), keys, MAX_INPUT_PRIORITY );
	}

	//	Create layer manager
	_layers = createLayer();
	if (!_layers) {
		LWARN( "Zapper", "Cannot create layer manager" );
		return false;
	}

	//	Setup layers
	if (!_layers->initialize()) {
		_sys->finalize();
		DEL(_sys);
		return false;
	}

	_sys->addDelegate( new canvas::settings::Layer(settings(), getLayer(layer::zapper)) );

	return true;
}

void Zapper::finCanvas() {
	_layers->finalize();
	DEL(_layers);
	_sys->finalize();
	DEL(_sys);
}

canvas::Factory *Zapper::createCanvas() const {
	return new canvas::Factory();
}

util::key::type Zapper::translateKey( util::key::type key ) {
	return key;
}

//	Aux hardare initialization
bool Zapper::initHardware() {
	return true;
}

void Zapper::finHardware() {
}

//	Stand by
Zapper::StandBySignal &Zapper::onStandby() {
	return _onStandby;
}

bool Zapper::supportStandby() const {
	return true;
}

void Zapper::onPowerKey( util::key::type /*key*/, bool isUp ) {
	if (isUp && _initialized) {
		if (!_standby) {
			_onStandby(true);
			standby();
		} else {
			wakeup();
 			_onStandby(false);
		}
		_standby = !_standby;
	}
}

void Zapper::standby() {
	sys()->standby();
	_middleware->stopAll();
	player()->stop();
	_pvr->session()->expire();
	doStandby();
}

void Zapper::wakeup() {
	doWakeup();
	player()->start();
	sys()->wakeup();
}

void Zapper::doStandby() {
	LINFO( "Zapper", "Standby!!!" );
}

void Zapper::doWakeup() {
	LINFO( "Zapper", "Wakeup!!!" );
}

//	Instance creation
mount::Mount *Zapper::createMount() const {
#if ZAPPER_MOUNT_USE_MDEV
	return new mount::mdev::Mount( _sys );
#else
	return new mount::Mount();
#endif
}

bool Zapper::initMount() {
	_mount = createMount();
	if (!_mount) {
		LERROR( "Zapper", "Cannot create mount manager" );
		return false;
	}
	if (!_mount->initialize()) {
		LERROR( "Zapper", "Cannot initialize mount manager" );
		return false;
	}

	//	Add images
	const std::string &path = util::cfg::getValue<std::string>("zapper.imagesPath");
	if (!path.empty()) {
		_mount->addPath( path, true );
	}

	return true;
}

mount::Mount *Zapper::mount() const {
	DTV_ASSERT(_mount);
	return _mount;
}

//	Database/RAM disk
bool Zapper::initSettings() {
	//	Database directory
	_dbDir = util::cfg::getValue<std::string>("zapper.db");
	if (_dbDir.empty()) {
		_dbDir = util::fs::stateDir();
	}
	LINFO( "Zapper", "Using DB directory=%s", _dbDir.c_str() );

	//	Create if not exist
	if (!fs::exists( _dbDir )) {
		fs::create_directories( _dbDir );
	}
	else if (!fs::is_directory( _dbDir )) {
		LERROR( "Zapper", "Database path is not a directory: dir=%s", _dbDir.c_str() );
		return false;
	}

	//	Setup RAM disk
	_ramDisk = util::cfg::getValue<std::string>("zapper.ramDisk");
	if (_ramDisk.empty()) {
		_ramDisk = util::fs::make( util::fs::stateDir(), "ramdisk" );
	}
	LINFO( "Zapper", "Using RamDisk directory=%s", _ramDisk.c_str() );

	//	Cleanup RAM disk
	if (!util::safeCleanDirectory( _ramDisk )) {
		LERROR( "Zapper", "Cannot clean RAM disk directory" );
		return false;
	}

	{	//	Load settings from db directory
		fs::path tmp( _dbDir );
		tmp /= util::main::name();
		std::string dbFile = tmp.string();

		_settings = util::Settings::create( dbFile, "bdb" );
		if (!_settings->initialize()) {
			DEL(_settings);
			LWARN("Zapper", "Could not initialize settings");
			return false;
		}
	}

	return true;
}

/**
 * Retorna el objeto de configuracion
 */
util::Settings *Zapper::settings() const {
	DTV_ASSERT(_settings);
	return _settings;
}

/**
 * Retorna el directorio de memoria volatile (RAM disk)
 */
const std::string &Zapper::ramDisk() const {
	return _ramDisk;
}

/**
 * Retorna el objeto de canales
 */
pvr::Pvr *Zapper::pvr() const {
	return _pvr;
}

/**
 * Retorna el directorio de memoria no volatile
 */
const std::string &Zapper::dbDir() const {
	return _dbDir;
}

/**
 * @return Puntero al @c canvas::System asociado al objeto Zapper.
 */
canvas::System *Zapper::sys() const {
	DTV_ASSERT(_sys);
	return _sys;
}

/**
 * @return Puntero al @c util::Dispatcher asociado al objeto Zapper.
 */
util::task::Dispatcher *Zapper::dispatcher() const {
	return _sys->dispatcher();
}

/*
 * @return Puntero al util::netlib::Manager asociado al objeto Zapper.
 */
util::netlib::Manager *Zapper::network() const {
	DTV_ASSERT(_netlib);
	return _netlib;
}

bool Zapper::initNetwork() {
	_netlib = util::netlib::Manager::create();
	if (!_netlib) {
		return false;
	}
	_netlib->dispatcher( sys()->dispatcher() );
	if (!_netlib->initialize()) {
		LERROR( "Zapper", "Cannot initialize network manager" );
		return false;
	}
	return true;
}

bool Zapper::initUpdate() {
	_update = new update::Update( ramDisk() );

	{	//	Use Network provider if necesary
		bool enable = util::cfg::getValue<bool>("zapper.update.useNetwork");
		LINFO( "Zapper", "Use Network update provider: %d", enable );
		if (enable) {
			_update->add( new update::network::Provider( this ) );
		}
	}

	return true;
}

update::Update *Zapper::update() const {
	DTV_ASSERT(_update);
	return _update;
}

/**
 * @param layer Tipo de capa que se quiere obtener.
 * @return Capa del tipo de @em layer.
 */
canvas::Layer *Zapper::getLayer( layer::type layer ) const {
	return _layers->getLayer( layer );
}

canvas::LayerManager *Zapper::createLayer() const {
	canvas::LayerManager *lm = new canvas::LayerManager();
	//	Setup layers
	lm->addLayer( new canvas::Layer( layer::video ) );
	lm->addLayer( new canvas::remote::Layer( _sys, layer::middleware, LAYER_MIDDLEWARE_ZINDEX ) );
	lm->addLayer( new canvas::layer::Surface( _sys->canvas(), layer::zapper, LAYER_ZAPPER_ZINDEX ) );
	return lm;
}

pvr::dvb::Delegate *Zapper::createTunerDelegate() const {
	return new dvb::Delegate( (Zapper *)this );
}

void Zapper::addDVB() {
	//	Create DVB Tuner
	pvr::dvb::Tuner *tuner = new pvr::dvb::Tuner( createTunerDelegate() );

	{	//	Use OTA provider if necesary
		bool enable = util::cfg::getValue<bool>("zapper.update.useOTA");
		LINFO( "Zapper", "Use OTA update provider: %d", enable );
		if (enable) {
			_update->add( new update::ota::Provider(this,tuner) );
		}
	}

	{	//	Add Ginga Middleware plugin if necesary
		bool useGinga = util::cfg::getValue<bool>("zapper.mpeg.useGinga");
		LINFO( "Zapper", "Use Ginga=%d", useGinga );
		if (useGinga) {
			_middleware->add( new middleware::ginga::Provider(this,tuner) );
		}
	}

	{	//	Setup Time source (TODO: Sacar de aca y llevarlo al mismo nivel de initPvr)
		const std::string &source = util::cfg::getValue<std::string>("zapper.time.source");
		const std::string &country = util::cfg::getValue<std::string>("zapper.countryCode");
		int offset = util::cfg::getValue<int>("zapper.time.offset");
		LINFO( "Zapper", "Use time from: source=%s, country=%s, offset=%d", source.c_str(), country.c_str(), offset );

		//	Setup source
		if (source == "ts") {
			tuner->attach( new tuner::TimeExtension(country) );
			pvr::time::init( new pvr::dvb::Time( offset, country, sys()->dispatcher() ) );
		}
		else {
			if (source != "sys") {
				LWARN( "Zapper", "Invalid time source: source=%s", source.c_str() );
			}
			//	Use system source
			pvr::time::init( new pvr::Time( offset, country ) );
		}
	}

	//	Add tuner to pvr
	_pvr->addTuner( tuner );
}

bool Zapper::initPvr() {
	//	Create PVR
	const std::string &file = util::cfg::getValue<std::string>("zapper.channelsDB");
	LDEBUG( "Zapper", "Create pvr: file=%s", file.c_str() );
	_pvr = new pvr::Pvr( settings(), file );

	//	Add DVB tuner
	addDVB();

#if ZAPPER_LOGOS_USE_NETWORK
	{	//	Use Network provider if necesary
		bool enable = util::cfg::getValue<bool>("zapper.logos.useNetwork");
		LINFO( "Zapper", "Use logos from network: enable=%d", enable );
		if (enable) {
			_pvr->logos()->add( new logos::network::Provider(this) );
		}
	}
#endif

	//	Initialize PVR
	if (!_pvr->initialize()) {
		return false;
	}

	//	Create channel player
	_player = new channel::Player( this );
	if (!_player->initialize()) {
		DEL(_player);
		LERROR( "Zapper", "Cannot initialize channel player" );
		return false;
	}

	return true;
}

void Zapper::finPvr() {
	LDEBUG( "Zapper", "Finalize channels" );
	_player->finalize();
	DEL(_player);

	_pvr->finalize();
	DEL(_pvr);
}

channel::Player *Zapper::player() const {
	DTV_ASSERT(_player);
	return _player;
}

//	Middleware
bool Zapper::initMiddleware() {
	_middleware = new middleware::Middleware();
	return true;
}

middleware::Middleware *Zapper::middleware() const {
	DTV_ASSERT(_middleware);
	return _middleware;
}

/**
 * @return Versión del zapper.
 */
std::string Zapper::getVersion() const {
	char version [15];
	snprintf( version, sizeof(version), "%03d.%03d.%03d",
		util::cfg::getValue<int>("zapper.mpeg.otaMakerId"),
		util::cfg::getValue<int>("zapper.mpeg.otaModelId"),
		util::cfg::getValue<int>("zapper.mpeg.otaVersion") );
	return version;
}

/**
 * @return Versión de la plataforma.
 * @note El archivo de la versión de la plataforma podría no ser encontrado, en cuyo caso el método retornará "00.00.00".
 */
std::string Zapper::getPlatformVersion() const {
	return util::cfg::getValue<std::string>("zapper.platformVersion");
}

}

