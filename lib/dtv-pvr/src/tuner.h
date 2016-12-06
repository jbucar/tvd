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

#include <string>
#include <queue>
#include <vector>

namespace pvr {

namespace logos {
	class Service;
}

namespace parental {
	class Session;
}

class Show;
class Channel;
class Channels;
class TunerPlayer;

class TunerDelegate {
public:
	TunerDelegate() {}
	virtual ~TunerDelegate() {}

	//	Getters
	virtual logos::Service *logos() const=0;
	virtual Channels *channels() const=0;
	virtual parental::Session *session() const=0;

	//	Scan notifications
	virtual void onStartNetworkScan( const std::string & /*tuner*/, const std::string & /*net*/ ) {}
	virtual void onEndScan( const std::string & /*tuner*/ ) {}

	//	Emergency
	virtual void onEmergency( bool /*state*/, int /*ch*/ ) {}
};

class Tuner {
public:
	explicit Tuner( const std::string &name );
	virtual ~Tuner();

	//	Initialization
	bool initialize( TunerDelegate *delegate );
	void finalize();
	bool isInitialized() const;

	//	Scan
	virtual bool isScanning() const;
	virtual void startScan();
	virtual void cancelScan();

	//	Status
	virtual void status( int &signal, int &quality );
	virtual size_t networks() const;

	//	Logos
	const std::string findLogo( Channel *ch ) const;

	//	Players
	TunerPlayer *defaultPlayer();
	TunerPlayer *reservePlayer();
	void freePlayer( TunerPlayer *p );
	size_t allocatedPlayers() const;

	//	Getters
	const std::string &name() const;
	parental::Session *session() const;
	Channels *channels() const;
	Show *defaultShow();

protected:
	virtual bool init();
	virtual void fin();
	virtual TunerPlayer *allocPlayer() const=0;
	virtual int maxPlayers() const;
	virtual bool fetchStatus( int &signal, int &quality );
	virtual Show *createDefaultShow() const;

	//	Scan methods
	void addChannel( Channel *ch );
	void onStartNetworkScan( const std::string &net );
	void onEndScan();

	//	Emergency
	void notifyEmergency( bool state, int chID=-1 );

	//	Getters
	logos::Service *logos() const;
	Channel *findChannel( const std::string &uri ) const;
	TunerDelegate *delegate() const;

private:
	std::string _name;
	Show *_defaultShow;
	std::queue<TunerPlayer *> _freePlayers;
	std::vector<TunerPlayer *> _players;
	TunerDelegate *_delegate;
};

}

