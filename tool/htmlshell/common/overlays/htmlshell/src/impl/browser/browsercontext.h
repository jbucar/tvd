#pragma once

#include "base/files/file_path.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"

namespace net {
class URLRequestContextGetter;
}

namespace content {
class BackgroundSyncController;
class ZoomLevelDelegate;
}

namespace tvd {

class BrowserClient;
class ResourceContext;
class DownloadManagerDelegate;
class URLRequestContextGetter;

class BrowserContext : public content::BrowserContext {
public:
	explicit BrowserContext( BrowserClient *browserClient );
	~BrowserContext() override;

	void setUrlRequestContextGetter( URLRequestContextGetter *getter );

	base::FilePath GetPath() const override;
	bool IsOffTheRecord() const override;
	net::URLRequestContextGetter *GetRequestContext() override;
	net::URLRequestContextGetter *GetRequestContextForRenderProcess( int renderer_child_id ) override;
	net::URLRequestContextGetter *GetMediaRequestContext() override;
	net::URLRequestContextGetter *GetMediaRequestContextForRenderProcess( int renderer_child_id ) override;
	net::URLRequestContextGetter *GetMediaRequestContextForStoragePartition( const base::FilePath &partition_path, bool in_memory ) override;
	content::ResourceContext *GetResourceContext() override;
	content::DownloadManagerDelegate *GetDownloadManagerDelegate() override;
	content::BrowserPluginGuestManager *GetGuestManager() override;
	storage::SpecialStoragePolicy *GetSpecialStoragePolicy() override;
	content::PushMessagingService *GetPushMessagingService() override;
	content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;
	scoped_ptr<content::ZoomLevelDelegate> CreateZoomLevelDelegate( const base::FilePath &partition_path ) override;
	content::PermissionManager *GetPermissionManager() override;
	content::BackgroundSyncController *GetBackgroundSyncController() override;

private:
	base::FilePath _path;
	BrowserClient *_browserClient;
	scoped_ptr<ResourceContext> _resourceCtx;
	scoped_ptr<DownloadManagerDelegate> _downloadMgrDelegate;
	scoped_ptr<content::BackgroundSyncController> _bgSyncCtrl;
	scoped_ptr<content::PermissionManager> _permissionManager;
};

}
