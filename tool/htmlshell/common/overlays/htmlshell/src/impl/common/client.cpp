#include "client.h"
#include "../app/maindelegate.h"
#include "../../util.h"
#include "../../switches.h"
#include "base/command_line.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/plugin_service.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/pepper_plugin_info.h"
#include "content/public/common/user_agent.h"
#include "gpu/config/gpu_info.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"

namespace tvd {

std::string Client::GetShellUserAgent() {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	std::string extraUA = cmd->GetSwitchValueASCII(switches::kExtraUA);
	return content::BuildUserAgentFromProduct(std::string("Chrome/") + CHROMIUM_VERSION + " " + extraUA);
}

Client::Client( MainDelegate *delegate )
{
	_mainDelegate = delegate;
	_pluginsRegistered = false;
}

Client::~Client()
{
	_mainDelegate = NULL;
}

void Client::AddAdditionalSchemes( std::vector<url::SchemeWithType> *standard_schemes, std::vector<std::string> *savable_schemes ) {
	LOG(INFO) << "Adding custom schemes";

	content::ChildProcessSecurityPolicy *policy = content::ChildProcessSecurityPolicy::GetInstance();
	for(MainDelegate::CustomScheme scheme : _mainDelegate->customSchemes()) {
		if (!policy->IsWebSafeScheme(scheme.first)) {
			policy->RegisterWebSafeScheme(scheme.first);
		}
		standard_schemes->push_back((url::SchemeWithType){scheme.first.c_str(), url::SCHEME_WITHOUT_AUTHORITY});
	}
}

void Client::addPlugin( const content::PepperPluginInfo &pInfo ) {
	if (!_pluginsRegistered) {
		LOG(INFO) << "Adding plugin: " << pInfo.name;
		_plugins.push_back(pInfo);
	} else {
		LOG(WARNING) << "Cannot add plugins after ppapi registration";
	}
}

bool Client::isPluginAvailable( content::WebPluginInfo *plugin ) const {
	for (const content::PepperPluginInfo &pInfo : _plugins) {
		if (base::ASCIIToUTF16(pInfo.name) == plugin->name) {
			return true;
		}
	}
	return false;
}

std::string Client::getPluginNameFromPath( const base::FilePath &path ) const {
	for (const content::PepperPluginInfo &pInfo : _plugins) {
		if (pInfo.path == path) {
			return pInfo.name;
		}
	}
	return "";
}

void Client::AddPepperPlugins( std::vector<content::PepperPluginInfo> *plugins ) {
	for (content::PepperPluginInfo pInfo : _plugins) {
		plugins->push_back(pInfo);
		content::PluginService::GetInstance()->RegisterInternalPlugin(pInfo.ToWebPluginInfo(), true);
	}
	content::PluginService::GetInstance()->RefreshPlugins();
	content::PluginService::GetInstance()->PurgePluginListCache(NULL, false);
	_pluginsRegistered = true;
}

std::string Client::GetUserAgent() const {
	return GetShellUserAgent();
}

base::string16 Client::GetLocalizedString( int message_id ) const {
	base::string16 value = ResourceBundle::GetSharedInstance().GetLocalizedString(message_id);
	if (value.empty()) {
		LOG(WARNING) << "No localized string available for id=" << message_id;
	}
	return value;
}

base::StringPiece Client::GetDataResource( int resource_id, ui::ScaleFactor scale_factor ) const {
	base::StringPiece value = ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(resource_id, scale_factor);
	if (value.empty()) {
		LOG(INFO) << "No data resource available for id=" << resource_id;
	}
	return value;
}

gfx::Image &Client::GetNativeImageNamed( int resource_id ) const {
	gfx::Image &value = ResourceBundle::GetSharedInstance().GetNativeImageNamed(resource_id);
	if (value.IsEmpty()) {
		LOG(INFO) << "No native image available for id=" << resource_id;
	}
	return value;
}

#if defined(OS_MACOSX) && !defined(OS_IOS)
std::string Client::GetCarbonInterposePath() const {
	return content::ContentClient::GetCarbonInterposePath();
}
#endif

base::RefCountedStaticMemory *Client::GetDataResourceBytes( int resource_id ) const {
	return ResourceBundle::GetSharedInstance().LoadDataResourceBytes(resource_id);
}

}
