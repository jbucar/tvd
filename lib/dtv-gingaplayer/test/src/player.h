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

#include "util.h"
#include "../../src/types.h"
#include "../../src/system.h"
#include "../../src/player.h"
#include <canvas/types.h>
#include <util/types.h>
#include <boost/thread.hpp>

namespace canvas {
	class System;
	class Canvas;
}

namespace player {
	class Player;
	class System;
	class Device;
	class TimeLineTimer;
}

void init();
void fin();
void run();
void stop();

class Player : public testing::TestWithParam<std::string> {
public:
	Player();
	virtual ~Player();

	void setResultAndNotify( bool result );

	void ref();
	void wait();
	void unref();

protected:
	virtual void SetUp( void );
	virtual void TearDown( void );

	bool play( player::Player *player );
	void stop( player::Player *player );
	void pause( player::Player *player, bool pause );
	template<typename T>
	bool setProperty( player::Player *player, const std::string &name, const T &value );
	void endDraw();

	bool compareImages( canvas::Canvas *canvas, const std::string &file );
	bool compareImagesFromPath( canvas::Canvas *canvas, const std::string &file );
	void compareImagesImpl( canvas::Canvas *canvas, const std::string &file );
	void compareImagesFromPathImpl( canvas::Canvas *canvas, const std::string &file );

	virtual std::string getExpectedPath( const std::string &fileName ) const;
	virtual std::string playerType() const = 0;
	virtual std::string property() const = 0;
	virtual player::type::type type() const = 0;

	canvas::Canvas *canvas();
	player::System *system();
	player::Device *device();

	player::Player *createPlayer( const player::type::type &type ) const;
	player::TimeLineTimer *createTimeLineTimer() const;
	void destroyPlayer( player::Player *player );
	util::id::Ident registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback );

	player::Player *_player;

private:
	void doPlay( player::Player *player );
	void doStop( player::Player *player );
	void doPause( player::Player *player, bool pause );
	void doDestroy( player::Player *player );
	void doEndDraw();

	bool _result;
	boost::condition_variable _cWakeup;
	boost::mutex _mutex;
	int _refs;
};

template<typename T>
class SetPropertyFunctor {
public:
	SetPropertyFunctor( player::Player *player, const std::string &name, const T &value )
		: _player(player), _name(name), _value(value) {}

	void apply( Player *playerTest ) {
		playerTest->setResultAndNotify( _player->setProperty( _name, _value ) );
	}

private:
	player::Player *_player;
	const std::string &_name;
	const T &_value;
};

template<typename T>
inline bool Player::setProperty( player::Player *player, const std::string &name, const T &value ) {
	SetPropertyFunctor<T> f( player, name, value );
	ref();
	system()->enqueue( boost::bind(&SetPropertyFunctor<T>::apply, &f, this ) );
	wait();
	return _result;
}
