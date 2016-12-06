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

#include "system.h"
#include "factory.h"
#include "point.h"
#include "canvas.h"
#include "audio.h"
#include "screen.h"
#include "window.h"
#include "input.h"
#include "player.h"
#include "webviewer.h"
#include "inputmanager.h"
#include "settings/audio.h"
#include "settings/screen.h"
#include "generated/config.h"
#if CANVAS_SYSTEM_USE_SDL
#       include "impl/sdl/system.h"
#endif
#if CANVAS_SYSTEM_USE_GTK
#       include "impl/gtk/system.h"
#endif
#if CANVAS_SYSTEM_USE_X11
#       include "impl/x11/system.h"
#endif
#if CANVAS_SYSTEM_USE_DIRECTFB
#       include "impl/directfb/system.h"
#endif
#if CANVAS_SYSTEM_USE_WIN32
#       include "impl/win32/system.h"
#endif
#if CANVAS_SYSTEM_USE_QT
#       include "impl/qt/system.h"
#endif
#if CANVAS_SYSTEM_USE_ANDROID
#       include "impl/android/system.h"
#endif
#if CANVAS_SYSTEM_USE_IOS
#       include "impl/ios/system.h"
#endif
#if CANVAS_SYSTEM_USE_PPAPI
#       include "impl/ppapi/system.h"
#endif
#include <util/io/dispatcher.h>
#include <util/task/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <util/cfg/cfg.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/settings/settings.h>
#include <util/settings/settingsdelegate.h>
#include <boost/bind.hpp>

namespace canvas {

#if CANVAS_SYSTEM_USE_GTK
#	define DEFAULT_SYSTEM "gtk"
#elif CANVAS_SYSTEM_USE_SDL
#	define DEFAULT_SYSTEM "sdl"
#elif CANVAS_SYSTEM_USE_DIRECTFB
#	define DEFAULT_SYSTEM "directfb"
#elif CANVAS_SYSTEM_USE_X11
#	define DEFAULT_SYSTEM "x11"
#elif CANVAS_SYSTEM_USE_WIN32
#	define DEFAULT_SYSTEM "win32"
#elif CANVAS_SYSTEM_USE_QT
#	define DEFAULT_SYSTEM "qt"
#elif CANVAS_SYSTEM_USE_ANDROID
#	define DEFAULT_SYSTEM "android"
#elif CANVAS_SYSTEM_USE_IOS
#	define DEFAULT_SYSTEM "ios"
#elif CANVAS_SYSTEM_USE_PPAPI
#	define DEFAULT_SYSTEM "ppapi"
#else
#	define DEFAULT_SYSTEM "custom"
#endif


REGISTER_INIT_CONFIG( gui ) {
	root().addNode( "gui" )
		.addValue( "use", "System to initialize", std::string(DEFAULT_SYSTEM) );
}

/**
 * Crea una instancia de System.
 * @param useParam indica la implementación a usar.
 */
System *System::create( const std::string &useParam/*=""*/ ) {
	System *sys=NULL;
	const std::string &use = useParam.empty() ? util::cfg::getValue<std::string> ("gui.use") : useParam;

	LINFO("System", "Using system: use=%s", use.c_str() );

#if CANVAS_SYSTEM_USE_SDL
	if (use == "sdl") {
		sys = new sdl::System();
	}
#endif

#if CANVAS_SYSTEM_USE_GTK
	if (use == "gtk") {
		sys = new gtk::System();
	}
#endif

#if CANVAS_SYSTEM_USE_QT
	if (use == "qt") {
		sys = new qt::System();
	}
#endif

#if CANVAS_SYSTEM_USE_WIN32
	if (use == "win32") {
		sys = new win32::System();
	}
#endif

#if CANVAS_SYSTEM_USE_DIRECTFB
	if (use == "directfb") {
		sys = new directfb::System();
	}
#endif

#if CANVAS_SYSTEM_USE_X11
	{
		bool createX11 = false;

#if CANVAS_2D_USE_GL and CANVAS_X11_RENDER_USE_GL
		if (use == "gl") {
			//	Setup render and canvas
			util::cfg::setValue("gui.window.x11.use", "gl");
			util::cfg::setValue("gui.canvas.use", "gl");
			createX11=true;
		}
#endif

#if CANVAS_2D_USE_SKIA and CANVAS_X11_RENDER_USE_SKIA
		if (use == "skia") {
			//	Setup render and canvas
			util::cfg::setValue("gui.window.x11.use", "skia");
			util::cfg::setValue("gui.canvas.use", "skia");
			createX11=true;
		}
#endif

		if (use == "x11") {
			createX11 = true;
		}

		if (createX11) {
			sys = new x11::System();
		}
	}
#endif	//	CANVAS_SYSTEM_USE_X11

#if CANVAS_SYSTEM_USE_ANDROID
	if (use == "android") {
		sys = new android::System();
	}
#endif

#if CANVAS_SYSTEM_USE_IOS
	if (use == "ios") {
		sys = new ios::System();
	}
#endif

#if CANVAS_SYSTEM_USE_PPAPI
	if (use == "ppapi") {
		sys = new ppapi::System();
	}
#endif

	if (use == "custom") {
		sys = new System();
	}

	DTV_ASSERT(sys);
	return sys;
}

/**
 * Constructor base. Para crear una instancia de System debe usarse el método estático create() y luego llamar al método initialize().
 */
System::System()
{
	_factory = NULL;
	_io = NULL;
	_screen = NULL;
	_win = NULL;
	_canvas = NULL;
	_player = NULL;
	_input = NULL;
	_audio = NULL;
}

/**
 * Destructor base. Antes de destruir el System se debe llamar al método finalize().
 */
System::~System()
{
	CLEAN_ALL( Input *, _inputs );
	DTV_ASSERT(!_io);
	DTV_ASSERT(!_screen);
	DTV_ASSERT(!_win);
	DTV_ASSERT(!_canvas);
	DTV_ASSERT(!_player);
	DTV_ASSERT(!_factory);
	DTV_ASSERT(!_input);
	DTV_ASSERT(!_audio);
}

void System::dispatchShutdown() {
	LDEBUG("System", "Shutdown event received");
	boost::optional<bool> res=_onShutdown();
	bool needShutdown = res ? (*res) : true;
	if (needShutdown) {
		exit();
	}
}

/**
 * Inicializa el estado del System.
 * @param factory Factory a utilizar.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool System::initialize( Factory *factory, util::Settings *settings /*=NULL*/ ) {
	LDEBUG("System", "Initialize");

	//	Setup factory
	DTV_ASSERT(factory);
	_factory = factory;

	//	Create all inputs
	_factory->createInputs(this);

	//	Create and initialize IO dispatcher
	if (!setupIO()) {
		cleanup();
		return false;
	}

	if (!init()) {
		LERROR("System", "Cannot initialize system");
		cleanup();
		return false;
	}

	//	Create and initialize screen
	if (!setupScreen()) {
		cleanup();
		return false;
	}

	//	Create and initialize window
	if (!setupWindow()) {
		cleanup();
		return false;
	}

	//	Create and initialize canvas
	if (!setupCanvas()) {
		cleanup();
		return false;
	}

	//	Create and initialize player
	if (!setupPlayer()) {
		cleanup();
		return false;
	}

	//	Create and initialize audio
	if (!setupAudio()) {
		cleanup();
		return false;
	}

	//	Create input manager
	_input = new InputManager(this);

	//	Initialize inputs
	BOOST_FOREACH( Input *input, _inputs ) {
		if (!input->initialize()) {
			LWARN("System", "Cannot initialize input: name=%s\n", input->name().c_str() );
		}
	}

	if (settings) {
		//	Setup settings delegate for audio and screen
		_delegates.push_back( new settings::Audio(settings, audio()) );
		_delegates.push_back( new settings::Screen(settings, screen()) );
	}

	onInitialized();

	return true;
}

void System::onInitialized() {
}

void System::cleanup() {
	onFinalized();

	CLEAN_ALL( util::SettingsDelegate *, _delegates );

	DEL(_input);

	if (_canvas) {
		_canvas->finalize();
		DEL(_canvas);
	}

	if (_win) {
		_win->finalize();
		DEL(_win);
	}

	if (_audio) {
		_audio->finalize();
		DEL(_audio);
	}

	if (_screen) {
		_screen->finalize();
		DEL(_screen);
	}

	if (_player) {
		_player->finalize();
		DEL(_player);
	}

	if (_io) {
		fin();

		_io->finalize();
		DEL(_io);
	}

	if (_factory) {
		DEL(_factory);
	}
}

void System::onFinalized() {
}

/**
 * Finaliza el estado del System.
 */
void System::finalize() {
	LDEBUG("System", "Finalize");
	//	Finalize inputs
	BOOST_FOREACH( Input *input, _inputs ) {
		input->finalize();
	}
	cleanup();
}

bool System::init() {
	return true;
}

void System::fin() {
}


/**
 * Ejecuta el loop principal. Este método retorna cuando se llama al método exit().
 */
void System::run() {
	_threadId = boost::this_thread::get_id();
	window()->mainLoopThreadId( _threadId );
	player()->mainLoopThreadId( _threadId );
	io()->run();
}

/**
 * Termina el loop principal causando el retorno del llamado a run().
 */
void System::exit() {
	DTV_ASSERT( _threadId == boost::this_thread::get_id() );
	io()->exit();
}

OnShutdownSignal &System::onShutdown() {
	return _onShutdown;
}

//	Standby/wakeup
void System::standby() {
	if (_factory) {	//	Initialized?
		input()->standby();
		screen()->turnOff();
		audio()->standby();
	}
}

void System::wakeup() {
	if (_factory) {
		audio()->wakeup();
		screen()->turnOn();
		input()->wakeup();
	}
}

// Dispatcher
util::task::Dispatcher *System::dispatcher() const {
	return io()->dispatcher();
}

/*
 * Encola una tarea para ser ejecutada en el thread principal
 * @param target El objeto al que esta asociada la tarea
 * @param task La tarea a ejecutar en el thread principal
 */
void System::post( util::task::Target target, const util::task::Type &task ) {
	dispatcher()->post( target, task );
}

//	IO dispatcher methods
util::io::Dispatcher *System::io() const {
	DTV_ASSERT(_io);
	return _io;
}

util::io::Dispatcher *System::createIO() const {
	return _factory->createIO();
}

bool System::setupIO() {
	_io = createIO();
	if (!_io) {
		LERROR("System", "cannot create io dispatcher");
		return false;
	}

	if (!_io->initialize()) {
		LERROR("System", "cannot initialize io dispatcher");
		DEL(_io);
		return false;
	}

	return true;
}

//	HTML viewer
/**
 * Crea y retorna una instancia de WebViewer que utilizará para el renderizado el Surface pasado como parámetro.
 * @param surface La superficie que se utilizará para el renderizado
 * @return Una instancia de WebViewer
 */
WebViewer *System::createWebViewer( Surface *surface ) {
	return _factory->createWebViewer( this, surface );
}

// Canvas
Canvas *System::createCanvas() const {
	return _factory->createCanvas();
}

bool System::setupCanvas() {
	_canvas = createCanvas();
	if (!_canvas) {
		LERROR("System", "cannot create canvas");
		return false;
	}

	if (!_canvas->initialize( _win )) {
		LERROR("System", "cannot initialize canvas");
		DEL(_canvas);
		return false;
	}

	return true;
}

/**
 * @return la instancia de Canvas creada al inicializar el System.
 */
Canvas *System::canvas() const {
	DTV_ASSERT(_canvas);
	return _canvas;
}

//	Screen methods
Screen *System::createScreen() const {
	return _factory->createScreen();
}

/**
 * @return la instancia de Screen creada/usada al inicializar el System.
 */
Screen *System::screen() const {
	DTV_ASSERT(_screen);
	return _screen;
}

bool System::setupScreen() {
	_screen = createScreen();
	if (!_screen) {
		LWARN("System", "cannot create screen");
		return false;
	}

	if (!_screen->initialize()) {
		LWARN("System", "cannot initialize screen");
		DEL(_screen);
		return false;
	}

	return true;
}

/**
 * @return la instancia de Audio creada al inicializar el System.
 */
Audio *System::audio() const {
	DTV_ASSERT(_audio);
	return _audio;
}

//	Audio methods
Audio *System::createAudio() const {
	return _factory->createAudio();
}

bool System::setupAudio() {
	_audio = createAudio();
	if (!_audio) {
		LWARN("System", "cannot create audio");
		return false;
	}

	if (!_audio->initialize( this )) {
		LWARN("System", "cannot initialize audio");
		DEL(_audio);
		return false;
	}

	return true;
}

//	Window
Window *System::createWindow() const {
	return _factory->createWindow();
}

/**
 * Retorna la instancia de Window creada al inicializar el System.
 */
Window *System::window() const {
	DTV_ASSERT(_win);
	return _win;
}

bool System::setupWindow() {
	//	Create and initialize window
	_win = createWindow();
	if (!_win) {
		LWARN("System", "cannot create window");
		return false;
	}

	if (!_win->initialize( this )) {
		LWARN("System", "cannot initialize window");
		DEL(_win);
		return false;
	}

	return true;
}

//	Player
Player *System::createPlayer() const {
	return _factory->createPlayer();
}

bool System::setupPlayer() {
	_player = createPlayer();
	if (!_player) {
		LWARN("System", "cannot create player");
		return false;
	}

	if (!_player->initialize()) {
		LWARN("System", "cannot initialize player");
		DEL(_player);
		return false;
	}

	return true;
}

/**
 * Retorna la instancia de Player creada al inicializar el System.
 */
Player *System::player() const {
	return _player;
}

/**
 * Agrega un manejador de eventos de entrada al System, causando que se registren los eventos
 * producidos por dicho Input.
 * @param in El manejador de eventos a agregar al System.
 */
void System::addInput( Input *in ) {
	LINFO( "System", "Using input: %s", in->name().c_str() );
	_inputs.push_back( in );
}

/**
 * Resetea la configuración
 */
void System::resetConfig() {
	BOOST_FOREACH( util::SettingsDelegate *d, _delegates ) {
		d->reset();
	}
}

/**
 * Carga la configuración
 */
void System::loadConfig() {
	BOOST_FOREACH( util::SettingsDelegate *d, _delegates ) {
		d->load();
	}
}

/**
 * Agrega un SettingDelegate para que éste sea utilizado a la hora de la configuración
 */
void System::addDelegate( util::SettingsDelegate *delegate ) {
	_delegates.push_back(delegate);
}

/**
 * Retorna la instancia de InputManager creada al inicializar el System.
 */
InputManager *System::input() const {
	DTV_ASSERT(_input);
	return _input;
}

}
