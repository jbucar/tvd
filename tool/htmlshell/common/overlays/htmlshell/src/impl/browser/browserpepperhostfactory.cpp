#include "browserpepperhostfactory.h"

#include "pepperisolatedfilesystemmessagefilter.h"
#include "content/public/browser/browser_ppapi_host.h"
#include "ppapi/host/message_filter_host.h"
#include "ppapi/host/ppapi_host.h"
#include "ppapi/host/resource_host.h"
#include "ppapi/proxy/ppapi_messages.h"

namespace tvd {

BrowserPepperHostFactory::BrowserPepperHostFactory( content::BrowserPpapiHost *host )
	: _host(host)
{
}

BrowserPepperHostFactory::~BrowserPepperHostFactory()
{
}

scoped_ptr<ppapi::host::ResourceHost> BrowserPepperHostFactory::CreateResourceHost( ppapi::host::PpapiHost *host, PP_Resource resource, PP_Instance instance, const IPC::Message &message ) {
	// Make sure the plugin is giving us a valid instance for this resource.
	if (_host->IsValidInstance(instance)) {
		// Permissions for the following interfaces will be checked at the time of the corresponding instance's methods calls (because permission check can be performed only on the UI thread).
		// Currently these interfaces are available only for whitelisted apps which may not have access to the other private interfaces.
		if (message.type() == PpapiHostMsg_IsolatedFileSystem_Create::ID) {
			int render_pid, render_fid;
			if (_host->GetRenderFrameIDsForInstance(instance, &render_pid, &render_fid)) {
				return scoped_ptr<ppapi::host::ResourceHost>(new ppapi::host::MessageFilterHost(host, instance, resource, new PepperIsolatedFileSystemMessageFilter(render_pid, _host->GetPpapiHost())));
			}
		}
	}

	return scoped_ptr<ppapi::host::ResourceHost>();
}

}
