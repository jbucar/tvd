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
#include <vector>
#include <boost/thread/mutex.hpp>

struct uv_async_s;
typedef struct uv_async_s uv_async_t;

namespace CEC {
	struct libcec_configuration;
	typedef struct libcec_configuration libcec_configuration;
	typedef struct ICECCallbacks ICECCallbacks;

	class ICECAdapter;
}

namespace tvd {

namespace impl {
	class AsyncCallback;
	class Notify;
}

class Cec : public node::ObjectWrap {
public:
	Cec();
	virtual ~Cec();

	//	Node implementation
	static void Init(v8::Handle<v8::Object> exports);
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

	//	Initialization
	DECL_METHOD(open);
	DECL_METHOD(close);
	DECL_METHOD(isOpen);

	//	Adapter methods
	DECL_METHOD(pingAdapter);
	DECL_METHOD(startBootloader);
	DECL_METHOD(transmit);
	DECL_METHOD(setLogicalAddress);
	DECL_METHOD(setPhysicalAddress);
	DECL_METHOD(powerOnDevices);
	DECL_METHOD(standbyDevices);
	DECL_METHOD(setActiveSource);
	DECL_METHOD(setDeckControlMode);
	DECL_METHOD(setDeckInfo);
	DECL_METHOD(setInactiveView);
	DECL_METHOD(setMenuState);
	DECL_METHOD(setOSDString);
	DECL_METHOD(switchMonitoring);
	DECL_METHOD(getDeviceCecVersion);
	DECL_METHOD(getDeviceMenuLanguage);
	DECL_METHOD(getDeviceVendorId);
	DECL_METHOD(getDevicePowerStatus);
	DECL_METHOD(pollDevice);
	DECL_METHOD(getActiveDevices);
	DECL_METHOD(isActiveDevice);
	DECL_METHOD(isActiveDeviceType);
	DECL_METHOD(volumeUp);
	DECL_METHOD(volumeDown);
	DECL_METHOD(muteAudio);
	DECL_METHOD(sendKeypress);
	DECL_METHOD(sendKeyRelease);
	DECL_METHOD(getDeviceOSDName);
	DECL_METHOD(getActiveSource);
	DECL_METHOD(isActiveSource);
	DECL_METHOD(setStreamPath);
	DECL_METHOD(getLogicalAddress);
	DECL_METHOD(getLogicalAddresses);
	DECL_METHOD(getCurrentConfiguration);
	DECL_METHOD(setConfiguration);
	DECL_METHOD(canPersistConfiguration);
	DECL_METHOD(persistConfiguration);
	DECL_METHOD(rescanActiveDevices);
	DECL_METHOD(isLibCECActiveSource);
	DECL_METHOD(getDeviceInformation);
	DECL_METHOD(enableCallbacks);
	DECL_METHOD(setHDMIPort);
	DECL_METHOD(getDevicePhysicalAddress);
	DECL_METHOD(getLibInfo);
	DECL_METHOD(getAdapterVendorId);
	DECL_METHOD(getAdapterProductId);
	DECL_METHOD(audioToggleMute);
	DECL_METHOD(audioMute);
	DECL_METHOD(audioUnmute);
	DECL_METHOD(audioStatus);
	DECL_METHOD(detectAdapters);

	//	Configuration methods
	DECL_METHOD(getClientVersion);
	DECL_METHOD(getServerVersion);
	DECL_METHOD(getFirmwareVersion);

	bool doOpenAsync( const std::string &port );
	void doCloseAsync();
	void setOpen( bool result );
	void doAsync( impl::AsyncCallback *data );
	void notify( const std::string &event, const v8::Handle<v8::Object> &evt );
	void send( impl::Notify *notify );
	void wakeup();
	void keyPress( int key, int duration );
	void sourceActivated( int laddr, bool isActive );

protected:
	static CEC::ICECAdapter *adapter(const v8::FunctionCallbackInfo<v8::Value>& args, bool checkIsOpen=true );
	bool doOpen();

private:
	CEC::libcec_configuration *_cfg;
	CEC::ICECCallbacks *_callbacks;
	CEC::ICECAdapter *_adapter;
	bool _isOpen;
	uv_async_t *_wakeup;
	std::vector<impl::Notify *> _notifications;
	boost::mutex _mNotify;
	static v8::Persistent<v8::Function> _constructor;
};

}	//	namespace tvd

