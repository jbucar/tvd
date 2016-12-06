#pragma once

#include "../src/util.h"
#include "../src/impl/app/maindelegate.h"
#include "base/test/test_suite.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/browser_test_base.h"
#include "content/public/test/content_test_suite_base.h"
#include "content/public/test/test_launcher.h"
#include "media/base/media_switches.h"

namespace content {
class ContentMainRunner;
}

namespace test {
void setCommandLineArguments( int argc, char **argv );

class FakeMainDelegate : public tvd::MainDelegate {
public:
	void stop( int code ) override {};
};

class HtmlShellBase : public tvd::MainDelegate {
protected:
	int loadApis() override;
};

class HtmlShell : public HtmlShellBase {
protected:
	void onMainLoopStarted() override;
};
}

class HtmlShellTestSuite : public content::ContentTestSuiteBase {
public:
	HtmlShellTestSuite(int argc, char** argv) : content::ContentTestSuiteBase(argc,argv) {}
	~HtmlShellTestSuite() override {}
};

class HtmlShellTestLauncherDelegate : public content::TestLauncherDelegate {
public:
	HtmlShellTestLauncherDelegate() {}
	~HtmlShellTestLauncherDelegate() override {}

	int RunTestSuite( int argc, char **argv ) override;
	bool AdjustChildProcessCommandLine( base::CommandLine *command_line, const base::FilePath &temp_data_dir ) override;

protected:
	content::ContentMainDelegate *CreateContentMainDelegate() override;

private:
	DISALLOW_COPY_AND_ASSIGN(HtmlShellTestLauncherDelegate);
};

class HtmlShellTest : public content::BrowserTestBase {
public:
	HtmlShellTest();
	~HtmlShellTest() override;

protected:
	int init( base::Closure *initTask=nullptr );

	void SetUp() override;
	void TearDown() override;

	virtual tvd::MainDelegate *createShell();

	void RunTestOnMainThread() override {}
	void RunTestOnMainThreadLoop() override {}

	bool _initialized;
	content::ContentMainRunner *_mainRunner;
	tvd::MainDelegate *_shell;
};
