#include "crashreporterclient.h"
#include "../../switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/common/content_switches.h"

namespace tvd {

CrashReporterClient::CrashReporterClient()
{}

CrashReporterClient::~CrashReporterClient()
{}

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
void CrashReporterClient::GetProductNameAndVersion( const char **product_name, const char **version ) {
	*product_name = "htmlshell";
	*version = CHROMIUM_VERSION;
}

base::FilePath CrashReporterClient::GetReporterLogFilename() {
	return base::FilePath(FILE_PATH_LITERAL("htmlshell_uploads.log"));
}
#endif

bool CrashReporterClient::GetCrashDumpLocation( base::FilePath *crash_dir ) {
	if (!base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kSysProfile)) {
		return false;
	}
	*crash_dir = base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(switches::kSysProfile).Append("crashes");
	if (!base::DirectoryExists(*crash_dir)) {
		LOG(INFO) << "Creating crash directory: " << crash_dir->value();
		if (!base::CreateDirectory(*crash_dir)) {
			LOG(WARNING) << "Fail to create crash directory: " << crash_dir->value();
		}
	}
	return true;
}

#if defined(OS_ANDROID)
int CrashReporterClient::GetAndroidMinidumpDescriptor() {
	return kAndroidMinidumpDescriptor;
}
#endif

bool CrashReporterClient::EnableBreakpadForProcess( const std::string &process_type ) {
	return process_type == switches::kRendererProcess ||
	       process_type == switches::kPluginProcess ||
	       process_type == switches::kPpapiPluginProcess ||
	       process_type == switches::kZygoteProcess ||
	       process_type == switches::kGpuProcess;
}

}
