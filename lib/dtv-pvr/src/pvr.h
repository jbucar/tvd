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
#include <boost/optional.hpp>
#include <vector>
#include <string>

namespace util {
	class Settings;
}

namespace pvr {

class Channel;
class Player;
class Tuner;
class Channels;
class PvrDelegate;

namespace logos {
	class Service;
}

namespace parental {
	class Session;
}

class Pvr {
public:
	Pvr( util::Settings *settings, const std::string &file="" );
	virtual ~Pvr();

	//	Initialization
	void addTuner( Tuner *tuner );
	bool initialize();
	void finalize();
	void resetConfig();

	//	Scanning
	bool isScanning() const;
	void startScan();
	void cancelScan();
	typedef boost::optional<std::string> ScanNetwork;
	typedef boost::signals2::signal<void (scan::Type,const ScanNetwork)> ScanSignal;
	ScanSignal &onScanChanged();
	size_t networks() const;

	//	Players
	Player *allocPlayer( bool useDefault=true );
	typedef Player * PlayerPtr;
	void freePlayer( PlayerPtr &p );
	Player *defaultPlayer() const;

	//	Getters
	Channels *channels() const;
	parental::Session *session() const;
	util::Settings *settings() const;
	logos::Service *logos() const;

protected:
	//	Types
	typedef std::vector<Tuner *> Tuners;
	typedef std::vector<Player *> Players;

	//	Tuner notifications
	void onStartNetworkScan( const std::string &tuner, const std::string &net );
	void onEndScan( const std::string &tuner );
	void onEmergency( bool state, int ch );

	//	Getters
	Tuner *tuner( const std::string &name ) const;
	friend class PvrDelegate;

private:
	util::Settings *_settings;
	PvrDelegate *_tunerDelegate;
	Tuners _tuners;
	Players _players;
	Player *_defaultPlayer;
	Channels *_channels;
	logos::Service *_logos;
	parental::Session *_session;
	bool _initialized;

	ScanSignal _onScanChanged;
	size_t _tunersInScan;
};

}

