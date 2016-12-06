#include "htmlshell.h"
#include "util.h"
#include "apis/testapi/testapi.h"
#include "../src/errors.h"
#include "../src/switches.h"
#include "../src/util.h"
#include "../src/apis/system/systemapi.h"
#include "../src/impl/app/maindelegate.h"
#include "dtv-htmlshellsdk/src/apis/system.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "content/public/app/content_main.h"
#include "content/public/app/content_main_runner.h"

namespace test {

int argc = 0;
char **argv = nullptr;

void setCommandLineArguments( int ac, char **av ) {
	test::argc = ac;
	test::argv = av;
}

int HtmlShellBase::loadApis() {
	if (!addApi(new TestApi(this))) {
		return HTMLSHELL_FAIL_TO_LOAD_API;
	}
	return tvd::MainDelegate::loadApis();
}

void HtmlShell::onMainLoopStarted() {
	stop(HTMLSHELL_NO_ERROR);
};

class HtmlShellInitFail : public HtmlShell {
protected:
	virtual bool init() { return false; };
};

}

class HtmlShellInitFailTest : public HtmlShellTest {
protected:
	virtual tvd::MainDelegate *createShell() { return new test::HtmlShellInitFail(); }
};

class HtmlShellRunTest : public HtmlShellTest {
protected:
	virtual tvd::MainDelegate *createShell() { return new test::HtmlShellBase(); }
};

int HtmlShellTestLauncherDelegate::RunTestSuite(int argc, char** argv) {
	return HtmlShellTestSuite(argc, argv).Run();
}

bool HtmlShellTestLauncherDelegate::AdjustChildProcessCommandLine( base::CommandLine *command_line, const base::FilePath &/*temp_data_dir*/ ) {
	command_line->AppendSwitch(switches::kUseFakeDeviceForMediaStream);
	command_line->AppendSwitch(switches::kUseFakeUIForMediaStream);
	return true;
}

content::ContentMainDelegate *HtmlShellTestLauncherDelegate::CreateContentMainDelegate() {
	return new tvd::MainDelegate();
}

HtmlShellTest::HtmlShellTest()
{
	_initialized = false;
	_shell = nullptr;
	_mainRunner = nullptr;
}

HtmlShellTest::~HtmlShellTest()
{
}

void HtmlShellTest::SetUp() {
	_shell = createShell();
	_mainRunner = content::ContentMainRunner::Create();
}

void HtmlShellTest::TearDown() {
	if (_initialized) {
		_mainRunner->Shutdown();
	}

	delete _shell;
	_shell = nullptr;

	delete _mainRunner;
	_mainRunner = nullptr;
}

tvd::MainDelegate *HtmlShellTest::createShell() {
	return new test::HtmlShell();
}

void noop() {}

int HtmlShellTest::init( base::Closure *initTask ) {
	content::ContentMainParams params(_shell);
	params.ui_task = initTask ? initTask : new base::Closure(base::Bind(&noop));
	params.argc = test::argc;
	params.argv = const_cast<const char**>(test::argv);
	int result = _mainRunner->Initialize(params);
	if (result >= 0) {
		return result;
	}
	_initialized = true;
	return HTMLSHELL_NO_ERROR;
}

TEST_F( HtmlShellTest, shell_constructed_correctly ) {
	ASSERT_TRUE(_shell != nullptr);
}

TEST_F( HtmlShellTest, initialization_fail_if_no_manifest_or_url_provided ) {
	ASSERT_EQ( HTMLSHELL_NO_MANIFEST_OR_URL, init() );
}

TEST_F( HtmlShellTest, initialization_succeed_if_url_provided ) {
	base::CommandLine::ForCurrentProcess()->AppendArg("http://google.com.ar");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
}

TEST_F( HtmlShellTest, initialization_succedd_if_valid_manifest_provided ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("manifest.json").value() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
}

TEST_F( HtmlShellTest, initialization_succedd_if_manifest_and_url_provided ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("manifest.json").value() );
	base::CommandLine::ForCurrentProcess()->AppendArg("http://google.com.ar");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
}

TEST_F( HtmlShellTest, initialization_fail_if_manifest_dont_exists ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("inexistent.json").value() );
	ASSERT_EQ( HTMLSHELL_INVALID_MANIFEST, init() );
}

TEST_F( HtmlShellTest, initialization_fail_if_manifest_has_errors ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("badmanifest.json").value() );
	ASSERT_EQ( HTMLSHELL_INVALID_MANIFEST, init() );
}

TEST_F( HtmlShellTest, shell_run_correctly_if_initialized_successfully ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("manifest.json").value() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, _mainRunner->Run() );
}

TEST_F( HtmlShellTest, system_profile_without_libs_doesnt_load_any_lib ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("withoutlibs.json").value() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( 0u, _shell->librariesCount() );
}

TEST_F( HtmlShellTest, shell_fail_to_initialize_if_library_dont_exists ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("inexistentlib.json").value() );
	ASSERT_EQ( HTMLSHELL_INVALID_MANIFEST, init() );
	ASSERT_EQ( 0u, _shell->librariesCount() );
}

TEST_F( HtmlShellRunTest, shell_fail_to_run_with_invalid_library ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("invalidlib.json").value() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_LIBRARY_LOAD_ERROR, _mainRunner->Run() );
	ASSERT_EQ( 0u, _shell->librariesCount() );
}

TEST_F( HtmlShellRunTest, shell_fail_to_run_when_fail_to_init_library ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("initerrorlib.json").value() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_LIBRARY_INIT_ERROR, _mainRunner->Run() );
	ASSERT_EQ( 0u, _shell->librariesCount() );
}

void runTestsOnUIT( tvd::MainDelegate *shell ) {
	if (!TVD_CURRENTLY_ON_UIT()) {
		tvd::util::postTask(TVD_UIT, base::Bind(&runTestsOnUIT, shell));
	} else {
		LOG(INFO) << "SEBAS runTests()";
		ASSERT_EQ( 4u, shell->librariesCount() );
		TestApi *testApi = static_cast<TestApi*>(shell->getShellApi(TEST_API));
		ASSERT_TRUE( testApi != nullptr );
		testApi->runTests();
		shell->stop(HTMLSHELL_NO_ERROR);
	}
}

void runTests( tvd::MainDelegate *shell ) {
	// Run test after libraries are successfully loaded
	tvd::util::postTask(TVD_FILET, base::Bind(&runTestsOnUIT, shell));
}

TEST_F( HtmlShellRunTest, shell_library_interact_succesfully_with_apis ) {
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("manifest.json").value() );
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kSysProfile, util::getTestProfileDir("system").value() );
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kUsrProfile, util::getTestProfileDir("user").value() );
	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII("test-switch", "Test");

	ASSERT_EQ( HTMLSHELL_NO_ERROR, init(new base::Closure(base::Bind(&runTests, _shell))) );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, _mainRunner->Run() );
}
