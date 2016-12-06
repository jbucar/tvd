/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-module implementation.

    DTV-module is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-module is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-module.

    DTV-module es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-module se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include <node/util.h>
#include <pvr/pvr.h>

namespace util {
	namespace task {
		class Dispatcher;
	}

	namespace io {
		class Dispatcher;
	}
}

namespace tvd {

class Session;
class Channels;
class Player;
class Delegate;

namespace ginga {
	class Controller;
}

class Tvd : public node::ObjectWrap {
public:
	Tvd();
	virtual ~Tvd();

	//	Node implementation
	static void Init(v8::Handle<v8::Object> exports);
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

	//	Initialization
	DECL_METHOD(load);
	DECL_METHOD(start);
	DECL_METHOD(stop);

	//	Add logo provider
	DECL_METHOD(addLogoProvider);

	//	Middleware
	DECL_METHOD(enableMiddleware);
	DECL_METHOD(runApplication);
	DECL_METHOD(disableApplications);

	//	Tuners
	DECL_METHOD(isScanning);
	DECL_METHOD(startScan);
	DECL_METHOD(cancelScan);

	//	Channels getter
	DECL_METHOD(channels);

	//	Session getter
	DECL_METHOD(session);

	//	Player getter
	DECL_METHOD(defaultPlayer);

	//	Implementation
	Player *defaultPlayer();
	util::task::Dispatcher *dispatcher() const;
	const std::string &ramDisk() const;
	const std::string &country() const;
	int areaCode() const;

protected:
	static pvr::Pvr *pvr(const v8::FunctionCallbackInfo<v8::Value>& args);

	bool load( const v8::Handle<v8::Object> &obj );
	bool initSettings( const v8::Handle<v8::Object> &obj );
	bool initDispatcher();
	bool initPvr( const v8::Handle<v8::Object> &obj );
	void addTuner( const v8::Handle<v8::Object> &obj );
	bool initialize();
	void finalize();

	Player *allocPlayer();
	void scanChanged( pvr::scan::Type scanState, const pvr::Pvr::ScanNetwork net );

	//	Middleare
	void enableGinga( bool state );
	void runApplication( const std::string &appID );
	void disableApplications( bool state );

private:
	Delegate *_delegate;
	std::string _ramDisk;
	std::string _country;
	int _areaCode;
	util::Settings *_settings;
	pvr::Pvr *_pvr;
	ginga::Controller *_gingaCtrl;
	util::io::Dispatcher *_dispatcher;
	boost::signals2::connection _cScan;
	Channels *_channels;
	Session *_session;
	std::vector<Player *> _players;
	static v8::Persistent<v8::Function> _constructor;
};

}	//	namespace tvd

