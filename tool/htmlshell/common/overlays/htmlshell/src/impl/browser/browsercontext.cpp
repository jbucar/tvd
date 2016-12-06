#include "browsercontext.h"
#include "browserclient.h"
#include "urlrequestcontextgetter.h"
#include "downloadmanagerdelegate.h"
#include "permissionmanager.h"
#include "resourcecontext.h"
#include "../../util.h"
#include "../../switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/threading/thread.h"
#if defined(OS_WIN)
#include "base/base_paths_win.h"
#elif defined(OS_LINUX)
#include "base/nix/xdg_util.h"
#elif defined(OS_MACOSX)
#include "base/base_paths_mac.h"
#endif
#include "content/public/browser/background_sync_controller.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"

namespace tvd {

BrowserContext::BrowserContext( BrowserClient *browserClient )
	: _browserClient(browserClient), _resourceCtx(new ResourceContext())
{
	_path = base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(switches::kSysProfile);

	if (_path.empty()) {
#if defined(OS_WIN)
		CHECK(PathService::Get(base::DIR_LOCAL_APP_DATA, &_path));
		_path = _path.Append(std::wstring(L"htmlshell"));
#elif defined(OS_LINUX)
		scoped_ptr<base::Environment> env(base::Environment::Create());
		base::FilePath config_dir(base::nix::GetXDGDirectory(env.get(), base::nix::kXdgConfigHomeEnvVar, base::nix::kDotConfigDir));
		_path = config_dir.Append("htmlshell");
#elif defined(OS_MACOSX)
		CHECK(PathService::Get(base::DIR_APP_DATA, &_path));
		_path = _path.Append("htmlshell");
#elif defined(OS_ANDROID)
		CHECK(PathService::Get(base::DIR_ANDROID_APP_DATA, &_path));
		_path = _path.Append(FILE_PATH_LITERAL("htmlshell"));
#else
		NOTIMPLEMENTED();
#endif
	} else {
		_path = _path.Append("htmlshell_data");
	}

	LOG(INFO) << "Browser context path: " << _path.value();
	if (!base::DirectoryExists(_path)) {
		base::File::Error error;
		if (!base::CreateDirectoryAndGetError(_path, &error)) {
			LOG(WARNING) << "Fail to create data dir! Error: " << base::File::ErrorToString(error);
		}
	}
}

BrowserContext::~BrowserContext()
{
	if (_resourceCtx) {
		content::BrowserThread::DeleteSoon(content::BrowserThread::IO, FROM_HERE, _resourceCtx.release());
	}
}

base::FilePath BrowserContext::GetPath() const {
	return _path;
}

bool BrowserContext::IsOffTheRecord() const {
	return false;
}

net::URLRequestContextGetter *BrowserContext::GetRequestContext() {
	return GetDefaultStoragePartition(this)->GetURLRequestContext();
}

net::URLRequestContextGetter *BrowserContext::GetRequestContextForRenderProcess( int renderer_child_id )  {
	return GetRequestContext();
}

net::URLRequestContextGetter *BrowserContext::GetMediaRequestContext() {
	return GetRequestContext();
}

net::URLRequestContextGetter *BrowserContext::GetMediaRequestContextForRenderProcess( int renderer_child_id ) {
	return GetRequestContext();
}

net::URLRequestContextGetter *BrowserContext::GetMediaRequestContextForStoragePartition( const base::FilePath &partition_path, bool in_memory ) {
	return GetRequestContext();
}

content::ResourceContext *BrowserContext::GetResourceContext() {
	return _resourceCtx.get();
}

void BrowserContext::setUrlRequestContextGetter( URLRequestContextGetter *getter ) {
	_resourceCtx->setUrlRequestContextGetter(getter);
}

content::DownloadManagerDelegate *BrowserContext::GetDownloadManagerDelegate()  {
	LOG(INFO) << "Getting download manager delegate";
	if (!_downloadMgrDelegate.get()) {
		content::DownloadManager *manager = content::BrowserContext::GetDownloadManager(this);
		_downloadMgrDelegate.reset(new DownloadManagerDelegate(manager));
	}
	return _downloadMgrDelegate.get();
}

content::BrowserPluginGuestManager *BrowserContext::GetGuestManager() {
	return nullptr;
}

storage::SpecialStoragePolicy *BrowserContext::GetSpecialStoragePolicy() {
	return nullptr;
}

content::PushMessagingService *BrowserContext::GetPushMessagingService() {
	return nullptr;
}

content::SSLHostStateDelegate *BrowserContext::GetSSLHostStateDelegate() {
	return nullptr;
}

scoped_ptr<content::ZoomLevelDelegate> BrowserContext::CreateZoomLevelDelegate( const base::FilePath &/*partition_path*/ ) {
	return scoped_ptr<content::ZoomLevelDelegate>();
}

content::PermissionManager *BrowserContext::GetPermissionManager() {
	if (!_permissionManager.get()) {
		_permissionManager.reset(new PermissionManager(_browserClient));
	}
	return _permissionManager.get();
}

content::BackgroundSyncController *BrowserContext::GetBackgroundSyncController() {
	if (!_bgSyncCtrl) {
		_bgSyncCtrl.reset(new content::BackgroundSyncController());
	}
	return _bgSyncCtrl.get();
}

}
