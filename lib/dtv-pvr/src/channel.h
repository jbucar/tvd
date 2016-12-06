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

#include "types.h"
#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <string>

namespace pvr {

namespace bpt=boost::posix_time;

class Tuner;
class Show;

namespace parental {
	class Control;
}

/**
 * Representa un canal.
 */
class Channel {
public:
	Channel();
	Channel( const std::string &name, const std::string &channel, const std::string &network );
	virtual ~Channel();

	//	Channel information
	int channelID() const;
	const std::string &name() const;
	const std::string &channel() const;
	const std::string &network() const;
	const std::string &logo();
	bool isMobile() const;
	void mobile( bool isMobile );
	bool isFree() const;
	void isFree( bool isFree );

	//	User
	bool isFavorite() const;
	void toggleFavorite();
	bool isBlocked() const;
	void toggleBlocked();
	void unblock();

	//	Current information
	bool isProtected() const;
	category::Type category() const;

	//	Shows
	typedef std::vector<Show *> Shows;
	void addShow( Show *s );
	Show *getCurrentShow() const;
	Show *getShow( const bpt::ptime &t ) const;
	void getShowsBetween( Shows &shows, const bpt::time_period &period ) const;

	//	Parental
	parental::Control *parentalControl() const;
	void parentalControl( parental::Control *parental );

	//	Notifications
	typedef boost::signals2::signal<void (Channel *)> Changed;
	Changed &onChanged();

	//	Implementation
	bool isOnline() const;
	bool isProtected( parental::Control *control ) const;
	const std::string &uri() const;
	void uri( const std::string &id );
	void tuner( Tuner *tuner );
	Tuner *tuner() const;
	const std::string &tunerName() const;
	void channelID( int channelID );

	template<class Marshall>
	void serialize( Marshall &m ) {
		m.serialize( "channelID", _channelID );
		m.serialize( "name", _name );
		m.serialize( "channel", _channel );
		m.serialize( "tuner", _tunerName );
		m.serialize( "network", _network );
		m.serialize( "isMobile", _isMobile );
		m.serialize( "isFree", _isFree );
		m.serialize( "isFavorite", _isFavorite );
		m.serialize( "isBlocked", _isBlocked );
		m.serialize( "uri", _uri );
	}

	void debug() const;

protected:
	void notify( bool markToSave=false );
	Show *getCurrentShow( bool excludeProtected ) const;
	Show *getShow( const bpt::ptime &t, bool excludeProtected ) const;

private:
	Tuner *_tuner;
	int _channelID;
	std::string _name;
	std::string _channel;
	std::string _network;
	std::string _tunerName;
	std::string _uri;
	std::string _logo;
	bool _isFavorite;
	bool _isBlocked;
	bool _isMobile;
	bool _isFree;
	Changed _onChanged;
	std::vector<Show *> _shows;
	parental::Control *_parental;
};

}

