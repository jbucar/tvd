#include "siteinstance.h"
#include "rendererclient.h"
#include "../common/messages.h"
#include "../../errors.h"
#include "../../util.h"

#include "base/compiler_specific.h"
MSVC_PUSH_WARNING_LEVEL(0);
#include "bindings/core/v8/V8RecursionScope.h"
MSVC_POP_WARNING();

#include "base/logging.h"
#include "content/public/child/v8_value_converter.h"
#include "content/public/renderer/render_view.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"

namespace tvd {

namespace impl {

static std::string parseV8string( v8::Local<v8::String> v8str ) {
	int len = v8str->Utf8Length();
	char *buf = new char[len + 1];
	v8str->WriteUtf8(buf, len + 1);
	std::string str(buf);
	delete buf;
	return str;
}

static base::Value *toBaseValue( v8::Handle<v8::Array> v8Var, v8::Local<v8::Context> v8Context ) {
	content::V8ValueConverter *converter = content::V8ValueConverter::create();
	converter->SetFunctionAllowed(false);
	base::Value *result = converter->FromV8Value(v8Var, v8Context);
	delete converter;

	return result;
}

// This are the expected params from javascript HtmlShellExtQuery call:
// info[0] (required): String --> "identifier"
// info[1] (required): Anyting -> "parameters"
// info[2] (optional): Funtion -> "callback"
static int checkQueryInfo( const v8::FunctionCallbackInfo<v8::Value> &info ) {
	int length = info.Length();
	bool check = (length >= 2) && (length <= 3);
	check &= (info[0]->IsString()) ? (info[0]->ToString()->Utf8Length() > 0) : false;
	check &= (length == 3) ? info[2]->IsFunction() || info[2]->IsUndefined(): true;
	return check ? HTMLSHELL_NO_ERROR : HTMLSHELL_JAVASCRIPT_SYNTAX_ERROR;
}

static void onExtQuery( const v8::FunctionCallbackInfo<v8::Value> &info ) {
	int error = checkQueryInfo(info);
	if (error == HTMLSHELL_NO_ERROR) {
		v8::Isolate *isolate = info.GetIsolate();
		v8::HandleScope handleScope(isolate);
		v8::Local<v8::Context> context = isolate->GetCallingContext();
		v8::Context::Scope contextScope(context);

		std::string msg = parseV8string(info[0]->ToString());
		if (msg.length() > 0) {
			base::ListValue paramList;
			paramList.Append(toBaseValue(v8::Handle<v8::Array>::Cast(info[1]), context));

			v8::Handle<v8::Function> callback;
			if (info.Length() == 3 && !info[2]->IsUndefined()) {
				callback = v8::Handle<v8::Function>::Cast(info[2]);
			}
			SiteInstance *site = RendererClient::getSiteInstanceForMainFrame(blink::WebLocalFrame::frameForCurrentContext());
			error = site->sendExtQuery(isolate, msg, paramList, callback);
		} else {
			LOG(WARNING) << "Bad HtmlShellCall message format!";
			error = HTMLSHELL_EXTQUERY_FORMAT_ERROR;
		}
	}
	else {
		LOG(WARNING) << "Malformed HtmlShellCall!";
	}

	info.GetReturnValue().Set(error);
}

}

SiteInstance::SiteInstance( content::RenderView *renderView, int id, const std::string &js )
	: content::RenderViewObserver(renderView), content::RenderFrameObserver(renderView->GetMainRenderFrame())
{
	_id = id;
	_js = js;
}

SiteInstance::~SiteInstance()
{
	_nextQueryId = 0;
}

bool SiteInstance::OnMessageReceived( const IPC::Message &message ) {
	bool handled = true;
	IPC_BEGIN_MESSAGE_MAP(SiteInstance, message)
		IPC_MESSAGE_HANDLER(Tvd_ExtQueryResponse, onExtQueryResponse)
		IPC_MESSAGE_UNHANDLED(handled = false)
	IPC_END_MESSAGE_MAP()
	return handled;
}

void SiteInstance::onExtQueryResponse( int siteHostId, int queryId, const std::string &error, const base::ListValue &params, bool isSignal ) {
	TVD_REQUIRE_RT();

	CHECK(siteHostId == _id);
	LOG(INFO) << "Extension query response received for queryId=" << queryId;

	JsCallbackMap::iterator it = _callbacks.find(queryId);
	if (it == _callbacks.end()) {
		LOG(INFO) << "Extension query callback not set for queryId=" << queryId;
	} else {
		v8::Isolate *isolate = it->second.isolate;
		blink::V8RecursionScope isolateScope(isolate);
		v8::HandleScope handleScope(isolate);
		v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, it->second.context);
		v8::Context::Scope contextScope(context);

		std::vector<v8::Handle<v8::Value> > args;
		if (!error.empty()) {
			args.push_back(v8::Exception::Error(v8::String::NewFromUtf8(isolate, error.c_str())));
		} else if (!isSignal) {
			args.push_back(v8::Undefined(isolate));
		}
		size_t argc = params.GetSize();
		if (argc>0) {
			content::V8ValueConverter *converter = content::V8ValueConverter::create();
			for (size_t i=0; i<argc; i++) {
				const base::Value *val;
				if (params.Get(i, &val)) {
					args.push_back(converter->ToV8Value(val, context));
				}
			}
			delete converter;
		}

		executeCallback(it, context, isolate, args, !isSignal);
	}
}

void SiteInstance::DidCreateScriptContext( v8::Local<v8::Context> context, int extension_group, int world_id ) {
	LOG(INFO) << "WebKit script context created";

	v8::Isolate *isolate = blink::mainThreadIsolate();
	blink::V8RecursionScope isolateScope(isolate);
	v8::HandleScope handle_scope(isolate);
	v8::Context::Scope scope(context);

	v8::Persistent<v8::Context> handler(isolate, context);
	v8::Handle<v8::Object> global = context->Global()->ToObject();

	// Create a new V8 function template.
	global->DefineOwnProperty(
		context,
		v8::String::NewFromUtf8(isolate, "HtmlShellCall"),
		v8::FunctionTemplate::New(isolate, impl::onExtQuery)->GetFunction(),
		static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum | v8::DontDelete) );

	if (!_js.empty()) {
		LOG(INFO) << "Loading init script: " << _js;
		v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate, _js.c_str()));
		if (!script.IsEmpty()) {
			script->Run();
		} else {
			LOG(WARNING) << "Fail to compile init script! src: " << _js;
		}
	}
}

int SiteInstance::sendExtQuery( v8::Isolate *isolate, const std::string &msg, const base::ListValue &params, v8::Handle<v8::Function> callback ) {
	TVD_REQUIRE_RT();

	_nextQueryId++;
	if (!callback.IsEmpty()) {
		JsCallback cb = {
			isolate,
			v8::Persistent<v8::Context,v8::CopyablePersistentTraits<v8::Context> >(isolate, isolate->GetCurrentContext()),
			v8::Persistent<v8::Function,v8::CopyablePersistentTraits<v8::Function> >(isolate, callback)
		};
		_callbacks.insert(std::make_pair(_nextQueryId, cb));
	}

	bool result = content::RenderViewObserver::Send(new Tvd_ExtQuery(content::RenderViewObserver::routing_id(), _nextQueryId, msg, params));
	LOG_IF(INFO, result) << "Extension query sent to browser process";
	LOG_IF(ERROR, !result) << "Fail to send extension query to browser process";

	return result ? HTMLSHELL_NO_ERROR : HTMLSHELL_IPC_ERROR;
}

void SiteInstance::executeCallback( JsCallbackMap::iterator &it,
                                    v8::Local<v8::Context> context,
                                    v8::Isolate *isolate,
                                    std::vector<v8::Handle<v8::Value> > &args,
                                    bool remove ) {
	v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, it->second.cb);
	v8::TryCatch tryCatch(isolate);
	tryCatch.SetVerbose(true);
	v8::Local<v8::Value> response = func->Call(context->Global(), args.size(), &args[0]);
	if (tryCatch.HasCaught()) {
		LOG(WARNING) << "Catch javascript exception from javascript callback function! queryId=" << it->first
		             << "Exception message: '" << impl::parseV8string(tryCatch.Message()->Get())
		             << "' line: " << tryCatch.Message()->GetLineNumber();
	} else if (response->IsNativeError()) {
		LOG(WARNING) << "Got a native error from javascript callback function! queryId=" << it->first;
	}

	if (remove) {
		it->second.context.Reset();
		it->second.cb.Reset();
		_callbacks.erase(it);
	}
}

}
