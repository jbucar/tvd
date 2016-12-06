#pragma once

#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/host/host_factory.h"

namespace content {
class BrowserPpapiHost;
}

namespace ppapi {
namespace host {
class PpapiHost;
}
}

namespace tvd {

class BrowserPepperHostFactory : public ppapi::host::HostFactory {
public:
	explicit BrowserPepperHostFactory( content::BrowserPpapiHost *host );
	~BrowserPepperHostFactory() override;

	scoped_ptr<ppapi::host::ResourceHost> CreateResourceHost( ppapi::host::PpapiHost *host, PP_Resource resource, PP_Instance instance, const IPC::Message &message ) override;

private:
	content::BrowserPpapiHost *_host;

	DISALLOW_COPY_AND_ASSIGN(BrowserPepperHostFactory);
};

}
