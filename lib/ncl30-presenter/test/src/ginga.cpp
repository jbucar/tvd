#include "ginga.h"
#include "util.h"
#include "../../src/presentationenginemanager.h"
#include <gingaplayer/system.h>
#include <gingaplayer/player/keys.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <gingaplayer/device.h>
#include <util/cfg/cfg.h>
#include <util/registrator.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/main.h>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;
namespace bptgn = br::pucrio::telemidia::ginga::ncl;

namespace ginga {

namespace impl {
	static player::System *sys;
	static bptgn::PresentationEngineManager* pem;
}

void init() {
	//	Initialize the configuration
	util::main::init("ncl30-presentertest");

	//	Init sys
	impl::sys = new player::System();
	player::Device *dev=new player::Device( impl::sys );
	dev->initialize();
	impl::sys->addDevice( dev );

	//	Init PEM
	impl::pem = new bptgn::PresentationEngineManager( impl::sys );

	//	Uninitialize the configuration to test in a clean environment
	util::main::fin();
}

void fin() {
	DEL( impl::pem );
	DEL( impl::sys );
}

void run() {
	impl::pem->run();
}

void stop() {
	impl::sys->enqueue( boost::bind(&player::System::exit,impl::sys) );
}

Ginga::Ginga() {
	_compare = false;
	_keyPressed = false;
	_compareReady = false;
}

Ginga::~Ginga() {
}

void Ginga::SetUp( void ) {
	//	Reset configuration
	{
		util::reg::fin();
		util::reg::init();
	}

	util::log::init();
	util::cfg::get().addNode( "tool" ).addValue( "name", "", "ginga" );

	player::keys::init();

	DTV_ASSERT( impl::pem );
	bptgn::PresentationEngineManager::onKeyPress( boost::bind( &Ginga::onKeyPressed, this, _1 ) );
}

void Ginga::TearDown( void ) {
	bptgn::PresentationEngineManager::onKeyPress( NULL );
	player::keys::fin();
	util::log::fin();
}

bool Ginga::play( const std::string &file ) {
	fs::path url( util::getTestRoot() );
	url /= "src";
	url /= "ncl";
	url /= file;

	if (exists( url ) && is_regular_file( url )) {
		return impl::pem->startDocument( url.string() );
	}

	return false;
}

canvas::Canvas *Ginga::canvas( void ) {
	return impl::pem->mainScreen()->system()->canvas();
}

//	Compare
static boost::condition_variable _compareWakeup;
static boost::mutex _compareMutex;

bool Ginga::compareTo( const std::string &file ) {
	_file = file;
	impl::pem->enqueue( boost::bind( &Ginga::compare, this ) );

	{ //	Wait for compare
		boost::unique_lock<boost::mutex> lock( _compareMutex );
		while (!_compareReady) {
			_compareWakeup.wait( lock );
		}
		_compareReady = false;
	}
	return _compare;
}

void Ginga::compare( void ) {
	_compareMutex.lock();
	_compare = util::compareImages( canvas(), _file );
	_compareReady = true;
	_compareMutex.unlock();
	_compareWakeup.notify_one();
}

//	Key press
static boost::condition_variable _keyWakeup;
static boost::mutex _keyMutex;

bool Ginga::pressKey( ::util::key::type key ) {
	impl::pem->enqueue( boost::bind( &player::Device::dispatchKey, impl::pem->mainScreen(), key, false ) );
	{ //	Wait for compare
		boost::unique_lock<boost::mutex> lock( _keyMutex );
		while (!_keyPressed) {
			_keyWakeup.wait( lock );
		}
		_keyPressed = false;
	}
	return true;
}

void Ginga::onKeyPressed( ::util::key::type /*key*/) {
	_keyMutex.lock();
	_keyPressed = true;
	_keyMutex.unlock();
	_keyWakeup.notify_one();
}

}
