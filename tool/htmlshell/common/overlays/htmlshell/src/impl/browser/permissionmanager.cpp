#include "permissionmanager.h"
#include "browserclient.h"
#include "content/public/browser/permission_type.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/permission_type.h"

namespace tvd {

PermissionManager::PermissionManager( BrowserClient *browserClient )
	: _browserClient(browserClient)
{}

PermissionManager::~PermissionManager()
{}

int PermissionManager::RequestPermission(content::PermissionType permission, content::RenderFrameHost* rfh, const GURL& requesting_origin, bool user_gesture, const base::Callback<void(content::PermissionStatus)> &cb) {
	content::WebContents *webContents = content::WebContents::FromRenderFrameHost(rfh);
	cb.Run( (webContents && _browserClient->siteHasPermission(webContents, permission))
		? content::PERMISSION_STATUS_GRANTED
		: content::PERMISSION_STATUS_DENIED );
	return kNoPendingOperation;
}

int PermissionManager::RequestPermissions(const std::vector<content::PermissionType> &permissions, content::RenderFrameHost* rfh, const GURL& requesting_origin, bool user_gesture, const base::Callback<void(const std::vector<content::PermissionStatus>&)>& callback) {
	content::WebContents *webContents = content::WebContents::FromRenderFrameHost(rfh);
	std::vector<content::PermissionStatus> result(permissions.size());
	for (const auto &permission : permissions) {
		result.push_back((webContents && _browserClient->siteHasPermission(webContents, permission)) ? content::PERMISSION_STATUS_GRANTED : content::PERMISSION_STATUS_DENIED);
	}
	callback.Run(result);
	return kNoPendingOperation;
}

void PermissionManager::CancelPermissionRequest( int request_id ) {
}

void PermissionManager::ResetPermission( content::PermissionType permission, const GURL &requesting_origin, const GURL &embedding_origin ) {
}

content::PermissionStatus PermissionManager::GetPermissionStatus( content::PermissionType permission, const GURL &requesting_origin, const GURL &embedding_origin ) {
	return content::PERMISSION_STATUS_GRANTED;
}

void PermissionManager::RegisterPermissionUsage( content::PermissionType permission, const GURL &requesting_origin, const GURL &embedding_origin ) {
}

int PermissionManager::SubscribePermissionStatusChange( content::PermissionType permission, const GURL &requesting_origin, const GURL &embedding_origin, const base::Callback<void(content::PermissionStatus)> &callback ) {
	return -1;
}

void PermissionManager::UnsubscribePermissionStatusChange( int subscription_id ) {
}

}
