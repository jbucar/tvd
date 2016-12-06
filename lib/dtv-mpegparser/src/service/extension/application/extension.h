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

#include "applicationid.h"
#include "../extension.h"
#include <util/keydefs.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace tuner {

namespace dsmcc {
	class DataCarouselHelper;
}

namespace app {

class Application;
class Factory;
class Controller;
class FindApplicationIterator;

class Extension : public tuner::Extension {
public:
	Extension( Controller *ctrl );
	virtual ~Extension();

	//	Add factories
	void addFactory( Factory *factory );

	//	Add/remove/start/stop application (Backend API)
	void add( Application *app );
	void remove( const ApplicationID &appID );
	void removeAll();
	void removeScanned();
	void changePriority( const ApplicationID &appID, util::BYTE priority );

	void start( const ApplicationID &id );
	void stop( const ApplicationID &id, bool kill=false );
	void stopAll();
	void queueAll();
	void startQueued();
	void download( const ApplicationID &appID );
	typedef boost::function<void (void)> EndScanApplicationCallback;
	void scan( const std::string path, int maxDepth, const EndScanApplicationCallback &callback=EndScanApplicationCallback() );
	void dispatchKey( const ApplicationID &appID, util::key::type key, bool isUp );
	bool dispatchKey( util::key::type key, bool isUp );

	//	Aux download
	ID findTag( util::BYTE tag, ID service );

	//	Service Provider notifications
	virtual void onServiceStarted( Service *srv, bool started );

	//	Getters
	inline Controller *ctrl();

protected:
	//	Aux initialization
	virtual bool onInitialize();
	virtual void onFinalize();

	//	Aux applications
	void remove( FindApplicationIterator *iter );
	template<class T> inline bool loopApplications( const T &finder, const boost::function<void (Application *app)> &fnc );

	//	Aux filesystem
	void scanApplications( const std::string &appPath, int maxDepth, int curDepth );

private:
	Controller *_ctrl;
	std::vector<Application *> _applications;
	std::vector<Factory *> _factories;

	//	Object carousel transport
	dsmcc::DataCarouselHelper *_dsmcc;

	ID _appID;
};

//	Getters
inline Controller *Extension::ctrl() {
	return _ctrl;
}

template<class T>
inline bool Extension::loopApplications( const T &finder, const boost::function<void (Application *app)> &fnc ) {
	bool any=false;
	std::vector<Application *>::iterator it=_applications.begin();
	while (it != _applications.end()) {
		Application *app = (*it);
		if (finder(app)) {
			fnc( app );
			any=true;
		}
		it++;
	}
	return any;
}

}
}
