#pragma once

#include "content/public/common/content_client.h"

namespace base {
class FilePath;
}

namespace content {
struct WebPluginInfo;
}

namespace tvd {

class MainDelegate;

class Client : public content::ContentClient {
public:
	explicit Client( MainDelegate *delegate );
	~Client() override;

	static std::string GetShellUserAgent();

	// Plugins need to be added before AddPepperPlugins is called by content layer
	void addPlugin( const content::PepperPluginInfo &pInfo );
	bool isPluginAvailable( content::WebPluginInfo *plugin ) const;
	std::string getPluginNameFromPath( const base::FilePath &path ) const;

	// ContentClient implementation:
	void AddAdditionalSchemes( std::vector<url::SchemeWithType> *standard_schemes, std::vector<std::string> *savable_schemes ) override;
	void AddPepperPlugins( std::vector<content::PepperPluginInfo> *plugins ) override;
	std::string GetUserAgent() const override;
	base::string16 GetLocalizedString( int message_id ) const override;
	base::StringPiece GetDataResource( int resource_id, ui::ScaleFactor scale_factor ) const override;
	base::RefCountedStaticMemory* GetDataResourceBytes( int resource_id ) const override;
	gfx::Image &GetNativeImageNamed( int resource_id ) const override;
#if defined(OS_MACOSX) && !defined(OS_IOS)
	std::string GetCarbonInterposePath() const override;
#endif

private:
	bool _pluginsRegistered;
	MainDelegate *_mainDelegate;
	std::vector<content::PepperPluginInfo> _plugins;
};

}
