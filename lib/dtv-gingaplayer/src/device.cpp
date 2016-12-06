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

#include "device.h"
#include "system.h"
#include "generated/config.h"
#include "player.h"
#include "player/luaplayer.h"
#include "player/imageplayer.h"
#include "player/animeplayer.h"
#include "player/textplayer.h"
#include "player/soundplayer.h"
#include "player/videoplayer.h"
#include "player/htmlplayer.h"
#include <canvas/inputmanager.h>
#include <canvas/player.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <canvas/factory.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <util/mcr.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <curl/curl.h>

#define EXTERNAL_RESOURCE_TIMEOUT 5

namespace player {

namespace fs = boost::filesystem;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

/**
 * Constructor.
 * @param sys La instancia del @c System al que pertenece el device.
 */
Device::Device( System *sys )
	: _sys(sys)
{
	_canvas = NULL;
}

/**
 * Destructor.
 */
Device::~Device()
{
	if (_players.size()) {
		LWARN("Device", "not all players destroyed: size=%d", _players.size());
	}

	DTV_ASSERT(!_canvas);

	BOOST_FOREACH( const DownloadCache::value_type &val, _downloadCache ) {
		try {
			fs::remove( val.second );
		} catch (...) {
		}
	}
}

/**
 * @return Puntero al @c System asociado al dispositivo.
 */
System *Device::systemPlayer() const {
	return _sys;
}

/**
 * @return Puntero al @c canvas::System asociado al dispositivo.
 */
canvas::System *Device::system() const {
	DTV_ASSERT(_canvas);
	return _canvas;
}

/**
 * @return True si el dispositivo tiene video, false de otra manera.
 */
bool Device::haveVideo() const {
	return true;
}

/**
 * @return True si el dispositivo tiene audio, false de otra manera.
 */
bool Device::haveAudio() const {
	return true;
}

Player *Device::addPlayer( Player *player ) {
	//	Initialize player
	if (player->initialize()) {
		_players.push_back( player );
		return player;
	}
	else {
		LWARN("Device", "cannot initialize player");
		delete player;
		return NULL;
	}
}

/**
 * Crea un player en el contexto de el dispositivo.
 * @param typeOfPlayer Tipo del player.
 * @return Puntero al @c Player creado.
 */
Player *Device::create( enum type::type typeOfPlayer ) {
	Player *player;

	switch (typeOfPlayer) {
		case type::audio:
			player = new SoundPlayer( this );
			break;
		case type::image:
			player = new ImagePlayer( this );
			break;
		// case type::anime:
		// 	player = new AnimePlayer( this );
		// 	break;
		case type::lua:
			player = new LuaPlayer( this );
			break;
		case type::text:
			player = new TextPlayer( this );
			break;
		case type::video:
			player = new VideoPlayer( this );
			break;
		case type::html:
			player = new HtmlPlayer( this );
			break;
		default: {
			LWARN("Device", "cannot create player: type=%s", type::getName(typeOfPlayer) );
			return NULL;
		}
	};

	//	Add player to device
	return addPlayer( player );
}

/**
 * Crea un player en el contexto de el dispositivo.
 * @param url Url del player.
 * @param mime Tipo mime del player.
 * @return Puntero al @em Player creado.
 */
Player *Device::create( const std::string &url, const std::string &mime ) {
	schema::type schema;
	std::string body;
	type::type typeOfPlayer;

	url::parse( url, schema, body );

	//	Check schema
	if (schema == schema::file || schema == schema::http || schema == schema::https) {
		typeOfPlayer=type::get( schema, body, mime );
	}
	else if (schema == schema::sbtvd
		|| schema == schema::rtsp
		|| schema == schema::rtp)
	{
		typeOfPlayer=type::video;
	}
	else {
		LWARN("Device", "schema not supported: schema=%s", schema::getName(schema) );
		return NULL;
	}

	LINFO("Device", "Create player: type=%s, schema=%s, body=%s, mime=%s",
		type::getName(typeOfPlayer), schema::getName(schema), body.c_str(), mime.c_str() );

	//	Setup src and type
	Player *player=create( typeOfPlayer );
	if (player) {
		player->setProperty( "src", url );
		player->setProperty( "type", mime );
	}
	return player;
}

/**
 * Destruye al @em player.
 */
void Device::destroy( Player *player ) {
	LDEBUG("Device", "Destroy player: player=%p", player);
	DTV_ASSERT(player);

	std::vector<Player *>::iterator it=std::find(
		_players.begin(), _players.end(), player );
	if (it != _players.end()) {
		player->finalize();
		delete player;
		_players.erase( it );
	}
	else {
		LWARN("Device", "player is not member of this device");
	}
}

/**
 * Inicia al dispositivo.
 */
void Device::start() {
}

/**
 * Detiene al dispositivo.
 */
void Device::stop() {
}

/**
 * Inicializa al dispositivo.
 * @return True si se ha podido inicializar al dispositivo exitosamente, false caso contrario.
 */
bool Device::initialize() {
	canvas::Factory *factory = createFactory();
	_canvas = factory->createSystem();
	return system()->initialize(factory);
}

canvas::Factory *Device::createFactory() const {
	return new canvas::Factory();
}

/**
 * Finaliza al dispositivo.
 */
void Device::finalize() {
	system()->finalize();
	DEL(_canvas);
}

/**
 * Produce la finalización del dispositivo.
 */
void Device::exit() {
	LINFO( "Device", "Exit received" );
	systemPlayer()->exit();
}

/**
 * Envía el mensaje onReserveKeys a todos los objetos en su lista de dispositivos con las @em keys como parámetro.
 * @param keys Conjunto de objetos key.
 */
void Device::onReserveKeys( const ::util::key::Keys &/*keys*/ ) {
	//	Do nothing
}

/**
 * Despacha un evento de teclado.
 * @param keystroke La tecla a despachar.
 * @param isUp True si el evento se generó por la pulsación de una tecla, false caso contrario.
 */
void Device::dispatchKey( util::key::type keyValue, bool isUp ) {
	_sys->dispatchKey( keyValue, isUp );
}

void Device::dispatchButton( unsigned int button, int x, int y, bool isPress ) {
	canvas::input::ButtonEvent evt;
	evt.button = button;
	evt.x = x;
	evt.y = y;
	evt.isPress = isPress;
	_sys->dispatchButton( &evt );
}

/**
 * Despacha un evento de editing command.
 * @param event Evento a despachar.
 */
void Device::dispatchEditingCommand( connector::EditingCommandData *event ) {
	_sys->dispatchEditingCommand( event );
}

/**
 * Descarga un recurso externo.
 * @param url Url del recurso.
 * @param newFile Nombre que tomará el archivo descargado.
 * @return True si se ha logrado descargar con éxito el recurso, false caso contrario.
 */
bool Device::download( const std::string &url, std::string &newFile ) {
	bool result;

	DownloadCache::const_iterator it=_downloadCache.find( url );
	if (it != _downloadCache.end()) {
		newFile=(*it).second;
		result=true;
	}
	else {
		result = tryDownload( url, newFile );
		if (result) {
			_downloadCache[url] = newFile;
		}
	}
	return result;
}

bool Device::tryDownload( const std::string &url, std::string &file ) {
	CURL *curl;
	bool result=false;

	curl = curl_easy_init();
	if (curl) {
		//	Set basic options
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, EXTERNAL_RESOURCE_TIMEOUT);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

		//	HTTPS options
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		//	Setup file name (temporary, with unknown extension)
		fs::path fsTemp = fs::temp_directory_path();
		fsTemp /= "ginga_player_file%%%%%%%%";

		//	Add temporary extension
		std::string fileToDownload = fs::unique_path(fsTemp).string();
		fileToDownload += ".unknown";

		//	Open file
		FILE *pagefile = fopen(fileToDownload.c_str(), "wb");
		if (pagefile) {
			//	Setup file
			curl_easy_setopt(curl, CURLOPT_FILE, pagefile);

			//	Perform operation
			result = curl_easy_perform(curl) == CURLE_OK;

			//	Close file
			fclose(pagefile);

			//	Check result
			if (result) {
				//	Check content type
				char *ct;
				result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct) == CURLE_OK && ct;
				if (result) {
					const char *ext=type::getExtensionFromMime( ct );
					if (ext) {
						//	Fix file extension
						file = fs::unique_path(fsTemp).string();
						file += ".";
						file += ext;

						//	Rename file
						fs::rename( fileToDownload, file );
						LDEBUG( "Device", "File download: name=%s, mime=%s\n",
							file.c_str(),  ct );

						result=true;
					}
					else {
						result=false;
					}
				}
			}
		}

		//	Cleanup
		curl_easy_cleanup(curl);
	}

	if (!result) {
		LERROR("Device", "cannot load external resource: url=%s", url.c_str());
	}

	return result;
}

/**
 * Crea un nuevo @c MediaPlayer de la librería canvas.
 * @return Puntero al @c MediaPlayer creado.
 */
canvas::MediaPlayer *Device::createMediaPlayer() {
	return system()->player()->create( system() );
}

void Device::destroy( canvas::MediaPlayerPtr &mp ) {
	system()->player()->destroy(mp);
}

/**
 * Crea un nuevo @c WebViewer de la librería canvas.
 * @param surface @c Surface a partir de la cual se crea la vista web.
 * @return Puntero al @c WebViewer creado.
 */
canvas::WebViewer *Device::createWebViewer( canvas::Surface *surface ) {
	return system()->createWebViewer( surface );
}

}
