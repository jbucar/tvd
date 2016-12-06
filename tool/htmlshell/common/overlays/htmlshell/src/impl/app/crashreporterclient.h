#pragma once

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "build/build_config.h"
#include "components/crash/content/app/crash_reporter_client.h"

namespace tvd {

class CrashReporterClient : public crash_reporter::CrashReporterClient {
public:
	CrashReporterClient();
	~CrashReporterClient() override;

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
	// Returns a textual description of the product type and version to include in the crash report.
	void GetProductNameAndVersion( const char **product_name, const char **version ) override;
	base::FilePath GetReporterLogFilename() override;
#endif

	// The location where minidump files should be written. Returns true if |crash_dir| was set.
	bool GetCrashDumpLocation( base::FilePath *crash_dir ) override;

#if defined(OS_ANDROID)
	// Returns the descriptor key of the android minidump global descriptor.
	int GetAndroidMinidumpDescriptor() override;
#endif

	bool EnableBreakpadForProcess(const std::string& process_type) override;

private:
	DISALLOW_COPY_AND_ASSIGN(CrashReporterClient);
};

}
