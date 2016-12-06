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

#pragma once

#include "types.h"
#include "property/types.h"
#include "property/property.h"
#include "player/lua/event/types.h"
#include <util/log.h>
#include <boost/function.hpp>
#include <string>
#include <map>

namespace canvas {
	class System;
}

namespace player {

class TimeLineTimer;
class Device;

/**
 * Las instancias de @c Player reproducen distintos tipos de contenidos como pueden ser textos, imágenes, animaciones, 
 * htmls, scripts lua, sonidos y videos. Cada @c Player contiene funcionalidad común agrupada en la superclase Player y 
 * comportamiento específico relacionado con el tipo de contenido que muestra. La finalidad de los players es brindar una capa 
 * de abstracción por sobre los detalles de presentación del contenido, que mediante métodos genéricos podrán ser objeto de 
 * operaciones como, por ejemplo, stop, pause y play. Si se desea implementar nuevos @c Players que representen otros 
 * elementos sólo es necesario crear una clase que reimplemente todos o algunos, dependiendo de el elemento a representar, de 
 * los métodos virtuales, tanto públicos como protegidos, de la interfaz de @c Player.
 */
class Player {
public:
	explicit Player( Device *dev );
	virtual ~Player();

	//	Initialization
	bool initialized() const;
	bool initialize();
	void finalize();

	//	Media
	bool isPlaying() const;
	bool isPaused() const;
	bool play();
	void stop();
	void abort();
	void pause(bool pause);

	//	Properties
	template<typename T>
	bool setProperty( const std::string &name, const T &value );
	void addProperty( property::type::Type type, Property *prop );
	std::string rootPath() const;
	util::DWORD uptime() const;
	virtual bool isApplication() const;
	virtual bool isVisible() const;
	virtual bool isGraphic() const;
	static void convertText( std::string &text );

	//	Callback initialization
	virtual void setEventCallback( const event::Callback &callback );
	virtual void setInputEventCallback( const event::InputCallback &callback );

	//	Timers
	typedef boost::function<void (void)> TimerCallback;
	void createTimer( util::DWORD ms, const TimerCallback &fnc );

	//	Signals
	typedef boost::function<void (void)> OnStatusChanged;
	typedef boost::function<void (bool)> OnPauseStatusChanged;

	void onStarted(const OnStatusChanged &fnc);
	void onStopped(const OnStatusChanged &fnc);
	void onPaused(const OnPauseStatusChanged &fnc);

	//	Getters
	Device *device() const;

protected:
	//	Types
	typedef std::map<property::type::Type,Property *> Properties;
	
	virtual bool init();
	virtual void fin();
	
	virtual bool startPlay()=0;
	virtual void stopPlay()=0;
	virtual void abortPlay();
	virtual void pausePlay( bool pause );
	virtual bool canPlay() const;
	virtual void beginRefresh();
	virtual void refresh();
	virtual void endRefresh();
	virtual void onPropertyChanged();
	void markModified();
	void applyChanges();
	void apply( bool needRefresh );

	//	Properties
	virtual void registerProperties();
	Property *getProperty( property::type::Type type ) const;	
	Property *getProperty( const std::string &name ) const;

	//	Source
	const std::string &url() const;
	schema::type schema() const;
	const std::string &body() const;
	const std::string &type() const;
	virtual bool supportSchemma( schema::type sch ) const;
	virtual bool supportRemote() const;
	bool checkUrl( const std::string &url );	

	//	Aux explicit duration
	void applyDuration();
	void onDurationExpired( util::DWORD dur );

private:
	Device *_dev;	
	std::string _url;
	std::string _type;
	std::string _body;
	schema::type _schema;
	util::DWORD _duration;
	bool _pause;
	bool _playing;
	bool _initialized;
	Properties _properties;
	TimeLineTimer *_timer;
	OnStatusChanged _onStarted, _onStopped;
	OnPauseStatusChanged _onPaused;

	Player() {}
};

/**
 * Configura una propiedad.
 * @param name Nombre de la propiedad.
 * @param value Valor de la propiedad.
 * @return True si se ha podido configurar la propiedad con éxito, false caso contrario.
 */
template<typename T>
inline bool Player::setProperty( const std::string &name, const T &value ) {
	bool result=false;

	LOG_PUBLISH( DEBUG, "gingaplayer", "Player", "Set property begin: name=%s", name.c_str());
	
	//	Get property
	Property *prop = getProperty( name );
	if (!prop) {
		LOG_PUBLISH( WARN, "gingaplayer", "Player", "property not supported: %s", name.c_str());
		return result;
	}
	
	if (!isPlaying() && prop->isOnlyDynamic()) {
		LOG_PUBLISH( WARN, "gingaplayer", "Player", "property only supported when player has started: %s", name.c_str());
		return result;
	}

	try {
		//	Assign value, property changed?
		if (prop->assign( value ) && isPlaying()) {
			//	Dynamic property
			apply( prop->apply() );
		}

		//	Assign was ok!
		result=true;
	} catch ( std::exception &e ) {
		LOG_PUBLISH( WARN, "gingaplayer", "Player", "cannot set property: name=%s, error=%s", name.c_str(), e.what());
	}

	LOG_PUBLISH( DEBUG, "gingaplayer", "Player", "Set property end: name=%s, result=%d", name.c_str(), result);
	return result;
}

}

