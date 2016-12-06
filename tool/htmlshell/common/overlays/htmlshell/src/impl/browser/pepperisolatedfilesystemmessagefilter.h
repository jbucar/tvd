#pragma once

#include "ppapi/c/private/ppb_isolated_file_system_private.h"
#include "ppapi/host/resource_message_filter.h"

namespace ppapi {
namespace host {
class PpapiHost;
}
}

namespace tvd {

class PepperIsolatedFileSystemMessageFilter : public ppapi::host::ResourceMessageFilter {
public:
	PepperIsolatedFileSystemMessageFilter( int render_pid, ppapi::host::PpapiHost *ppapi_host );
	~PepperIsolatedFileSystemMessageFilter() override;

	// ppapi::host::ResourceMessageFilter implementation.
	scoped_refptr<base::TaskRunner> OverrideTaskRunnerForMessage(const IPC::Message& msg) override;
	int32_t OnResourceMessageReceived( const IPC::Message& msg, ppapi::host::HostMessageContext* context) override;

protected:
	int32_t OnOpenFileSystem( ppapi::host::HostMessageContext *context, PP_IsolatedFileSystemType_Private type );
	int32_t OpenPluginPrivateFileSystem( ppapi::host::HostMessageContext *context );

private:
	const int _renderPid;
	ppapi::host::PpapiHost *_ppapiHost;

	DISALLOW_COPY_AND_ASSIGN(PepperIsolatedFileSystemMessageFilter);
};

}
