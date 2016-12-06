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

#include "../tuner.h"
#include <mpegparser/types.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace tuner {
	class Tuner;
	class Extension;
	class Service;
	class ServiceID;
	class ResourceManager;
	class Eit;

	namespace player {
		class Player;
	}

	namespace ewbs {
		class Extension;
	}
}

namespace pvr {
namespace dvb {

namespace id {

struct ChannelIDS {
	size_t networkID;
	tuner::ID nitID;
	tuner::ID tsID;
	tuner::ID srvID;
};
typedef ChannelIDS ChannelID;

std::string toString( const ChannelID &id );
std::string toString( size_t netID, tuner::ID nitID, tuner::ID tsID, tuner::ID srvID );
std::string toString( size_t netID, const tuner::ServiceID &srvID );
bool fromString( const std::string &str, ChannelID &id );

}	//	namespace id

class ScanIterator;
class Delegate;

class Tuner : public pvr::Tuner {
public:
	explicit Tuner( Delegate *delegate );
	virtual ~Tuner();

	//	Scan
	virtual bool isScanning() const;
	virtual void startScan();
	virtual void cancelScan();

	//	Status
	virtual size_t networks() const;
	virtual void status( int &signal, int &quality );

	//	Emergency
	bool inEmergency( tuner::ID srvID ) const;

	//	Implementation
	void attach( tuner::Extension *ext );
	void detach( tuner::Extension *ext );
	bool startService( size_t networkID, tuner::ID srvID );
	void stopService( tuner::ID srvID );

	//	Scan implementation
	void onServiceReady( tuner::Service *srv, bool ready );

	//	Getter
	Delegate *delegate() const;

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();
	virtual TunerPlayer *allocPlayer() const;
	virtual pvr::Show *createDefaultShow() const;

	//	HACK:
	friend class ScanIterator;

	//	EPG
	void onEit( const boost::shared_ptr<tuner::Eit> &eit );

	//	EWBS
	Channel *findEmergencyChannel( tuner::ID srvID );
	void onEmergency( bool state );

private:
	Delegate *_delegate;
	ScanIterator *_scanIter;
	tuner::Tuner *_tuner;
	tuner::ewbs::Extension *_ewbs;
};

}
}

