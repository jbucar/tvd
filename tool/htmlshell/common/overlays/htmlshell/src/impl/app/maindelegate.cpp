#include "maindelegate.h"
#include "crashreporterclient.h"
#include "../browser/browserclient.h"
#include "../browser/udpprotocolhandler.h"
#include "../browser/urlrequestcontextgetter.h"
#include "../renderer/rendererclient.h"
#include "../../apis/shellapi.h"
#include "../../apis/browser/browserapi.h"
#include "../../apis/browser/util.h"
#include "../../apis/fs/fsapi.h"
#include "../../apis/system/systemapi.h"
#include "../../apis/web/webapi.h"
#include "../../errors.h"
#include "../../library.h"
#include "../../media/video_proxy.h"
#include "../../switches.h"
#include "../../util.h"
#include "base/base_switches.h"
#if defined(ARCH_CPU_ARM_FAMILY) && (defined(OS_ANDROID) || defined(OS_LINUX))
#include "base/cpu.h"
#endif
#include "base/command_line.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/json/json_file_value_serializer.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/pepper_plugin_info.h"
#if defined(OS_POSIX) && !defined(OS_MACOSX)
#include "components/crash/content/app/breakpad_linux.h"
#endif
#include "net/base/filename_util.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_job_factory_impl.h"
#include "ppapi/shared_impl/ppapi_permissions.h"
#include "ui/base/resource/resource_bundle.h"

namespace tvd {

base::LazyInstance<CrashReporterClient>::Leaky g_crash_client = LAZY_INSTANCE_INITIALIZER;

MainDelegate::MainDelegate()
	: _client(this)
{
	_mainManifest = nullptr;
}

MainDelegate::~MainDelegate()
{
	for( MainDelegate::CustomScheme scheme : _customSchemes ) {
		delete scheme.second;
	}
	_customSchemes.clear();
}

bool MainDelegate::BasicStartupComplete( int *exit_code ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();

#if defined(OS_WIN)
	// Enable trace control and transport through event tracing for Windows.
	logging::LogEventProvider::Initialize(kContentShellProviderName);
	v8_breakpad_support::SetUp();
#endif
#if defined(OS_MACOSX)
	// Needs to happen before InitializeResourceBundle() and before
	// BlinkTestPlatformInitialize() are called.
	OverrideFrameworkBundlePath();
	OverrideChildProcessPath();
	EnsureCorrectResolutionSettings();
#endif  // OS_MACOSX

	util::log::init();

	LOG(INFO) << "Process basic start-up completed";

	// Add custom scheme handlers
	addCustomScheme(switches::kUdpScheme, new UDPProtocolHandler());

	// Setup defaults profile directories if necesary
	base::FilePath defaultPath;
	PathService::Get(base::DIR_EXE, &defaultPath);
	if (!cmd->HasSwitch(switches::kSysProfile)) {
		cmd->AppendSwitchPath(switches::kSysProfile, defaultPath.Append("profiles").Append("system"));
	}
	if (!cmd->HasSwitch(switches::kUsrProfile)) {
		cmd->AppendSwitchPath(switches::kUsrProfile, defaultPath.Append("profiles").Append("user"));
	}

	int initCode = HTMLSHELL_NO_ERROR;
	if (init()) {
		// Save process type
		_processType = cmd->GetSwitchValueASCII(::switches::kProcessType);
		if (_processType.empty()) {
			_processType = "Browser";

			initCode = parseCommandLine(cmd);
		}
	} else {
		LOG(ERROR) << "Custom initialization fail";
		initCode = HTMLSHELL_INITIALIZATION_ERROR;
	}

	if (initCode != HTMLSHELL_NO_ERROR) {
		*exit_code = initCode;
		return true;
	}

	// Set content client and return false to proced with initialization!
	content::SetContentClient(&_client);
	return false;
}

void MainDelegate::PreSandboxStartup() {
	LOG(INFO) << "Pre sandbox startup";

#if defined(TAC_USE_VIDEO_PROXY)
	if (_processType == switches::kZygoteProcess) {
		video::init();
	}
#endif

#if defined(ARCH_CPU_ARM_FAMILY) && (defined(OS_ANDROID) || defined(OS_LINUX))
	// Create an instance of the CPU class to parse /proc/cpuinfo and cache cpu_brand info.
	base::CPU cpu_info;
#endif

	if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableCrashReporter)) {
		std::string process_type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(switches::kProcessType);
		crash_reporter::SetCrashReporterClient(g_crash_client.Pointer());
#if defined(OS_POSIX) && !defined(OS_MACOSX)
		if (process_type != switches::kZygoteProcess) {
#if defined(OS_ANDROID)
			if (process_type.empty()) {
				breakpad::InitCrashReporter(process_type);
			} else {
				breakpad::InitNonBrowserCrashReporterForAndroid(process_type);
			}
#else
			breakpad::InitCrashReporter(process_type);
#endif
		}
#endif
	}

#if defined(OS_ANDROID)
	// In the Android case, the renderer runs with a different UID and can never access the file system.
	// So we are passed a file descriptor to the ResourceBundle pak at launch time.
	int pak_fd = base::GlobalDescriptors::GetInstance()->MaybeGet(kShellPakDescriptor);
	if (pak_fd >= 0) {
		// This is clearly wrong. See crbug.com/330930
		ui::ResourceBundle::InitSharedInstanceWithPakFile(base::File(pak_fd), false);
		ResourceBundle::GetSharedInstance().AddDataPackFromFile(base::File(pak_fd), ui::SCALE_FACTOR_100P);
		return;
  	}
#endif

	base::FilePath pak_file;
#if defined(OS_MACOSX)
	pak_file = GetResourcesPakFilePath();
#else
	base::FilePath pak_dir;

#if defined(OS_ANDROID)
	bool got_path = PathService::Get(base::DIR_ANDROID_APP_DATA, &pak_dir);
	DCHECK(got_path);
	pak_dir = pak_dir.Append(FILE_PATH_LITERAL("paks"));
#else
	PathService::Get(base::DIR_MODULE, &pak_dir);
#endif

	pak_file = pak_dir.Append(FILE_PATH_LITERAL("htmlshell.pak"));
#endif

	ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);

	if (_processType == "Browser") {
		int res = loadApis();
		if (res != HTMLSHELL_NO_ERROR) {
			stop(res);
		}
	}
}

int MainDelegate::RunProcess( const std::string &type, const content::MainFunctionParams &params ) {
	LOG(INFO) << "Running process of type=" << (type.empty() ? "browser" : type);
	if (type.empty()) {
		_browserMainRunner.reset(content::BrowserMainRunner::Create());

		// Initialize browser process state.
		// Results in a call to BrowserMainParts::PreMainMessageLoopStart() which creates the UI message loop.
		int result = _browserMainRunner->Initialize(params);
		if (result >= 0) {
			LOG(ERROR) << "Fail to initialize content main runner";
			return HTMLSHELL_BROWSER_PROCESS_INIT_ERROR;
		}
#if !defined(OS_ANDROID)
		result = _browserMainRunner->Run();
		LOG(INFO) << "Browser side of HtmlShell finished! result=" << result << ", _exitCode=" << _exitCode;
		_browserMainRunner->Shutdown();
#endif
		return _exitCode;
	}
	return -1;
}

content::ContentBrowserClient *MainDelegate::CreateContentBrowserClient() {
	LOG(INFO) << "Browser client created";
	_browserClient.reset(new BrowserClient(this));
	return _browserClient.get();
}

content::ContentRendererClient *MainDelegate::CreateContentRendererClient() {
	LOG(INFO) << "Render client created";
	_rendererClient.reset(new RendererClient());
	return _rendererClient.get();
}

#if defined(OS_POSIX) && !defined(OS_ANDROID) && !defined(OS_MACOSX)
void MainDelegate::ZygoteForked() {
	if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableCrashReporter)) {
		std::string process_type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(switches::kProcessType);
		breakpad::InitCrashReporter(process_type);
	}
}
#endif

Client *MainDelegate::contentClient() {
	return &_client;
}

BrowserClient *MainDelegate::browserClient() {
	CHECK(_browserClient.get());
	return _browserClient.get();
}

void MainDelegate::addCustomScheme( const std::string &scheme, net::URLRequestJobFactory::ProtocolHandler *handler ) {
	LOG(INFO) << "Add custom scheme handler for scheme=" << scheme;
	_customSchemes.push_back(std::make_pair(scheme, handler));
}

MainDelegate::CustomSchemeList MainDelegate::customSchemes() {
	return _customSchemes;
}

void MainDelegate::setupProtocolHandlers() {
	net::URLRequestJobFactoryImpl *jobFactory = browserClient()->jobFactory();
	if (jobFactory) {
		CustomSchemeList schemesToDelete;
		for(MainDelegate::CustomScheme scheme : _customSchemes) {
			if (!jobFactory->SetProtocolHandler(scheme.first, make_scoped_ptr(scheme.second))) {
				LOG(WARNING) << "Fail to register protocol handler for scheme: " << scheme.first;
				schemesToDelete.push_back(scheme);
			}
		}
		_customSchemes.clear();

		// Delete ProtocolHandlers wich failed to register.
		// Successfuly registered ProtocolHandlers are owned by jobFactory.
		for(MainDelegate::CustomScheme schemeToDelete : schemesToDelete) {
			_customSchemes.remove(schemeToDelete);
			delete schemeToDelete.second;
		}
		schemesToDelete.clear();
	} else {
		LOG(WARNING) << "Fail to setup protocol handlers, could not get URLRequestJobFactory!";
	}
}

void MainDelegate::shutdown() {
	LOG(INFO) << "Shuting-down main delegate";

	unloadApis();

	if (_browserClient.get()) {
		_browserClient->shutdown();
	}

	base::MessageLoop::current()->PostTask(FROM_HERE, base::MessageLoop::QuitWhenIdleClosure());

#if defined(TAC_USE_VIDEO_PROXY)
	if (_processType == switches::kZygoteProcess) {
		video::fin();
	}
#endif

	if (_mainManifest) {
		delete _mainManifest;
		_mainManifest = NULL;
	}
}

void MainDelegate::parseUrlFromCmdLine( base::CommandLine *cmd ) {
	const base::CommandLine::StringVector &args = cmd->GetArgs();
	if (!args.empty()) {
		GURL url(args[0]);
		if (url.is_valid() && url.has_scheme()) {
			_initUrl = url.spec();
		} else {
			std::vector<std::string> parts = base::SplitString(args[0], "?", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
			_initUrl = net::FilePathToFileURL(base::FilePath(parts[0])).spec();
			for (size_t i=1; i < parts.size(); i++) {
				_initUrl += "?";
				_initUrl += parts[i];
			}
		}
		LOG(INFO) << "InitURL: " << _initUrl;
	}
}

base::FilePath MainDelegate::getPathFromCmd( base::CommandLine *cmd, const std::string &switchName ) {
	if (cmd->HasSwitch(switchName)) {
		base::FilePath path(cmd->GetSwitchValueASCII(switchName));
		if (!path.IsAbsolute()) {
			path = base::MakeAbsoluteFilePath(path);
		}
		return path;
	}
	return base::FilePath();
}

base::Value *MainDelegate::loadJSON( const base::FilePath &path ) {
	scoped_ptr<base::Value> jsonObj;
	if (base::PathExists(path)) {
		JSONFileValueDeserializer jsonReader(path);
		int errorCode;
		std::string errorMsg;
		jsonObj = jsonReader.Deserialize( &errorCode, &errorMsg );
		if (!jsonObj.get()) {
			LOG(WARNING) << "Invalid JSON file! path: " << path.value() << ", error: " << errorMsg;
		}
	} else {
		LOG(WARNING) << "JSON file not found! path: " << path.value();
	}
	return jsonObj.release();
}

void MainDelegate::loadPlugins( base::ListValue *plugins, const base::FilePath &rootDir ) {
	for (auto it=plugins->begin(); it != plugins->end(); it++) {
		base::DictionaryValue *plugIn = nullptr;
		if (!(*it)->GetAsDictionary(&plugIn)) {
			LOG(WARNING) << "Invalid plugin entry in manifest";
			continue;
		}
		content::PepperPluginInfo plugin;
		if (!plugIn->GetString("name", &plugin.name)) {
			LOG(WARNING) << "Fail to get plugin name";
			continue;
		}
		std::string pluginDir;
		if (!plugIn->GetString("path", &pluginDir)) {
			LOG(WARNING) << "Plugin path invalid";
			continue;
		}
		plugin.path = base::FilePath(pluginDir);
		if (!plugin.path.IsAbsolute()) {
			plugin.path = rootDir.Append(pluginDir);
		}
		if (!base::PathExists(plugin.path)) {
			LOG(WARNING) << "Plugin entry in manifest with invalid path: " << plugin.path.value();
			continue;
		}
		if (!plugIn->GetBoolean("out_of_process", &plugin.is_out_of_process)) {
			plugin.is_out_of_process = true;
		}
		if (!plugIn->GetBoolean("sandboxed", &plugin.is_sandboxed)) {
			plugin.is_sandboxed = true;
		}
		if (!plugIn->GetString("version", &plugin.version)) {
			plugin.version = "1.0";
		}
		if (!plugIn->GetString("description", &plugin.description)) {
			plugin.description = plugin.name + " " + plugin.version;
		}
		base::ListValue *mimeTypes = nullptr;
		plugin.permissions = ppapi::PERMISSION_DEV | ppapi::PERMISSION_PRIVATE;
		if (plugIn->GetList("mime_types", &mimeTypes)) {
			for (auto mit=mimeTypes->begin(); mit != mimeTypes->end(); mit++) {
				base::DictionaryValue *mime = nullptr;
				if (!(*mit)->GetAsDictionary(&mime)) {
					LOG(WARNING) << "Invalid plugin mime type entry";
					continue;
				}
				std::string type, extensions, description;
				if (!mime->GetString("type", &type)) {
					LOG(WARNING) << "Mime type not specified";
					continue;
				}
				if (!mime->GetString("extensions", &extensions)) {
					LOG(WARNING) << "Mime extensions not specified";
					continue;
				}
				if (!mime->GetString("description", &description)) {
					LOG(WARNING) << "Mime description not specified";
					continue;
				}
				content::WebPluginMimeType pluginMimeType(type, extensions, description);

				base::DictionaryValue *aditional_params = nullptr;
				if (mime->GetDictionary("aditional_params", &aditional_params)) {
					base::DictionaryValue::Iterator adp_it(*aditional_params);
					while (!adp_it.IsAtEnd()) {
						std::string adp_value;
						if (adp_it.value().GetAsString(&adp_value)) {
							pluginMimeType.additional_param_names.push_back(base::ASCIIToUTF16(adp_it.key()));
							pluginMimeType.additional_param_values.push_back(base::ASCIIToUTF16(adp_value));
						}
						adp_it.Advance();
					}

				}
				plugin.mime_types.push_back(pluginMimeType);
			}
		}
		_client.addPlugin(plugin);
	}
}

int MainDelegate::parseCommandLine( base::CommandLine *cmd ) {
	base::FilePath path;
	base::ListValue *plugins = nullptr;
	parseUrlFromCmdLine(cmd);

	path = getPathFromCmd(cmd, switches::kPlugins);
	if (!path.empty()) {
		base::Value *pluginManifest = loadJSON(path);
		if (pluginManifest && pluginManifest->GetAsList(&plugins)) {
			loadPlugins(plugins, path.DirName());
		} else {
			LOG(WARNING) << "Plugin manifest has to be a valid JSON array";
			return HTMLSHELL_INVALID_MANIFEST;
		}
	}

	path = getPathFromCmd(cmd, switches::kManifest);
	if (path.empty() && _initUrl.empty()) {
		LOG(WARNING) << "Neither manifest or url provided";
		return HTMLSHELL_NO_MANIFEST_OR_URL;
	}
	else if (!path.empty()) {
		_manifestRoot = path.DirName();
		base::Value *mainManifest = loadJSON(path);
		if (mainManifest && mainManifest->GetAsDictionary(&_mainManifest) && isMainManifestValid()) {
			if (_mainManifest->HasKey("plugins")) {
				_mainManifest->GetList("plugins", &plugins);
				loadPlugins(plugins, _manifestRoot);
			}
		} else {
			LOG(WARNING) << "Invalid main manifest! path=" << path.value();
			return HTMLSHELL_INVALID_MANIFEST;
		}
	}
	return HTMLSHELL_NO_ERROR;
}

bool MainDelegate::isMainManifestValid() {
	size_t fields = _mainManifest->size();
	if (fields < 4 || fields > 6) {
		LOG(WARNING) << "Bad number of fields in manifest";
		return false;
	}

	std::string value;
	base::ListValue *list = nullptr;

	// Name (required)
	if (!_mainManifest->GetString("name", &value)) {
		LOG(WARNING) << "name field invalid or not present in manifest";
		return false;
	}

	// Version (required)
	if (!_mainManifest->GetString("version", &value)) {
		LOG(WARNING) << "version field invalid or not present in manifest";
		return false;
	}

	// Sdk min version (required)
	if (!(_mainManifest->GetString("sdk_version", &value) && value=="1.0")) {
		LOG(WARNING) << "sdk_version field invalid or not present in manifest";
		return false;
	}

	// Url to launch on start-up (optional)
	if (_mainManifest->HasKey("main") && !_mainManifest->GetString("main", &value)) {
		LOG(WARNING) << "main field invalid";
		return false;
	}

	// Dynamic libraries (optional)
	if (_mainManifest->HasKey("libs")) {
		if (_mainManifest->GetList("libs", &list)) {
			base::ListValue::const_iterator it = list->begin();
			for( ; it != list->end(); it++ ) {
				std::string pathStr;
				if (!(*it)->GetAsString(&pathStr)) {
					LOG(WARNING) << "Fail to get library path from manifest";
					return false;
				}
				if (!base::PathExists(_manifestRoot.Append(pathStr))) {
					LOG(WARNING) << "Library not found! path=" << _manifestRoot.Append(pathStr).value();
					return false;
				}
			}
		} else {
			LOG(WARNING) << "libs field invalid";
			return false;
		}
	}

	// Plugins (optional)
	if (_mainManifest->HasKey("plugins")) {
		if (_mainManifest->GetList("plugins", &list)) {
			for (auto it=list->begin(); it != list->end(); it++) {
				base::DictionaryValue *plugIn = nullptr;
				if (!(*it)->GetAsDictionary(&plugIn)) {
					LOG(WARNING) << "Invalid plugin entry in manifest";
					return false;
				}
				if (!plugIn->GetString("name", &value)) {
					LOG(WARNING) << "Plugin entry in manifest without name specified";
					return false;
				}
				if (!plugIn->GetString("path", &value)) {
					LOG(WARNING) << "Plugin entry in manifest without path specified";
					return false;
				}
				base::FilePath pPath(value);
				if (!pPath.IsAbsolute()) {
					pPath = _manifestRoot.Append(value);
				}
				if (!base::PathExists(pPath)) {
					LOG(WARNING) << "Plugin entry in manifest with invalid path: " << pPath.value();
					return false;
				}
			}
		} else {
			LOG(WARNING) << "plugins field invalid";
			return false;
		}
	}
	return true;
}

int MainDelegate::loadApis() {
	LOG(INFO) << "Loading shell apis";

	bool result = true;
	result &= addApi(new SystemApi(this));
	result &= addApi(new FsApi());
	result &= addApi(new WebApi(browserClient()));
	result &= addApi(new BrowserApi(browserClient()));

	return result ? HTMLSHELL_NO_ERROR : HTMLSHELL_FAIL_TO_LOAD_API;
}

bool MainDelegate::addApi( ShellApi *api ) {
	if (!api->initialize()) {
		LOG(ERROR) << "Fail to initialize api: " << api->name();
		delete api;
		return false;
	}
	LOG(INFO) << "Added shell api: " << api->name();
	_apis.push_back(api);
	return true;
}

void MainDelegate::unloadApis() {
	LOG(INFO) << "Unloading shell apis";

	std::vector<ShellApi*> tmpApis(_apis);
	_apis.clear();
	for (ShellApi *api : tmpApis) {
		api->finalize();
		delete api;
	}
}

void MainDelegate::stop( int code ) {
	LOG(INFO) << "Received request to stop main delegate with code=" << code;
	_exitCode = code;
	util::postTask(TVD_FILET, base::Bind(&MainDelegate::unloadLibraries, base::Unretained(this)));
}

void *MainDelegate::getApiImpl( const std::string &name, uint32_t major, uint32_t minor ) {
	std::vector<ShellApi*>::const_iterator it = _apis.begin();
	for( ; it != _apis.end(); it++ ) {
		if ((*it)->name() == name) {
			return (*it)->getInterface(major, minor);
		}
	}
	LOG(WARNING) << "Fail to get api interface! Api " << name << " doesn't exist";
	return NULL;
}

bool MainDelegate::getStartUpURL( std::string &url ) {
	if (util::isAllowedUrl(_initUrl)) {
		url = _initUrl;
		return true;
	}

	if (_mainManifest) {
		std::string main = "";
		_mainManifest->GetString("main", &main);
		if (!main.empty()) {
			url = "file://" + _manifestRoot.Append(main).value();
		}
		return true;
	}

	return false;
}

ShellApi *MainDelegate::getShellApi( const std::string &name ) const {
	for (ShellApi *api : _apis) {
		if (api->name()  == name) {
			return api;
		}
	}
	LOG(WARNING) << "Shell api " << name << " not found!";
	return NULL;
}

bool MainDelegate::hasApi( const std::string &name, uint32_t major, uint32_t minor ) {
	for (ShellApi *api : _apis) {
		if (api->name() == name) {
			return api->hasInterface(major, minor);
		}
	}
	LOG(WARNING) << "Fail to find api interface! Api " << name << " doesn't exist";
	return false;
}

void MainDelegate::loadLibraries() {
	LOG(INFO) << "Loading libraries on file thread";
	TVD_REQUIRE_FILET();
	bool result = true;
	base::ListValue *list = NULL;
	if (_mainManifest && _mainManifest->GetList("libs", &list)) {
		base::ListValue::const_iterator it = list->begin();
		for( ; it != list->end(); it++ ) {
			std::string pathStr;
			(*it)->GetAsString(&pathStr);
			base::FilePath libFullPath(_manifestRoot.Append(pathStr));
			LOG(INFO) << "Found lib: " << libFullPath.value();

			Library *lib = Library::createFromPath(libFullPath);
			if (!lib) {
				LOG(ERROR) << "Fail to create library from path: " << libFullPath.value();
				util::postTask(TVD_UIT, base::Bind(&MainDelegate::stop, base::Unretained(this), HTMLSHELL_LIBRARY_LOAD_ERROR));
				result = false;
				break;
			}

			if (!lib->init(this)) {
				LOG(ERROR) << "Fail to initialize library! path: " << libFullPath.value();
				delete lib;
				util::postTask(TVD_UIT, base::Bind(&MainDelegate::stop, base::Unretained(this), HTMLSHELL_LIBRARY_INIT_ERROR));
				result = false;
				break;
			}

			LOG(INFO) << "Library " << lib->name() << " loaded successfully";
			_libraries.push_back(lib);
		}
	}

	if (result) {
		util::postTask(TVD_UIT, base::Bind(&MainDelegate::onMainLoopStarted, base::Unretained(this)));
	}
}

void MainDelegate::unloadLibraries() {
	TVD_REQUIRE_FILET();
	LOG(INFO) << "Unloading libraries";
	for (Library *lib : _libraries) {
		lib->fin();
		delete lib;
	}
	_libraries.clear();
	util::postTask(TVD_UIT, base::Bind(&MainDelegate::shutdown, base::Unretained(this)));
}

size_t MainDelegate::librariesCount() const {
	return _libraries.size();
}

void MainDelegate::onMainLoopStarted() {
	browser::ApiInterface_1_0 *browserApi = getApi<browser::ApiInterface_1_0>(BROWSER_API, 1, 0);
	if (!browserApi) {
		LOG(WARNING) << "Browser api not present";
		stop(HTMLSHELL_API_NOT_FOUND);
		return;
	}

	browser::Options opts;
	opts.transparentBg = false;
	opts.needFocus = true;
	opts.visible = true;
	opts.bounds = {0, 0, 0, 0};
	opts.enableMouse = true;

	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	if (cmd->HasSwitch(switches::kInitJs)) {
		opts.initJS = cmd->GetSwitchValueASCII(switches::kInitJs);
	}

	std::string url;
	if (!getStartUpURL(url)) {
		stop(HTMLSHELL_NO_MANIFEST_OR_URL);
		return;
	}

	if (!url.empty()) {
		LOG(INFO) << "Launching url on start-up: " << url;
		browserApi->launchBrowser(0, url, opts);
	}
}

}
