#pragma once

#include "base/callback.h"
#include "content/public/browser/permission_manager.h"

namespace tvd {

class BrowserClient;

class PermissionManager : public content::PermissionManager {
public:
	explicit PermissionManager( BrowserClient *browserClient );
	~PermissionManager() noexcept(true) override;

	// PermissionManager implementation:
	int RequestPermission(content::PermissionType permission, content::RenderFrameHost* render_frame_host, const GURL& requesting_origin, bool user_gesture, const base::Callback<void(content::PermissionStatus)>& callback) override;
	int RequestPermissions(const std::vector<content::PermissionType> &permission, content::RenderFrameHost* render_frame_host, const GURL& requesting_origin, bool user_gesture, const base::Callback<void(const std::vector<content::PermissionStatus>&)>& callback) override;
	void CancelPermissionRequest(int request_id) override;
	void ResetPermission(content::PermissionType permission, const GURL& requesting_origin, const GURL& embedding_origin) override;
	content::PermissionStatus GetPermissionStatus(content::PermissionType permission, const GURL& requesting_origin, const GURL& embedding_origin) override;
	void RegisterPermissionUsage(content::PermissionType permission, const GURL& requesting_origin, const GURL& embedding_origin) override;
	int SubscribePermissionStatusChange(content::PermissionType permission, const GURL& requesting_origin, const GURL& embedding_origin, const base::Callback<void(content::PermissionStatus)>& callback) override;
	void UnsubscribePermissionStatusChange(int subscription_id) override;

private:
	BrowserClient *_browserClient;

	DISALLOW_COPY_AND_ASSIGN(PermissionManager);
};

}
