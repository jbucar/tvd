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

#include "../../../application.h"

namespace connector {
	class Connector;

	template<typename T> class AVDescriptor;
	typedef AVDescriptor<int> NCLVideoDescriptor;
	typedef boost::shared_ptr<NCLVideoDescriptor> NCLVideoDescriptorPtr;

	typedef std::set<util::key::type> KeyRegisterData;
	typedef boost::shared_ptr<KeyRegisterData> KeyRegisterDataPtr;
}

namespace tuner {
namespace app {

//	Types
class PlayerAdapter;
typedef std::map<size_t,PlayerAdapter *> Players;

class NCLApplication : public Application {
public:
	NCLApplication( Extension *ext, const ApplicationID &id, const std::string &script );
	virtual ~NCLApplication();

	virtual void show() const;
	const std::string &script() const;

protected:
	//	Key handling
	virtual void dispatchKeyImpl( util::key::type key, bool isUp );
	void onRegisterKeyEvent( const connector::KeyRegisterDataPtr &data );

	//	Stop handling
	virtual void onStarting( RunTask *task );
	virtual void onStopping();
	virtual void onStopped();
	virtual int exitTimeout() const;

	//	Aux Editing command
	void registerEditingCommands();
	void onEditingCommand( const Buffer &buf );

	//	Aux video
	void onPlayVideoEvent( size_t videoID, const std::string &url );
	void onStopVideoEvent( size_t videoID );
	void onResizeVideoEvent( size_t videoID, const connector::NCLVideoDescriptorPtr &av );
	void destroyPlayers();

	//	Aux
	void onStartPresentation( bool start );

private:
	std::string _script;
	connector::Connector *_connector;
	Players _players;
};

}
}
