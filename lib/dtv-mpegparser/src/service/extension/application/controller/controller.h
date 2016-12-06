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

#include "../applicationid.h"
#include <util/keydefs.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

namespace tuner {

class ServiceManager;

namespace app {

class Extension;
class Application;
typedef std::vector<util::key::type> Keys;
class PlayerAdapter;
class RunTask;

class Controller {
public:
	Controller();
	virtual ~Controller();

	//	Create and setup extension
	Extension *initialize();
	void finalize();
	bool isInitialized() const;
	void attach( ServiceManager *mgr );

	//	Start/stop application
	virtual void start( RunTask *task )=0;
	virtual void stop( RunTask *task, bool kill )=0;
	bool canStartApplications() const;
	void disableApplications( bool disable );
	void onChannelBlocked( bool blocked );

	//	Keys
	virtual void reserveKeys( const ApplicationID &id, const Keys &keys, int priority );

	//	Graphic layer
	virtual bool setupLayer( Application *app, int &width, int &heigth );
	virtual void enableLayer( bool st );

	//	Video operations
	virtual PlayerAdapter *createMediaPlayerAdapter() const;
	virtual PlayerAdapter *createElementaryPlayerAdapter( int id ) const;
	virtual PlayerAdapter *createMainPlayerAdapter() const;

	//	Application notifications
	virtual void onApplicationAdded( Application *app );
	virtual void onApplicationRemoved( Application *app );
	virtual void onApplicationChanged( Application *app );
	virtual void onDownloadProgress( Application *app, int step, int total );

	//	Getters
	Extension *extension() const;

protected:
	ServiceManager *mgr() const;
	virtual void setupFactories( Extension *ext );

	//	Start/stop application aux
	void disableMask( util::DWORD mask, bool set );

private:
	Extension *_extension;
	ServiceManager *_mgr;
	util::DWORD _disableMask;
};

}
}

