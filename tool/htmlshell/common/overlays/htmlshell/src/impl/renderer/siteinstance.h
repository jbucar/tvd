#pragma once

#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/render_view_observer.h"
#include <map>
#include <vector>

namespace base {
class ListValue;
}

namespace tvd {

/**
 * This is the representation of a web-site instance on the renderer process
 */
class SiteInstance : public content::RenderViewObserver,
                     public content::RenderFrameObserver {
public:
	SiteInstance( content::RenderView *renderView, int id, const std::string &js );
	~SiteInstance() override;

	int sendExtQuery( v8::Isolate *isolate, const std::string &msg, const base::ListValue &params, v8::Handle<v8::Function> callback );

	// content::RenderViewObserver implementation:
	bool OnMessageReceived( const IPC::Message &message ) override;

	// content::RenderFrameObserver implementation:
	void DidCreateScriptContext( v8::Local<v8::Context> context, int extension_group, int world_id ) override;

protected:
	struct JsCallback {
		v8::Isolate *isolate;
		v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context> > context;
		v8::Persistent<v8::Function,v8::CopyablePersistentTraits<v8::Function> > cb;
	};
	typedef std::map<int, JsCallback> JsCallbackMap;

	void onExtQueryResponse( int siteHostId, int queryId, const std::string &error, const base::ListValue &params, bool isSignal );
	void executeCallback( JsCallbackMap::iterator &it,
                          v8::Local<v8::Context> context,
                          v8::Isolate *isolate,
                          std::vector<v8::Handle<v8::Value> > &args,
                          bool remove );

private:
	int _id;
	unsigned _nextQueryId;
	std::string _js;
	JsCallbackMap _callbacks;
};

}
