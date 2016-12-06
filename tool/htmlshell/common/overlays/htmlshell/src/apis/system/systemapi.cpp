#include "systemapi.h"
#include "../../errors.h"
#include "../../impl/app/maindelegate.h"
#include "dtv-htmlshellsdk/src/apis/system.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "base/command_line.h"
#include "base/logging.h"

namespace tvd {

namespace impl {

class SystemApi_1_0 : public system::ApiInterface_1_0 {
public:
	explicit SystemApi_1_0( MainDelegate *delegate ) { _delegate = delegate; }
	virtual ~SystemApi_1_0() {}

	virtual void shutdown( unsigned exitCode ) override {
		LOG(INFO) << "Stopping HtmlShell with external code=" << exitCode;
		_delegate->stop(HTMLSHELL_EXTERNAL_CODE_BASE + exitCode);
	}

	virtual std::string getCmdSwitch( const std::string &name ) override {
		return base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(name);
	}

private:
	MainDelegate *_delegate;
};

}

SystemApi::SystemApi( MainDelegate *delegate )
	: ShellApi(SYSTEM_API), _delegate(delegate)
{
	_iface = NULL;
}

SystemApi::~SystemApi()
{
}

bool SystemApi::init() {
	_iface = new impl::SystemApi_1_0(_delegate);
	return true;
}
void SystemApi::fin() {
	if (_iface) {
		delete _iface;
		_iface = NULL;
	}
}

bool SystemApi::registerInterfaces() {
	if (!registerInterface(MAKE_API_VERSION(1u,0u), _iface)) {
		return false;
	}
	return true;
}

uint32_t SystemApi::getLastVersion() const {
	return MAKE_API_VERSION(SYSTEM_API_VERSION_MAJOR, SYSTEM_API_VERSION_MINOR);
}

}
