#include "htmlshell.h"
#include "util.h"
#include "../src/errors.h"
#include "../src/switches.h"
#include "base/command_line.h"
#include "content/public/app/content_main_runner.h"

class ProcessComunicationTest : public HtmlShellTest {
protected:
	void setupCmdLine( const std::string &app ) {
		base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kManifest, util::getTestDir("launcher").Append("manifest.json").value() );
		base::CommandLine::ForCurrentProcess()->AppendArg(util::getTestResourceDir("apps").Append(app).value());
	}

	tvd::MainDelegate *createShell() override { return new test::HtmlShellBase(); }
};

TEST_F( ProcessComunicationTest, comunication_between_render_and_browser_process ) {
	setupCmdLine("comunication.html");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, _mainRunner->Run() );
}

TEST_F( ProcessComunicationTest, signals_dont_remove_callbacks ) {
	setupCmdLine("signals.html");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, _mainRunner->Run() );
}

TEST_F( ProcessComunicationTest, shell_stopped_with_library_runtime_error ) {
	setupCmdLine("apierror.html");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_LIBRARY_RUNTIME_ERROR, _mainRunner->Run() );
}

TEST_F( ProcessComunicationTest, shell_stopped_with_javascript_runtime_error ) {
	setupCmdLine("malformedQuery.html");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_JAVASCRIPT_SYNTAX_ERROR, _mainRunner->Run() );
}

TEST_F( ProcessComunicationTest, shell_api_modification_not_allowed ) {
	setupCmdLine("apimodification.html");
	ASSERT_EQ( HTMLSHELL_NO_ERROR, init() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, _mainRunner->Run() );
}
