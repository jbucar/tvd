#pragma once

#include "base/macros.h"
#include "content/public/browser/plugin_service_filter.h"

namespace tvd {

class MainDelegate;

class PluginServiceFilter : public content::PluginServiceFilter {
public:
	explicit PluginServiceFilter( MainDelegate *delegate );
	~PluginServiceFilter() override;

	bool IsPluginAvailable( int render_process_id,
                            int render_frame_id,
                            const void *ctx,
                            const GURL &url,
                            const GURL &policyUrl,
                            content::WebPluginInfo *plugin ) override;
	bool CanLoadPlugin( int render_process_id, const base::FilePath &path) override;

private:
	MainDelegate *_delegate;
	DISALLOW_COPY_AND_ASSIGN(PluginServiceFilter);
};

}
