#include "util.h"
#include "switches.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"
#include "content/public/common/content_switches.h"

namespace boost {
// Need to define this function to properly load htmlshellsdk
void throw_exception( std::exception const &e ) {
	LOG(ERROR) << "boost::exception thrown: " << e.what();
	exit(-1);
}
}

namespace tvd {
namespace util {

gfx::Size getWindowSize() {
	static gfx::Size defaultSize(1280, 720);
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	if (cmd->HasSwitch(switches::kWindowSize)) {
		int w,h;
		if (sscanf(cmd->GetSwitchValueASCII(switches::kWindowSize).c_str(), "%dx%d", &w, &h) == 2) {
			defaultSize.SetSize(w, h);
		}
	}
	return defaultSize;
}

void postTask( int tid, const base::Closure &task ) {
	if (content::BrowserThread::IsMessageLoopValid(static_cast<content::BrowserThread::ID>(tid))) {
		base::MessageLoop *message_loop =
			content::BrowserThread::UnsafeGetMessageLoopForThread(static_cast<content::BrowserThread::ID>(tid));
		if (message_loop) {
			message_loop->PostTask(FROM_HERE, task);
		}
	}
}

namespace log {
void init() {
	::logging::LoggingSettings settings;
	settings.logging_dest = ::logging::LOG_TO_SYSTEM_DEBUG_LOG;
	::logging::InitLogging(settings);
	::logging::SetLogItems(true, true, false, false); // Process-ID | Thread-ID | Timestamp | TickCount

	// Use a minimum log level if the command line asks for one, otherwise leave it at the default level (INFO).
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	if (cmd->HasSwitch(switches::kLoggingLevel)) {
		std::string log_level = cmd->GetSwitchValueASCII(switches::kLoggingLevel);
		int level = 0;
		if (base::StringToInt(log_level, &level) &&	level >= 0 && level < logging::LOG_NUM_SEVERITIES) {
			logging::SetMinLogLevel(level);
		} else {
			LOG(WARNING) << "Bad log level: " << log_level;
		}
	}
}
}

}
}
