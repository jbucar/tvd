#pragma once

#include "../common/client.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/app/content_main_delegate.h"
#include "net/url_request/url_request_job_factory.h"
#include <list>
#include <vector>

namespace base {
class CommandLine;
class DictionaryValue;
class ListValue;
class Value;
}

namespace content {
class BrowserMainRunner;
}

namespace tvd {

class BrowserClient;
class Library;
class RendererClient;
class ShellApi;

class MainDelegate : public content::ContentMainDelegate,
                     public HtmlShellWrapper {
public:
	MainDelegate();
	~MainDelegate() override;

	virtual void stop( int code );

	Client *contentClient();
	BrowserClient *browserClient();

	typedef std::pair<std::string, net::URLRequestJobFactory::ProtocolHandler*> CustomScheme;
	typedef std::list<CustomScheme> CustomSchemeList;
	CustomSchemeList customSchemes();
	void addCustomScheme( const std::string &scheme, net::URLRequestJobFactory::ProtocolHandler *handler );
	void setupProtocolHandlers();

	bool addApi( ShellApi *api );
	ShellApi *getShellApi( const std::string &name ) const;

	void loadLibraries();
	size_t librariesCount() const;

	// HtmlShellWrapper implementation:
	virtual bool hasApi( const std::string &name, uint32_t major, uint32_t minor ) override;

	// ContentMainDelegate implementation:
	bool BasicStartupComplete( int *exit_code ) override;
	void PreSandboxStartup() override;
	int RunProcess( const std::string& process_type, const content::MainFunctionParams& main_function_params ) override;
#if defined(OS_POSIX) && !defined(OS_ANDROID) && !defined(OS_MACOSX)
	void ZygoteForked() override;
#endif

protected:
	content::ContentBrowserClient *CreateContentBrowserClient() override;
	content::ContentRendererClient *CreateContentRendererClient() override;

	// Overriden in tests
	virtual bool init() const { return true; }
	virtual int loadApis();
	virtual void onMainLoopStarted();

	void shutdown();
	void unloadApis();
	void unloadLibraries();

	int parseCommandLine( base::CommandLine *cmd );
	bool getStartUpURL( std::string &url );
	void parseUrlFromCmdLine( base::CommandLine *cmd );
	base::FilePath getPathFromCmd( base::CommandLine *cmd, const std::string &switchName );
	base::Value *loadJSON( const base::FilePath &path );
	void loadPlugins( base::ListValue *plugins, const base::FilePath &rootDir );
	bool isMainManifestValid();


	// HtmlShellWrapper implementation:
	virtual void *getApiImpl( const std::string &name, uint32_t major, uint32_t minor ) override;

private:
	int _exitCode;
	std::string _processType;
	std::string _initUrl;
	base::FilePath _manifestRoot;
	base::DictionaryValue *_mainManifest;
	std::vector<ShellApi*> _apis;
	std::vector<Library*> _libraries;
	Client _client;
	CustomSchemeList _customSchemes;
	scoped_ptr<content::BrowserMainRunner> _browserMainRunner;
	scoped_ptr<BrowserClient> _browserClient;
	scoped_ptr<RendererClient> _rendererClient;

	DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

}
