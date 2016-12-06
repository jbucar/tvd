/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <boost/signals2.hpp>
#include <boost/function.hpp>
#include <string>
#include <vector>

namespace pvr {

class Tuner;
class Channel;
typedef std::vector<Channel *> ChannelList;

namespace parental {
	class Session;
}

namespace channels {
	class Persister;
}

/**
 * Clase que brinda funcionalidad para el manejo de canales.
 */
class Channels {
public:
	explicit Channels( channels::Persister *persister, parental::Session *session );
	virtual ~Channels();

	//	Load db
	typedef boost::function<Tuner *(const char *name)> FncLoad;
	bool load( const FncLoad &fnc );
	void reset();

	//	Persist
	void beginTransaction();
	void endTransaction();
	void save();

	//	Update methods
	int add( Channel *ch );
	bool remove( int chID );
	void removeAll();

	//	Notifications
	typedef boost::signals2::signal<void (Channel *)> NewChannelSignal;
	NewChannelSignal &onNewChannel();
	typedef boost::signals2::signal<void (Channel *ch,bool all)> ChannelRemovedSignal;
	ChannelRemovedSignal &onChannelRemoved();

	//	Getters
	const ChannelList &getAll() const;
	Channel *get( int chID ) const;

protected:
	void erase( Channel *ch, bool removeAll );

	// Session
	void onSessionEnabledChanged( bool isEnabled );
	bool isProtected() const;

private:
	channels::Persister *_persister;
	parental::Session *_session;
	int _nextID;
	int _transaction;
	ChannelList _channels;
	NewChannelSignal _onNewChannel;
	ChannelRemovedSignal _onChannelRemoved;
	boost::signals2::connection _cEnabled;
};

}

