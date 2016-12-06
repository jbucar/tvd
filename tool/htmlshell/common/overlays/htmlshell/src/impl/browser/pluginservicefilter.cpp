#include "pluginservicefilter.h"
#include "browserclient.h"
#include "../app/maindelegate.h"
#include "../common/client.h"

namespace tvd {

PluginServiceFilter::PluginServiceFilter( MainDelegate *delegate )
{
	_delegate = delegate;
}

PluginServiceFilter::~PluginServiceFilter()
{
}

bool PluginServiceFilter::IsPluginAvailable( int render_process_id,
                                             int render_frame_id,
                                             const void *ctx,
                                             const GURL &url,
                                             const GURL &policyUrl,
                                             content::WebPluginInfo *plugin )
{
	return _delegate->contentClient()->isPluginAvailable(plugin);
}

bool PluginServiceFilter::CanLoadPlugin( int render_process_id, const base::FilePath &path ) {
	std::string pName = _delegate->contentClient()->getPluginNameFromPath(path);
	return _delegate->browserClient()->isPluginAllowedForSite(render_process_id, pName);
}

}
