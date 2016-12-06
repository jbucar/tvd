/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "player.h"
#include "util.h"
#include "../../src/player/imageplayer.h"
#include "../../src/player/timelinetimer.h"
#include "../../src/player/settings.h"
#include "../../src/device.h"
#include "../../src/system.h"
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>
#include <cstring>

namespace fs = boost::filesystem;

namespace impl {
static player::System *sys;
static player::Device *dev;
}

void init() {
	//	Init sys
	impl::sys = new player::System();
	impl::dev = new player::Device( impl::sys );
	impl::dev->initialize();
	impl::sys->addDevice( impl::dev );

	//	Load settings configuration
	player::settings::load();
}

void fin() {
	DEL( impl::sys );
}

void run() {
	impl::sys->run();
}

void stop() {
	impl::sys->enqueue( boost::bind(&player::System::exit,impl::sys) );
}

Player::Player() {
	_player = NULL;
}

Player::~Player() {
}

player::Player *Player::createPlayer( const player::type::type &type ) const {
	return impl::dev->create( type );
}

player::TimeLineTimer *Player::createTimeLineTimer() const {
	return new player::TimeLineTimer( impl::sys );
}

void Player::destroyPlayer( player::Player *player ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::doDestroy, this, player) );
	wait();
}

//	Timers
util::id::Ident Player::registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback ) {
	return impl::sys->registerTimer( ms, callback );
}


void Player::SetUp( void ){
	_refs = 0;
	_player = impl::dev->create( type() );
	ASSERT_TRUE( _player != NULL );
	_result = false;
}

void Player::TearDown( void ){
	if (_player) {
		destroyPlayer( _player );
	}
}

void Player::setResultAndNotify( bool result ) {
	_result = result;
	unref();
}

void Player::compareImagesImpl( canvas::Canvas *canvas, const std::string &file ) {
	setResultAndNotify( util::compareImages( canvas, file ) );
}

void Player::compareImagesFromPathImpl( canvas::Canvas *canvas, const std::string &file ) {
	setResultAndNotify( util::compareImagesFromPath( canvas, file ) );
}

bool Player::compareImages( canvas::Canvas *canvas, const std::string &file ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::compareImagesImpl, this, canvas, file) );
	wait();
	return _result;
}

bool Player::compareImagesFromPath( canvas::Canvas *canvas, const std::string &file ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::compareImagesFromPathImpl, this, canvas, file) );
	wait();
	return _result;
}

void Player::doPlay( player::Player *player ) {
	_result = player->play();
	unref();
}

void Player::doStop( player::Player *player ) {
	player->stop();
	unref();
}

void Player::doPause( player::Player *player, bool pause ) {
	player->pause( pause );
	unref();
}

void Player::doDestroy( player::Player *player ) {
	impl::dev->destroy( player );
	unref();
}

void Player::wait() {
	{
		boost::unique_lock<boost::mutex> lock( _mutex );
		while (_refs > 0) {
			_cWakeup.wait( lock );
		}
	}
}

void Player::ref() {
	_mutex.lock();
	_refs++;
	_mutex.unlock();
}

void Player::unref() {
	_mutex.lock();
	_refs--;
	if (_refs <= 0) {
		_cWakeup.notify_one();
	}
	_mutex.unlock();
}

void Player::doEndDraw() {
	canvas()->endDraw();
	unref();
}

void Player::endDraw() {
	ref();
	impl::sys->enqueue( boost::bind(&Player::doEndDraw, this) );
	wait();
}

bool Player::play( player::Player *player ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::doPlay, this, player) );
	wait();
	return _result;
}

void Player::stop( player::Player *player ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::doStop, this, player) );
	wait();
}

void Player::pause( player::Player *player, bool pause ) {
	ref();
	impl::sys->enqueue( boost::bind(&Player::doPause, this, player, pause) );
	wait();
}

canvas::Canvas *Player::canvas() {
	return impl::dev->system()->canvas();
}

player::System *Player::system() {
	return impl::sys;
}

player::Device *Player::device() {
	return impl::dev;
}

std::string Player::getExpectedPath( const std::string &fileName ) const {
	fs::path root;
	root /= playerType();
	root /= property();
	root /= fileName;
	return root.string();
}

TEST( Player, fromMime ) {
	ASSERT_TRUE( player::type::fromMime( "IMAGE/BMP" ) == player::type::image );
	ASSERT_TRUE( player::type::fromMime( "ImAGe/BmP" ) == player::type::image );
	ASSERT_TRUE( player::type::fromMime( "IMAGE/PNG" ) == player::type::image );
	ASSERT_TRUE( player::type::fromMime( "IMAGE/JPEG" ) == player::type::image );
	ASSERT_TRUE( player::type::fromMime( "APPLICATION/X-GINGA-NCLUA" ) == player::type::lua );
	ASSERT_TRUE( player::type::fromMime( "TEXT/PLAIN" ) == player::type::text );
	ASSERT_TRUE( player::type::fromMime( "" ) == player::type::unknown );
}

TEST( Player, getExtensionfromMime ) {
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "IMAGE/BMP" ), "bmp") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "ImAGe/BmP" ), "bmp") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "IMAGE/PNG" ), "png") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "IMAGE/JPEG" ), "jpg") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "IMAGE/GIF" ), "gif") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "AUDIO/BASIC" ), "wav") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "AUDIO/AC3" ), "ac3") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "AUDIO/MPA" ), "mpa") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "AUDIO/MP3" ), "mp3") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "AUDIO/MP2" ), "mp2") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "VIDEO/MPEG4" ), "mp4") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "VIDEO/MPEG" ), "mpg") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "VIDEO/MPV" ), "mpv") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "VIDEO/QUICKTIM" ), "mov") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "VIDEO/X-MSVIDEO" ), "avi") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "APPLICATION/X-GINGA-NCLUA" ), "lua") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "APPLICATION/X-NCL-NCLUA" ), "lua") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "APPLICATION/X-GINGA-NCL" ), "ncl") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "APPLICATION/X-NCL-NCL" ), "ncl") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "TEXT/PLAIN" ), "txt") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "TEXT/HTML" ), "htm") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "TEXT/CSS"), "css") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "TEXT/XML"), "xml") == 0 );
	ASSERT_TRUE( strcmp(player::type::getExtensionFromMime( "TEXT/SRT"), "srt") == 0 );
	ASSERT_TRUE( player::type::getExtensionFromMime( "" ) == NULL );
}

TEST( Player, fromExtension ) {
	ASSERT_TRUE( player::type::fromBody( "" ) == player::type::unknown );
	ASSERT_TRUE( player::type::fromBody( "." ) == player::type::unknown );
	ASSERT_TRUE( player::type::fromBody( "algo.bmp" ) == player::type::image );
	ASSERT_TRUE( player::type::fromBody( "algo.BMP" ) == player::type::image );
	ASSERT_TRUE( player::type::fromBody( "/home/alguien/foto.pNg" ) == player::type::image );
	ASSERT_TRUE( player::type::fromBody( "/home/run.lua" ) == player::type::lua );
	ASSERT_TRUE( player::type::fromBody( "/home/run" ) == player::type::unknown );
}

TEST( Player, parse_invalid_url ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("http:/path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "http:/path/image.png");

	player::url::parse("http//path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "http//path/image.png");

	player::url::parse("://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::unknown );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("://", sch, body);
	ASSERT_TRUE( sch == player::schema::unknown );
	ASSERT_TRUE( body == "");

	player::url::parse("aaa://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::unknown );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("http://path://image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::http);
	ASSERT_TRUE( body == "path://image.png");
}

TEST( Player, parse_default ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("//path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "//path/image.png");

	player::url::parse("://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::unknown );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_file ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("file://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("FILE://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("FilE://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::file );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_http ) {
	player::schema::type sch;
	std::string body;
	player::type::type type;

	player::url::parse("http://path/image.png", sch, body);
	type = player::type::get( sch, body, "" );
	ASSERT_TRUE( type == player::type::image );
	ASSERT_TRUE( sch == player::schema::http );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("HTTP://path/image.png", sch, body);
	type = player::type::get( sch, body, "" );
	ASSERT_TRUE( type == player::type::image );
	ASSERT_TRUE( sch == player::schema::http );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("hTTp://path/image.png", sch, body);
	type = player::type::get( sch, body, "" );
	ASSERT_TRUE( type == player::type::image );
	ASSERT_TRUE( sch == player::schema::http );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("http://prueba/prueba.php?id=1", sch, body);
	type = player::type::get( sch, body, "" );
	ASSERT_TRUE( type == player::type::html );
	ASSERT_TRUE( sch == player::schema::http );
	ASSERT_TRUE( body == "prueba/prueba.php?id=1");
}

TEST( Player, parse_https ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("https://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::https );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("HTTPS://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::https );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("hTTpS://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::https );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_mirror ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("ncl-mirror://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::mirror );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("NCL-MIRROR://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::mirror );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("Ncl-MirROr://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::mirror );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_rtp ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("rtp://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtp );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("RTP://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtp );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("rTp://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtp );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_rtsp ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("rtsp://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtsp );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("RTSP://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtsp );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("rTSp://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::rtsp );
	ASSERT_TRUE( body == "path/image.png");
}

TEST( Player, parse_sbdtv ) {
	player::schema::type sch;
	std::string body;

	player::url::parse("sbtvd-ts://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::sbtvd );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("SBTVD-TS://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::sbtvd );
	ASSERT_TRUE( body == "path/image.png");

	player::url::parse("sBTvD-tS://path/image.png", sch, body);
	ASSERT_TRUE( sch == player::schema::sbtvd );
	ASSERT_TRUE( body == "path/image.png");
}
