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

#include "module.h"
#include "generated/config.h"
#include <util/assert.h>
#include <util/main.h>
#include <util/string.h>
#include <util/log.h>
#include <util/mcr.h>
#include <node/uv.h>
#include <libcec/cec.h>
#include <libcec/cectypes.h>

// NOTE:
// NodeJs c++ modules docs: http://nodejs.org/api/addons.html
// V8 docs: https://developers.google.com/v8/

#define DEVICE_NAME "node"

#define OBJ(p)	  ({ \
	Cec *obj = node::ObjectWrap::Unwrap<Cec>(p.This()); \
	DTV_ASSERT(obj); \
	obj; })

#define SCOPE	  \
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); \
	DTV_ASSERT(isolate); \
	v8::HandleScope scope(isolate);

#define INIT	  \
	SCOPE; \
	Cec *cec = OBJ(args); \
	if (!cec) { \
		THROW_RETURN("Invalid CEC object"); \
	}

#define CHECK_OPEN	  \
	if (!cec->_isOpen) { \
		THROW_RETURN("CEC object not opened"); \
	}

#define CHECK_PARAMS(l)	\
	if (args.Length() != l || !args[l-1]->IsFunction()) { \
		THROW_RETURN("Invalid arguments"); \
	}


#define NEW_BASIC_P1(tResult,method,tParam,param)	  \
	new impl::BasicAsyncP1<tResult,tParam>( args[1], cec->_adapter, &CEC::ICECAdapter::method, param)

#define DO_ASYNC(obj)	\
	INIT; \
	CHECK_OPEN; \
	CHECK_PARAMS(1); \
	cec->doAsync( obj ); \
	FIN;

#define DO_ASYNC_BASIC(tResult,method)	  \
	DO_ASYNC( new impl::BasicAsync<tResult>( args[0], cec->_adapter, &CEC::ICECAdapter::method) );

#define DO_ASYNC_BASIC_P1(tResult,method)	\
	INIT; \
	CHECK_OPEN; \
	CHECK_PARAMS(2); \
	int lAddr; \
	if (!impl::get( args[0], lAddr )) { \
		THROW_RETURN("Invalid argument #0"); \
	} \
	CEC::cec_logical_address addr = (CEC::cec_logical_address)lAddr; \
	cec->doAsync( NEW_BASIC_P1( tResult, method, CEC::cec_logical_address, addr ) ); \
	FIN;


namespace tvd {

//	Node implementation
v8::Persistent<v8::Function> Cec::_constructor;

inline v8::Local<v8::Integer> _val( CEC::cec_logical_addresses addrs ) {
	int result = -1;
	if (!addrs.IsEmpty()) {
		result = addrs.primary;
	}
	return _val(result);
}

namespace impl {

static void CecLogMessage( void * /*cbParam*/, const CEC::cec_log_message *message ) {
	LOG_LEVEL_TYPE tvdLevel = LOG_LEVEL_UNKNOWN;
	switch (message->level) {
		case CEC::CEC_LOG_ERROR:
			tvdLevel = LOG_LEVEL_ERROR;
			break;
		case CEC::CEC_LOG_WARNING:
			tvdLevel = LOG_LEVEL_WARN;
			break;
		case CEC::CEC_LOG_NOTICE:
			tvdLevel = LOG_LEVEL_INFO;
			break;
		case CEC::CEC_LOG_DEBUG:
			tvdLevel = LOG_LEVEL_DEBUG;
			break;
		case CEC::CEC_LOG_TRAFFIC:
			tvdLevel = LOG_LEVEL_TRACE;
			break;
		default:
			tvdLevel = LOG_LEVEL_NONE;
			break;
	};

	if (util::log::canLog( tvdLevel, "libcecnode", "libcec" )) {
		util::log::log( tvdLevel, "libcecnode", "libcec", "%s", message->message );
	}
}

class Notify {
public:
	explicit Notify( Cec *cec, const boost::function<void (void)> &cb ) : _cec(cec), _cb(cb) {}
	virtual ~Notify() {}

	virtual void run() { _cb(); }

protected:
	Cec *cec() const { return _cec; }

private:
	Cec *_cec;
	boost::function<void (void)> _cb;
};

static void CecKeyPress(void *cbParam, const CEC::cec_keypress *key) {
	DTV_ASSERT(cbParam);
	Cec *cec = (Cec *)cbParam;
	cec->send( new Notify( cec, boost::bind(&Cec::keyPress,cec,(int)key->keycode,(int)key->duration) ) );
}

static void CecSourceActivated( void *cbParam, const CEC::cec_logical_address addr, const uint8_t active ) {
	DTV_ASSERT(cbParam);
	Cec *cec = (Cec *)cbParam;
	cec->send( new Notify( cec, boost::bind(&Cec::sourceActivated,cec,(int)addr, active ? true : false) ) );
}

static void wakeup_cb( uv_async_t*handle ) {
	DTV_ASSERT(handle);
	DTV_ASSERT(handle->data);
	Cec *cec = (Cec *)handle->data;
	cec->wakeup();
}

static void wakeup_close_cb(uv_handle_t* handle) {
	uv_async_t *real_handle = (uv_async_t *)handle;
	free(real_handle);
}

class AsyncCallback {
public:
	explicit AsyncCallback( const v8::Local<v8::Value> &js )
	{
		DTV_ASSERT(js->IsFunction());
		_jsCb.Reset( v8::Isolate::GetCurrent(), v8::Local<v8::Function>::Cast(js) );
	}
	virtual ~AsyncCallback() {}

	virtual void run()=0;
	virtual void notify()=0;

protected:
	void notify( int argc, v8::Handle<v8::Value> *argv ) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::TryCatch tryCatch;
		tryCatch.SetVerbose(true);
		v8::Local<v8::Function> findFunc = v8::Local<v8::Function>::New(isolate, _jsCb);
		findFunc->Call( isolate->GetCurrentContext()->Global(), argc, argv );
		if (tryCatch.HasCaught()) {
			std::string msg;
			impl::get( tryCatch.Message()->Get(), msg );
			LERROR( "cec", "Catch javascript exception from javascript callback function: line=%d, msg=%s",
				tryCatch.Message()->GetLineNumber(), msg.c_str() );
		}
	}

	template<int argc,typename R>
	void notify( R result ) {
		SCOPE;

		//	Setup arguments
		v8::Handle<v8::Value> argv[argc];
		argv[0] = v8::Null(isolate);
		argv[1] = _val(result);

		AsyncCallback::notify( argc, argv );
	}

private:
	v8::Persistent<v8::Function> _jsCb;
};

template<typename R>
class BasicAsync : public AsyncCallback {
public:
	typedef R (CEC::ICECAdapter::*PtrFunction)();
	BasicAsync( const v8::Local<v8::Value> &js, CEC::ICECAdapter *adapter, PtrFunction fnc )
		: AsyncCallback( js ), _adapter(adapter), _fnc(fnc) {}
	virtual ~BasicAsync() {}

	virtual void run() {
		_result = (_adapter->*_fnc)();
	}

	virtual void notify() {
		AsyncCallback::notify<2>( _result );
	}

private:
	CEC::ICECAdapter *_adapter;
	R _result;
	PtrFunction _fnc;
};

template<typename R,typename P>
class BasicAsyncP1 : public AsyncCallback {
public:
	typedef R (CEC::ICECAdapter::*PtrFunction)( P p );

	BasicAsyncP1( const v8::Local<v8::Value> &js, CEC::ICECAdapter *adapter, PtrFunction fnc, P param1 )
		: AsyncCallback( js ), _adapter(adapter), _fnc(fnc), _p1(param1) {}
	virtual ~BasicAsyncP1() {}

	virtual void run() {
		_result = (_adapter->*_fnc)(_p1);
	}

	virtual void notify() {
		AsyncCallback::notify<2>( _result );
	}

private:
	CEC::ICECAdapter *_adapter;
	R _result;
	PtrFunction _fnc;
	P _p1;
};

class CloseAsync : public AsyncCallback {
public:
	CloseAsync( const v8::Local<v8::Value> &js, Cec *cec )
		: AsyncCallback( js ), _cec(cec) {}
	virtual ~CloseAsync() {}

	virtual void run() {
		_cec->doCloseAsync();
	}

	virtual void notify() {
		SCOPE;

		_cec->setOpen(false);

		//	Setup arguments
		v8::Handle<v8::Value> argv[1];
		argv[0] = v8::Null(isolate);

		AsyncCallback::notify( 1, argv );
	}

private:
	Cec *_cec;
	std::string _port;
};

class OpenAsync : public AsyncCallback {
public:
	OpenAsync( const v8::Local<v8::Value> &js, Cec *cec, const std::string &port )
		: AsyncCallback( js ), _cec(cec), _result(false), _port(port) {}
	virtual ~OpenAsync() {}

	virtual void run() {
		_result = _cec->doOpenAsync(_port.c_str());
	}

	virtual void notify() {
		_cec->setOpen(_result);
		AsyncCallback::notify<2>( _result );
	}

private:
	Cec *_cec;
	bool _result;
	std::string _port;
};

class GetActivesAsync : public AsyncCallback {
public:
	GetActivesAsync( const v8::Local<v8::Value> &js, CEC::ICECAdapter *adapter )
		: AsyncCallback( js ), _adapter(adapter) {}
	virtual ~GetActivesAsync() {}

	virtual void run() {
		CEC::cec_logical_addresses addrs = _adapter->GetActiveDevices();

		//	Get logical devices actives
		for (uint8_t iPtr = 0; iPtr < 16; iPtr++) {
			if (addrs[iPtr]) {
				_devices.push_back( (int) iPtr );
			}
		}
	}

	virtual void notify() {
		SCOPE;

		v8::Handle<v8::Array> arr = v8::Array::New( isolate, _devices.size() );
		for (size_t i=0; i<_devices.size(); i++) {
			arr->Set( i, _val( _devices[i] ) );
		}

		//	Setup arguments
		v8::Handle<v8::Value> argv[2];
		argv[0] = v8::Null(isolate);
		argv[1] = arr;

		AsyncCallback::notify( 2, argv );
	}

private:
	CEC::ICECAdapter *_adapter;
	std::vector<int> _devices;
};

static void DoAsync(uv_work_t *req) {
	DTV_ASSERT(req->data);
	AsyncCallback *cb = (AsyncCallback *)req->data;
	cb->run();
}

static void DoAsyncAfter(uv_work_t *req) {
	DTV_ASSERT(req->data);
	AsyncCallback *cb = (AsyncCallback *)req->data;
	cb->notify();
	delete cb;
	delete req;
}

}

Cec::Cec()
{
	//	Setup configuration
	_cfg = new CEC::libcec_configuration;
	_cfg->Clear();
	snprintf(_cfg->strDeviceName, strlen(DEVICE_NAME), DEVICE_NAME);
	_cfg->bActivateSource = 1;
	_cfg->deviceTypes.Add(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);

	//	Setup callbacks
	_callbacks = new CEC::ICECCallbacks();
	_callbacks->Clear();
	_callbacks->logMessage = &impl::CecLogMessage;
	_callbacks->keyPress = &impl::CecKeyPress;
	// g_callbacks->CBCecCommand     = &CecCommand;
	// g_callbacks->CBCecAlert       = &CecAlert;
	_callbacks->sourceActivated = &impl::CecSourceActivated;

	_cfg->callbackParam = this;
	_cfg->callbacks = _callbacks;

	//	Create CEC instance
	_adapter = static_cast<CEC::ICECAdapter*>(CECInitialise(_cfg));
	DTV_ASSERT(_adapter);
	_adapter->InitVideoStandalone();
	_isOpen = false;
	_wakeup = NULL;
}

Cec::~Cec()
{
	CECDestroy(_adapter);
	delete _cfg;
	delete _callbacks;
}

CEC::ICECAdapter *Cec::adapter(const v8::FunctionCallbackInfo<v8::Value>& args, bool checkIsOpen/*=true*/) {
	CEC::ICECAdapter *adapter = NULL;
	Cec *obj = OBJ(args);
	if ((checkIsOpen && obj->_isOpen) || !checkIsOpen) {
		adapter = obj->_adapter;
		DTV_ASSERT(adapter);
	}
	return adapter;
}

void Cec::notify( const std::string &event, const v8::Handle<v8::Object> &evt ) {
	v8::Handle<v8::Value> argv[2];
	argv[0] = _val(event);
	argv[1] = evt;

	//	Call
	makeCallback( this, "emit", 2, argv );
}

void Cec::send( impl::Notify *notify ) {
	_mNotify.lock();
	_notifications.push_back(notify);
	_mNotify.unlock();

	//	wakeup main loop
	uv_async_send( _wakeup );
}

void Cec::wakeup() {
	std::vector<impl::Notify *> tmp;
	_mNotify.lock();
	std::swap(tmp,_notifications);
	_mNotify.unlock();

	BOOST_FOREACH( impl::Notify *notify, tmp ) {
		notify->run();
		delete notify;
	}
}

void Cec::keyPress( int key, int duration ) {
	SCOPE;

	v8::Handle<v8::Object> evt = v8::Object::New(isolate);
	setValue( evt, "keyCode", key );
	setValue( evt, "duration", duration );

	notify( "keyPress", evt );
}

void Cec::sourceActivated( int laddr, bool isActive ) {
	SCOPE;

	v8::Handle<v8::Object> evt = v8::Object::New(isolate);
	setValue( evt, "laddr", laddr );
	setValue( evt, "active", isActive );

	notify( "sourceActivated", evt );
}

void Cec::Init(v8::Handle<v8::Object> exports) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, Cec::New);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(_val("Cec"));

	//	Prototype: Initialization
	NODE_SET_PROTOTYPE_METHOD(tpl, "open", Cec::open);
	NODE_SET_PROTOTYPE_METHOD(tpl, "close", Cec::close);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isOpen", Cec::isOpen);
	NODE_SET_PROTOTYPE_METHOD(tpl, "detectAdapters", Cec::detectAdapters);

	//	Adapter methods
	NODE_SET_PROTOTYPE_METHOD(tpl, "getLogicalAddress", Cec::getLogicalAddress);

	NODE_SET_PROTOTYPE_METHOD(tpl, "pingAdapter", Cec::pingAdapter);
	NODE_SET_PROTOTYPE_METHOD(tpl, "startBootloader", Cec::startBootloader);
	NODE_SET_PROTOTYPE_METHOD(tpl, "transmit", Cec::transmit);

	NODE_SET_PROTOTYPE_METHOD(tpl, "getActiveDevices", Cec::getActiveDevices);
	NODE_SET_PROTOTYPE_METHOD(tpl, "getActiveSource", Cec::getActiveSource);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isActiveSource", Cec::isActiveSource);
	NODE_SET_PROTOTYPE_METHOD(tpl, "setActiveSource", Cec::setActiveSource);

	NODE_SET_PROTOTYPE_METHOD(tpl, "getDeviceVendorId", Cec::getDeviceVendorId );
	NODE_SET_PROTOTYPE_METHOD(tpl, "getDeviceOSDName", Cec::getDeviceOSDName );
	NODE_SET_PROTOTYPE_METHOD(tpl, "getDeviceCecVersion", Cec::getDeviceCecVersion );
	NODE_SET_PROTOTYPE_METHOD(tpl, "getDevicePowerStatus", Cec::getDevicePowerStatus );

	NODE_SET_PROTOTYPE_METHOD(tpl, "getClientVersion", Cec::getClientVersion );
	NODE_SET_PROTOTYPE_METHOD(tpl, "getServerVersion", Cec::getServerVersion );
	NODE_SET_PROTOTYPE_METHOD(tpl, "getFirmwareVersion", Cec::getFirmwareVersion );

	_constructor.Reset(isolate, tpl->GetFunction());
	exports->Set(_val("Cec"),tpl->GetFunction());
}

void Cec::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	DTV_ASSERT(args.IsConstructCall());

	Cec* self = new Cec();
	self->Wrap(args.This());

	args.GetReturnValue().Set(args.This());
}

void Cec::doAsync( impl::AsyncCallback *data ) {
	//	Setup handler
	uv_work_t *req = new uv_work_t;
	req->data = data;
	uv_queue_work(
		uv_default_loop(),
		req,
		impl::DoAsync,
		(uv_after_work_cb)impl::DoAsyncAfter);
}

void Cec::open( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	INIT;
	if (cec->_isOpen) {
		THROW_RETURN("Cec already open" );
	}
	CHECK_PARAMS(2);

	std::string port;
	if (!impl::get( args[0], port )) {
		THROW_RETURN("Invalid parameter #0: \"port\"" );
	}

	if (!cec->doOpen()) {
		THROW_RETURN("Cannot setup wakeup handler" );
	}

	LINFO("cec", "open: port=%s", port.c_str() );
	cec->doAsync( new impl::OpenAsync(args[1],cec,port) );

	FIN;
}

bool Cec::doOpenAsync( const std::string &port ) {
	DTV_ASSERT(_adapter);
	return _adapter->Open( port.c_str() );
}

bool Cec::doOpen() {
	//	Create and initialize wakeup watcher
	_wakeup = (uv_async_t *)calloc( 1, sizeof(uv_async_t) );
	if (!_wakeup) {
		LWARN("io::uv::Dispatcher", "cannot create async event");
		return false;
	}
	_wakeup->data = this;
	uv_async_init(
		uv_default_loop(),
		_wakeup,
		impl::wakeup_cb
	);

	return true;
}

void Cec::doCloseAsync() {
	DTV_ASSERT(_adapter);
	_adapter->Close();
}

void Cec::setOpen( bool result ) {
	_isOpen = result;

	//	On close, close wakeup handle
	if (!_isOpen && _wakeup) {
		//	Close async
		uv_close( (uv_handle_t *)_wakeup, impl::wakeup_close_cb );
	}
}

void Cec::isOpen( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	INIT;
	args.GetReturnValue().Set(_val(cec->_isOpen));
}

void Cec::close( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	INIT;
	CHECK_OPEN;
	CHECK_PARAMS(1);
	LINFO("cec", "close");
	cec->doAsync( new impl::CloseAsync(args[0],cec) );
	FIN;
}

void Cec::getActiveDevices( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC(
		new impl::GetActivesAsync(args[0],cec->_adapter)
	);
}

void Cec::getLogicalAddress( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC( CEC::cec_logical_addresses, GetLogicalAddresses );
}

void Cec::getActiveSource( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC( CEC::cec_logical_address, GetActiveSource );
}

void Cec::setActiveSource( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	INIT;
	CHECK_OPEN;

	if (args.Length() != 1 && args.Length() != 2) {
		THROW_RETURN("Invalid arguments on setActiveSource" );
	}

	int type=CEC::CEC_DEVICE_TYPE_RESERVED;
	if (args.Length() == 2 && !impl::get( args[0], type )) {
		THROW_RETURN("Invalid argument #0 on setActiveSource");
	}

	LDEBUG("cec", "Set active source: type=%d", type );
	cec->doAsync( NEW_BASIC_P1(bool,SetActiveSource,CEC::cec_device_type,(CEC::cec_device_type)type) );

	FIN;
}

void Cec::setLogicalAddress( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( bool, SetLogicalAddress );
}

void Cec::pingAdapter( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC( bool, PingAdapter );
}

void Cec::startBootloader( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC( bool, StartBootloader );
}

void Cec::transmit( const v8::FunctionCallbackInfo<v8::Value>& args ) {	//	TODO
	// CEC::cec_command data;
	// bool result = a ? a->Transmit(data) : false;
	v8::HandleScope scope(v8::Isolate::GetCurrent());
	args.GetReturnValue().Set(_val(false));
}

void Cec::isActiveSource( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( bool, IsActiveSource );
}

void Cec::getDeviceVendorId( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( uint32_t, GetDeviceVendorId );
}

void Cec::getDeviceOSDName( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( std::string, GetDeviceOSDName );
}

void Cec::getDeviceCecVersion( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( CEC::cec_version, GetDeviceCecVersion );
}

void Cec::getDevicePowerStatus( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	DO_ASYNC_BASIC_P1( CEC::cec_power_status, GetDevicePowerStatus );
}

//	Find attached adapters
void Cec::detectAdapters( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	SCOPE;
	LINFO("cec", "detectAdapters");

	//	Get adapter
	CEC::ICECAdapter *adapt = adapter(args,false);

	//	Detect connected adapters
	int maxDevices = 0;
	int8_t detected=10;
	CEC::cec_adapter_descriptor *devList = NULL;
	while (detected > maxDevices) {
		delete[] devList;
		maxDevices = detected;
		devList = new CEC::cec_adapter_descriptor[maxDevices];
		detected = adapt->DetectAdapters( devList, sizeof(CEC::cec_adapter_descriptor) * maxDevices );
		if (detected == -1) {
			delete[] devList;
			args.GetReturnValue().Set(_val(false));
			return;
		}
	}

	v8::Handle<v8::Array> devices = v8::Array::New(v8::Isolate::GetCurrent(), detected);
	for (int8_t i=0; i<detected; i++) {
		v8::Local<v8::Object> device = v8::Object::New(v8::Isolate::GetCurrent());
		setValue( device, "portPath", _val(devList[i].strComPath) );
		setValue( device, "portName", _val(devList[i].strComName) );
		setValue( device, "vendorId", _val(devList[i].iVendorId) );
		setValue( device, "productId", _val(devList[i].iProductId) );
		setValue( device, "firmwareVersion", _val(devList[i].iFirmwareVersion) );
		setValue( device, "physicalAddress", _val(devList[i].iPhysicalAddress) );
		if (devList[i].iFirmwareBuildDate != CEC_FW_BUILD_UNKNOWN) {
			setValue( device, "firmwareBuildDate", _val(devList[i].iFirmwareBuildDate) );
		}
		setValue( device, "adapterType", _val(adapt->ToString(devList[i].adapterType)) );

		devices->Set( i, device );
	}

	delete[] devList;

	args.GetReturnValue().Set(devices);
}

//	Configuration methods
void Cec::getClientVersion( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LDEBUG("cec", "getClientVersion");
	INIT;
	args.GetReturnValue().Set(_val(cec->_cfg->clientVersion));
}

void Cec::getServerVersion( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LDEBUG("cec", "getServerVersion");
	INIT;
	args.GetReturnValue().Set(_val(cec->_cfg->serverVersion));
}

void Cec::getFirmwareVersion( const v8::FunctionCallbackInfo<v8::Value>& args ) {
	LDEBUG("cec", "getFirmwareVersion");
	INIT;
	args.GetReturnValue().Set(_val(cec->_cfg->iFirmwareVersion));
}

//	Node modules
void onExit( void * /*arg*/ ) {
	//	Finalize util
	util::main::fin();
}

void init( v8::Handle<v8::Object> exports ) {
	util::main::init(LIBCECNODE_NAME, LIBCECNODE_VERSION);
	util::log::setLevel("libcecnode", "libcec", "warn");
	LINFO("cec", "Initialize %s(%s)", LIBCECNODE_NAME, LIBCECNODE_VERSION);

	Cec::Init(exports);

	node::AtExit( onExit, NULL );
}

}	//	namespace tvd

//	Export module
NODE_MODULE(tvdmodule, tvd::init);

