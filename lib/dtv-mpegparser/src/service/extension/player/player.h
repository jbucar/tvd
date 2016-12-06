/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "types.h"
#include <map>
#include <boost/function.hpp>

namespace tuner {

class Service;
namespace ca {
	class Conditional;
}

namespace player {

class Extension;
class StreamInfo;
class Delegate;

namespace es {
	class Player;
}

class Player {
public:
	explicit Player( Delegate *delegate );
	virtual ~Player();

	//	Setup
	void addStreams( const std::vector<StreamInfo *> &supported );
	void addDefaultsStreams();
	const std::vector<StreamInfo *> &supportedStreams() const;
	void addPlayer( es::Player *player );

	//	Player initialization
	virtual bool initialize();
	virtual void finalize();

	//	Play service
	void playService( ID srvID );
	void stopService();
	bool isStarted() const;
	typedef boost::function<void (bool)> OnStartedCallback;
	void onStarted( const OnStartedCallback &callback );
	const std::string url() const;

	//	Start/Stop a elementary stream player
	int startAux( Delegate *delegate, pes::type::type pesType, int streamID=-1 );
	void stopAux( int playerID );

	//	Stream methods
	int nextVideo() { return nextStream( pes::type::video ); }
	int videoCount() const  { return streamCount( pes::type::video ); }
	const Stream *getVideo( int id=-1 ) const { return get( id, pes::type::video ); }

	int nextAudio() { return nextStream( pes::type::audio ); }
	int audioCount() const { return streamCount( pes::type::audio ); }
	const Stream *getAudio( int id=-1 ) const { return get( id, pes::type::audio ); }

	int nextSubtitle() { return nextStream( pes::type::subtitle ); }
	int subtitleCount() const { return streamCount( pes::type::subtitle ); }
	const Stream *getSubtitle( int id=-1 ) const { return get( id, pes::type::subtitle ); }

	ID pcrPID() const;
	ID srvID() const;
	const Stream *get( int id, pes::type::type pesType ) const;

	//	Find stream
	const Stream *findStream( fncFindStream fnc );
	const std::string makeURL( const std::string &type );

	//	Extension notifications
	void onStartService( Service *srv );
	void onStopService( Service *srv );
	ca::Conditional *ca() const;
	Extension *extension() const;
	Delegate *delegate() const;

protected:
	typedef std::map<pes::type::type,es::Player *> Players;
	typedef std::map<int,es::Player *> AuxPlayers;

	//	Aux
	void start();
	void onStart();
	void stop();
	void onStop();
	void started( bool value );
	es::Player *getPlayer( pes::type::type pesType ) const;
	int nextStream( pes::type::type pesType );
	int streamCount( pes::type::type pesType ) const;
	StreamInfo *getStreamInfo( const ElementaryInfo &info ) const;
	bool startPCR( ID pid );

private:
	Delegate *_delegate;
	Extension *_ext;
	Players _players;
	AuxPlayers _aux;
	std::vector<StreamInfo *> _supportedStreams;
	OnStartedCallback _onStarted;
	ID _srvID;
	ID _pcrPID;
	bool _needPCRFilter;
	bool _started;
	int _nextAuxPlayer;
};

}
}

