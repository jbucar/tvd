#include "htmlshell.h"
#include "base/sys_info.h"

int main( int argc, char **argv ) {
	test::setCommandLineArguments(argc, argv);

	// Extracted from $CHROMIUM_SRC_ROOT/content/test/content_test_launcher.cc
	int jobs = std::max(1, base::SysInfo::NumberOfProcessors() / 2);
	HtmlShellTestLauncherDelegate launcherDelegate;
	return content::LaunchTests(&launcherDelegate, jobs, argc, argv);
}
