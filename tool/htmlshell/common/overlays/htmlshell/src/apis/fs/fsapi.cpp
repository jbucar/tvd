#include "fsapi.h"
#include "dtv-htmlshellsdk/src/apis/fs.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "../../switches.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"

namespace tvd {

namespace impl {

class FsApi_1_0 : public fs::ApiInterface_1_0 {
public:
	FsApi_1_0( const std::string &sys, const std::string &usr ) {
		_sysProfileDir = sys;
		_usrProfileDir = usr;
	}

	virtual ~FsApi_1_0() {}

	virtual const std::string &getSystemProfileDir() {
		return _sysProfileDir;
	}

	virtual const std::string &getUserProfileDir() {
		return _usrProfileDir;
	}

private:
	std::string _sysProfileDir;
	std::string _usrProfileDir;
};

}

FsApi::FsApi()
	: ShellApi(FS_API)
{
	_iface = NULL;
}

FsApi::~FsApi()
{
}

bool FsApi::init() {
	// Set up user profile directories
	base::FilePath sysProfileDir = getProfilePath("system", switches::kSysProfile);
	LOG(INFO) << "Using system profile: " << sysProfileDir.value();

	// Set up system profile directories
	base::FilePath usrProfileDir = getProfilePath("user", switches::kUsrProfile);
	LOG(INFO) << "Using user profile: " << usrProfileDir.value();

	_iface = new impl::FsApi_1_0(sysProfileDir.value(), usrProfileDir.value());
	return true;
}

void FsApi::fin() {
	if (_iface) {
		delete _iface;
		_iface = NULL;
	}
}

bool FsApi::registerInterfaces() {
	if (!registerInterface(MAKE_API_VERSION(1u,0u), _iface)) {
		return false;
	}

	return true;
}

uint32_t FsApi::getLastVersion() const {
	return MAKE_API_VERSION(FS_API_VERSION_MAJOR, FS_API_VERSION_MINOR);
}

base::FilePath FsApi::getProfilePath( const std::string &type, const std::string &key ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();

	base::FilePath path = cmd->GetSwitchValuePath(key);
	if (!base::DirectoryExists(path)) {
		LOG(WARNING) << key << "=" << path.value() << " dont exists, fallback to default!";
		PathService::Get(base::DIR_EXE, &path);
		path = path.Append("profiles").Append(type);
	}
	return path;
}

}
