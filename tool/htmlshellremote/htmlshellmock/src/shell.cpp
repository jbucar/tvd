#include <node/node.h>
#include <node/v8.h>
#include <node/uv.h>
#include <string>
#include <htmlshellsdk/libraryinterface.h>
#include <htmlshellsdk/test/mocks.h>
#include <boost/bind.hpp>

#define RO_ELEMENT static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete)
#define TVD_EXPORT_METHOD(OBJ, METHOD) OBJ->ForceSet(v8::String::NewFromUtf8(isolate, #METHOD, v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, METHOD)->GetFunction(), RO_ELEMENT);
#define TVD_EXPORT_VALUE(OBJ, NAME, VALUE) OBJ->ForceSet(v8::String::NewFromUtf8(isolate, NAME, v8::String::kInternalizedString), VALUE, RO_ELEMENT);

using namespace v8;

#define DO_STR(s) #s
#define INVALID_NARGS(n) \
	if (args.Length() != n) { \
		printf( "[%s] Error: Cantidad invalida de argumentos: args=%d, valid=%d\n", __FUNCTION__, args.Length(), n ); \
		args.GetReturnValue().Set(False(isolate)); \
		return; \
	}

#define INVALID_ARG(name,type) \
	printf( "[%s] error: El argumento " DO_STR(name) " no es un " DO_STR(type) "\n", __FUNCTION__ ); \
	args.GetReturnValue().Set(False(isolate)); \
	return;


namespace impl {

static uv_lib_t lib;
static tvd::HtmlShellWrapper *wrapper = NULL;

bool get( const v8::Handle<v8::Value> &val, std::string &value ) {
	if (!val->IsString()) {
		return false;
	}
	v8::String::Utf8Value utf8Val(val->ToString());
	value = *utf8Val;
	return true;
}

bool get( const v8::Handle<v8::Value> &val, int &value ) {
	if (!val->IsInt32()) {
		return false;
	}
	value = val->Int32Value();
	return true;
}

bool get( const v8::Handle<v8::Value> &val, bool &value ) {
	if (!val->IsBoolean()) {
		return false;
	}
	value = val->BooleanValue();
	return true;
}

static Persistent<Function> shutdownCB;
static Persistent<Function> jsResponseCB;

struct ExitRequestS {
	uv_work_t req;
	int exitCode;
};
typedef struct ExitRequestS ExitRequest;

struct JsResponseRequestS {
	uv_work_t req;
	int browserId;
	int queryId;
	bool isSignal;
	std::string error;
	std::string params;
};
typedef struct JsResponseRequestS JsResponseRequest;

static void doNothing( uv_work_t * /*req*/ ) {
}

static void doExitCallback( uv_work_t *req, int /*status*/ ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	ExitRequest *exitReq = static_cast<ExitRequest*>(req->data);
	Local<Value> argv[1];
	argv[0] = Local<Value>::New(isolate, Integer::New(isolate, exitReq->exitCode));
	Local<Function> localShutdownCB = Local<Function>::New( isolate, shutdownCB);
	localShutdownCB->Call( isolate->GetCurrentContext()->Global(), 1, argv );
	delete exitReq;
}

static void onShutdown( int exitCode ) {
	ExitRequest *req = new ExitRequest();
	req->req.data = req;
	req->exitCode = exitCode;
	uv_queue_work( uv_default_loop(), &req->req, doNothing, doExitCallback );
}

static void doJsResponseCallback( uv_work_t *req, int /*status*/ ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	JsResponseRequest *workReq = static_cast<JsResponseRequest*>(req->data);
	Local<Value> argv[5];
	int argc=0;
	argv[argc++] = Local<Value>::New(isolate, Integer::New(isolate, workReq->browserId));
	argv[argc++] = Local<Value>::New(isolate, Integer::New(isolate, workReq->queryId));
	argv[argc++] = Local<Value>::New(isolate, Boolean::New(isolate, workReq->isSignal));
	argv[argc++] = Local<Value>::New(isolate, String::NewFromUtf8(isolate, workReq->error.c_str()));
	argv[argc++] = Local<Value>::New(isolate, String::NewFromUtf8(isolate, workReq->params.c_str()));
	Local<Function> localJsResponseCB = Local<Function>::New( isolate, jsResponseCB);
	localJsResponseCB->Call( isolate->GetCurrentContext()->Global(), argc, argv );
	delete workReq;
}

static bool onJsResponse( const tvd::web::Response &response ) {
	JsResponseRequest *req = new JsResponseRequest();
	req->req.data = req;
	req->browserId = response.browserId;
	req->queryId = response.queryId;
	req->isSignal = response.isSignal;
	req->error = response.error;
	req->params = response.params;

	uv_queue_work( uv_default_loop(), &req->req, doNothing, doJsResponseCallback );
	return true;
}

}

static void Init(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	INVALID_NARGS(2);

	//	Get shutdown callback
	if (!args[0]->IsFunction()) {
		INVALID_ARG(onShutdown,function);
	}
	impl::shutdownCB.Reset(isolate, Handle<Function>::Cast(args[0]));

	//	Get api execute callback
	if (!args[1]->IsFunction()) {
		INVALID_ARG(jsResponse,function);
	}
	impl::jsResponseCB.Reset(isolate, Handle<Function>::Cast(args[1]));

	impl::wrapper = tvd::test::getHtmlShellWrapperMock();
	tvd::test::createMockApis();
	tvd::system::test::onShutdown( &impl::onShutdown );
	{
		tvd::web::test::ExecuteCallback fnc = boost::bind(&impl::onJsResponse,_1);
		tvd::web::test::onExecuteCallback( fnc );
	}
	args.GetReturnValue().Set(True(isolate));
}

static void Fin( const FunctionCallbackInfo<Value> &args ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	tvd::test::destroyHtmlShellWrapperMock();
	impl::wrapper = NULL;

	args.GetReturnValue().SetUndefined();
}

static void Start( const FunctionCallbackInfo<Value> &args ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(1);

	if (!impl::wrapper) {
		printf( "[%s] Start error: wrapper no inicializado\n", __FUNCTION__ );
		args.GetReturnValue().Set(False(isolate));
		return;
	}

	std::string url;
	if (!impl::get( args[0], url )) {
		INVALID_ARG( url, string );
	}

	if (uv_dlopen( url.c_str(),	&impl::lib )) {
		printf( "[%s] Start error: no se puede cargar la libreria: url=%s\n", __FUNCTION__, url.c_str() );
		args.GetReturnValue().Set(False(isolate));
		return;
	}

	tvd::InitFn init;
	if (uv_dlsym(&impl::lib, "init", (void **)&init)) {
		printf( "[%s] Start error: no se puede obtener el simbolo init en la libreria: url=%s\n", __FUNCTION__, url.c_str() );
		uv_dlclose(&impl::lib);
		args.GetReturnValue().Set(False(isolate));
		return;
	}

	if (!init( impl::wrapper )) {
		printf( "[%s] Start error: error en la funcion init: url=%s\n", __FUNCTION__, url.c_str() );
		uv_dlclose(&impl::lib);
		args.GetReturnValue().Set(False(isolate));
		return;
	}

	args.GetReturnValue().Set(True(isolate));
}

static void Stop(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	tvd::FinFn fin;

	INVALID_NARGS(0);


	if (!uv_dlsym(&impl::lib, "fin", (void **)&fin)) {
		fin();
	}

	uv_dlclose(&impl::lib);

	args.GetReturnValue().SetUndefined();
}


static void SetSwitchCmd(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(2);

	std::string name;
	if (!impl::get( args[0], name )) {
		INVALID_ARG( name, string );
	}

	std::string value;
	if (!impl::get( args[1], value )) {
		INVALID_ARG( value, string );
	}

	tvd::system::test::setCmdSwitch( name, value );

	args.GetReturnValue().Set(True(isolate));
}

static void EmitKeyEvent(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(3);

	int browserID;
	if (!impl::get( args[0], browserID )) {
		INVALID_ARG(browserID,integer);
	}

	int keyCode;
	if (!impl::get( args[1], keyCode )) {
		INVALID_ARG(keyCode,integer);
	}

	bool isUp;
	if (!impl::get( args[2], isUp )) {
		INVALID_ARG(isUp,bool);
	}

	tvd::browser::test::emitKeyboardEvent( browserID, keyCode, isUp );

	args.GetReturnValue().Set(True(isolate));
}

static void EmitErrorEvent(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(4);
	int argc=0;

	int browserID;
	if (!impl::get( args[argc++], browserID )) {
		INVALID_ARG(browserID,integer);
	}

	int line;
	if (!impl::get( args[argc++], line )) {
		INVALID_ARG(line,string);
	}

	std::string source;
	if (!impl::get( args[argc++], source )) {
		INVALID_ARG(source,string);
	}

	std::string msg;
	if (!impl::get( args[argc++], msg )) {
		INVALID_ARG(msg,string);
	}

	tvd::browser::WebLog logData;
	logData.level = 2;
	logData.line = line;
	logData.source = source;
	logData.message = msg;
	tvd::browser::test::emitWebLog( browserID, logData );
	args.GetReturnValue().Set(True(isolate));
}

static void EmitApiCall(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(5);
	int argc=0;

	std::string api;
	if (!impl::get( args[argc++], api )) {
		INVALID_ARG(api,string);
	}

	int browserID;
	if (!impl::get( args[argc++], browserID )) {
		INVALID_ARG(browserID,integer);
	}

	int queryID;
	if (!impl::get( args[argc++], queryID )) {
		INVALID_ARG(queryID,integer);
	}

	std::string name;
	if (!impl::get( args[argc++], name )) {
		INVALID_ARG(name,string);
	}

	std::string params;
	if (!impl::get( args[argc++], params )) {
		INVALID_ARG(params,string);
	}

	tvd::web::Request req;
	req.browserId = browserID;
	req.queryId = queryID;
	req.name = name;
	req.params = params;
	bool res=tvd::web::test::simulateApiCall( api, req );
	args.GetReturnValue().Set(Boolean::New(isolate,res));
}

static void EmitLoadedEvent(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	INVALID_NARGS(1);
	int argc=0;

	int browserID;
	if (!impl::get( args[argc++], browserID )) {
		INVALID_ARG(browserID,integer);
	}

	tvd::browser::test::emitLoaded( browserID );
	args.GetReturnValue().Set(True(isolate));
}

void init(Handle<Object> exports) {
	Isolate* isolate = Isolate::GetCurrent();

	TVD_EXPORT_METHOD(exports, Init);
	TVD_EXPORT_METHOD(exports, Fin);
	TVD_EXPORT_METHOD(exports, Start);
	TVD_EXPORT_METHOD(exports, Stop);
	TVD_EXPORT_METHOD(exports, SetSwitchCmd);
	TVD_EXPORT_METHOD(exports, EmitKeyEvent);
	TVD_EXPORT_METHOD(exports, EmitErrorEvent);
	TVD_EXPORT_METHOD(exports, EmitApiCall);
	TVD_EXPORT_METHOD(exports, EmitLoadedEvent);
}

NODE_MODULE(htmlshellmock, init)

