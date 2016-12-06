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
#include "device.h"
#include "property/propertyimpl.h"
#include "property/compositeproperty.h"
#include "player/timelinetimer.h"
#include <util/functions.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <algorithm>

namespace player {

namespace fs = boost::filesystem;

/**
 * Construye un objeto @c Player.
 * @param dev Dispositivo al que el @c Player pertenece.
 */
Player::Player( Device *dev )
{
	_dev         = dev;
	_initialized = false;
	_playing     = false;
	_pause       = false;
	_schema      = schema::unknown;
	_timer       = new TimeLineTimer( dev->systemPlayer() );
	_duration    = 0;
}

/**
 * Destruye un objeto @c Player.
 */
Player::~Player()
{
	DTV_ASSERT(!_initialized);
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		delete value.second;
	}
	delete _timer;
}

/**
 * @return True si el player ha sido inicializado, false de otra manera.
 */
bool Player::initialized() const {
	return _initialized;
}

/**
 * Inicializa al @c Player.
 * @return True si el player ha logrado inicializar al player con éxito, false caso contrario.
 */
bool Player::initialize() {
	LDEBUG("player", "Initialize");

	bool result=true;
	if (!initialized()) {
		result=init();
	}

	if (result) {
		registerProperties();
		_initialized=true;
	}

	return result;
}

bool Player::init() {
	return true;
}

/**
 * Finaliza al @c Player.
 */
void Player::finalize() {
	LDEBUG("player", "Finalize");
	if (initialized()) {
		stop();
		fin();
		_initialized=false;
	}
}

void Player::fin() {
}

/**
 * Crea un timer que, pasados @em ms milisegundos, disparará una acción.
 * @param ms Tiempo que deberá transcurrir para que se produzca la acción.
 * @param fnc Función que será llamada cuándo haya transcurrido el tiempo indicado.
 */
void Player::createTimer( util::DWORD ms, const TimerCallback &fnc ) {
	LDEBUG("player", "Create timer with %u", ms);
	_timer->addTimer( ms, fnc );
}

/**
 * @param fnc Función que se llamara cuándo el Player pase a estado detenido.
 */
void Player::onStopped(const OnStatusChanged &fnc) {
	_onStopped = fnc;
}

/**
 * @param fnc Función que se llamará cuándo el Player comience a reproducirse.
 */
void Player::onStarted(const OnStatusChanged &fnc){
	_onStarted = fnc;
}

/**
 * @param fnc Función que se llamará cuándo el Player pase a estado pausado.
 */
void Player::onPaused(const OnPauseStatusChanged &fnc){
	_onPaused = fnc;
}

bool Player::isPlaying() const {
	return _playing;
}

/**
 * @return True si el player esta en estado @em pause, false de otra manera.
 */
bool Player::isPaused() const {
	return _pause;
}

/**
 * Comienza a reproducir el contenido del @c Player.
 * @return True si se ha podido comenzar a reproducir el contenido del player, false caso contrario.
 * @note Éste método provocará una llamada al callback OnStarted.
 */
bool Player::play() {
	LDEBUG("player", "play");
	DTV_ASSERT(initialized());

	//	Stop if playing
	stop();

	//	Mark all properties modified
	markModified();

	//	Can play?
	if (canPlay()) {
		//	Initialize src
		url::parse( _url, _schema, _body );

		_playing = startPlay();
		if (_playing) {
			applyChanges();
			_timer->start();
		}
		if(!_onStarted.empty()){
			_onStarted();
		}
	}
	return _playing;
}

bool Player::canPlay() const {
	bool result=true;
	LDEBUG("player", "Can play begin");
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		if (!value.second->canStart()) {
			LWARN("player", "property can't start, name=%s", property::getPropertyName(value.first));
			result=false;
			break;
		}
	}
	LDEBUG("player", "Can play: result=%d", result );
	return result;
}

void Player::pausePlay( bool /*pause*/ ) {
}

void Player::abortPlay() {
	stop();
}

/**
 * Pone o quita al @c Player en estado pausado. El contenido deja de reproducirse.
 * @param pause True si se desea pausar al player y false si se desea sacar del estado @em pause al mismo.
 * @note Éste método provocará una llamada al callback OnPaused.
 */
void Player::pause( bool pause ) {
	LDEBUG("player", "Pause: pause=%d", pause );
	if (isPlaying() && _pause != pause) {
		pausePlay( pause );
		if (!pause) {
			applyChanges();
			_timer->unpause();
		}
		else {
			_timer->pause();
		}
		_pause = pause;
		if(!_onPaused.empty()){
			_onPaused( pause );
		}
	}
}

/**
 * Detiene la reproducción del contenido del @c Player.
 * @note Éste método provocará una llamada al callback OnStopped.
 */
void Player::stop( void ) {
	DTV_ASSERT(initialized());

	if (isPlaying()) {
		LDEBUG("player", "Stop");
		_playing = false;
		_pause   = false;
		_timer->stop();
		stopPlay();
		if(!_onStopped.empty()){
			_onStopped();
		}
	}
}

/**
 * Análogo al método stop.
 */
void Player::abort( void ) {
	DTV_ASSERT(initialized());

	if (isPlaying()) {
		LDEBUG("player", "abort");
		abortPlay();
	}
}

void Player::markModified() {
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		value.second->markModified();
	}
}

void Player::applyChanges() {
	bool needRefresh=false;
	bool changed=false;
	LDEBUG("player", "Apply changes");

	//	Apply changes on all properties
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		Property *prop = value.second;
		if (prop->changed()) {
			needRefresh |= prop->apply();
			changed=true;
		}
	}

	if (changed) {
		apply( needRefresh );
	}
}

void Player::apply( bool needRefresh ) {
	//	If need refresh the player, do work
	if (needRefresh) {
		beginRefresh();
		refresh();
		endRefresh();
	}

	onPropertyChanged();
}

void Player::beginRefresh() {
}

void Player::refresh() {
}

void Player::endRefresh() {
}

void Player::onPropertyChanged() {
}

/**
 * Configura que acción se tomará cuándo se produzca un evento.
 * @param callback Función que será llamada cuando se produzca un evento.
 */
void Player::setEventCallback( const event::Callback &/*callback*/ ) {
}

/**
 * Configura que acción se tomará cuándo se produzca un evento de teclado.
 * @param callback Función que será llamada cuando se produzca un evento.
 */
void Player::setInputEventCallback( const event::InputCallback &/*callback*/ ) {
}

/**
 * @return Ubicación del recurso que se está reproduciendo.
 */
std::string Player::rootPath() const {
	if (_schema == schema::file) {
		return fs::path(_body).branch_path().string();
	}
	return _body;
}

/**
 * @return Cuánto tiempo se ha estado reproduciendo el contenido.
 */
util::DWORD Player::uptime() const {
	return _timer->uptime();
}

/**
 * @return True si el @c Player es un reproductor de una aplicación Lua, false caso contrario.
 */
bool Player::isApplication() const {
	return false;
}

/**
 * @return True si el @c Player es visible, false caso contrario.
 */
bool Player::isVisible() const {
	return false;
}

bool Player::isGraphic() const {
	return false;
}

/**
 * Convierte el texto a UTF8
 * @param text Texto a convertir
 */
void Player::convertText( std::string &text ) {
	util::toUTF8( "ISO-8859-1", text );
}

bool Player::checkUrl( const std::string &url ) {
	schema::type schema;
	std::string body;
	bool result=false;

	//	Parse url
	url::parse( url, schema, body );
	if (schema != schema::unknown) {
		result=supportSchemma( schema );
		if (result) {
			if (schema == schema::file) {
				result=check::fileExists( body );
			}
			else if (!supportRemote() && (schema == schema::http || schema == schema::https)) {
				std::string fileDownloaded;
				if (_dev->download( url, fileDownloaded )) {
					std::string newUrl = "file://" + fileDownloaded;
					if (checkUrl( newUrl )) {
						_url = newUrl;
					}
				}
			}
		}
	}
	return result;
}

bool Player::supportSchemma( schema::type sch ) const {
	return sch == schema::http || sch == schema::file || sch == schema::https;
}

bool Player::supportRemote() const {
	return false;
}

schema::type Player::schema() const {
	return _schema;
}

const std::string &Player::body() const {
	return _body;
}

const std::string &Player::url() const {
	return _url;
}

const std::string &Player::type() const {
	return _type;
}

void Player::registerProperties() {
	{	//	Source property
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _url );
		prop->setCheck( boost::bind(&Player::checkUrl,this,_1) );
		addProperty( property::type::src, prop );
	}

	{	//	Type property
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _type );
		addProperty( property::type::type, prop );
	}

	{	//	Explicit duration
		PropertyImpl<util::DWORD> *prop = new PropertyImpl<util::DWORD>( false, _duration );
		prop->setCheck( boost::bind( std::greater_equal<int>(), _1, 0) );
		prop->setApply( boost::bind(&Player::applyDuration,this) );
		addProperty( property::type::explicitDur, prop );
	}
}

/**
 * Agrega una nueva propiedad al @c Player.
 * @param type Tipo de la propiedad.
 * @param prop Nueva propiedad.
 */
void Player::addProperty( property::type::Type type, Property *prop ) {
	Property *old = getProperty( type );
	if (old) {
		CompositeProperty *tmp = new CompositeProperty();
		tmp->add( old );
		tmp->add( prop );
		_properties[type] = tmp;
	}
	else {
		_properties[type] = prop;
	}
}

Property *Player::getProperty( property::type::Type type ) const {
	Properties::const_iterator it=_properties.find( type );
	if (it != _properties.end()) {
		return (*it).second;
	}
	return NULL;
}

Property *Player::getProperty( const std::string &name ) const {
	return getProperty( property::getProperty( name.c_str() ) );
}

//	Explicit duration
void Player::applyDuration() {
	if (_duration > 0) {
		_timer->addTimer( _duration, boost::bind(&Player::onDurationExpired,this,_duration) );
	}
}

void Player::onDurationExpired( util::DWORD dur ) {
	if (dur == _duration) {
		stop();
	}
}

/**
 * @return El dispositivo en el que corre el @c Player.
 */
Device *Player::device() const {
	return _dev;
}

}

