#include "pepperisolatedfilesystemmessagefilter.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/child_process_security_policy.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/host/dispatch_host_message.h"
#include "ppapi/host/host_message_context.h"
#include "ppapi/host/ppapi_host.h"
#include "ppapi/proxy/ppapi_messages.h"
#include "ppapi/shared_impl/file_system_util.h"
#include "storage/browser/fileapi/isolated_context.h"

namespace tvd {


PepperIsolatedFileSystemMessageFilter::PepperIsolatedFileSystemMessageFilter( int render_pid, ppapi::host::PpapiHost *ppapi_host )
	: _renderPid(render_pid), _ppapiHost(ppapi_host)
{
}

PepperIsolatedFileSystemMessageFilter::~PepperIsolatedFileSystemMessageFilter()
{
}

scoped_refptr<base::TaskRunner> PepperIsolatedFileSystemMessageFilter::OverrideTaskRunnerForMessage( const IPC::Message &msg ) {
	return content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::UI);
}

int32_t PepperIsolatedFileSystemMessageFilter::OnResourceMessageReceived( const IPC::Message &msg, ppapi::host::HostMessageContext *context ) {
	PPAPI_BEGIN_MESSAGE_MAP(PepperIsolatedFileSystemMessageFilter, msg)
		PPAPI_DISPATCH_HOST_RESOURCE_CALL(PpapiHostMsg_IsolatedFileSystem_BrowserOpen, OnOpenFileSystem)
	PPAPI_END_MESSAGE_MAP()
	return PP_ERROR_FAILED;
}

int32_t PepperIsolatedFileSystemMessageFilter::OnOpenFileSystem( ppapi::host::HostMessageContext *context, PP_IsolatedFileSystemType_Private type ) {
	switch (type) {
		case PP_ISOLATEDFILESYSTEMTYPE_PRIVATE_INVALID: break;
		case PP_ISOLATEDFILESYSTEMTYPE_PRIVATE_CRX:     return PP_ERROR_NOTSUPPORTED;
		case PP_ISOLATEDFILESYSTEMTYPE_PRIVATE_PLUGINPRIVATE: return OpenPluginPrivateFileSystem(context);
	}
	NOTREACHED();
	context->reply_msg = PpapiPluginMsg_IsolatedFileSystem_BrowserOpenReply(std::string());
	return PP_ERROR_FAILED;
}

int32_t PepperIsolatedFileSystemMessageFilter::OpenPluginPrivateFileSystem(ppapi::host::HostMessageContext* context) {
	// Only plugins with private permission can open the filesystem.
	if (!_ppapiHost->permissions().HasPermission(ppapi::PERMISSION_PRIVATE)) {
		return PP_ERROR_NOACCESS;
	}

	// Grant full access of isolated filesystem to renderer process.
	const std::string& fsid = storage::IsolatedContext::GetInstance()->RegisterFileSystemForVirtualPath(
		storage::kFileSystemTypePluginPrivate, ppapi::IsolatedFileSystemTypeToRootName(PP_ISOLATEDFILESYSTEMTYPE_PRIVATE_PLUGINPRIVATE), base::FilePath());
	content::ChildProcessSecurityPolicy::GetInstance()->GrantCreateReadWriteFileSystem(_renderPid, fsid);

	context->reply_msg = PpapiPluginMsg_IsolatedFileSystem_BrowserOpenReply(fsid);
	return PP_OK;
}

}
