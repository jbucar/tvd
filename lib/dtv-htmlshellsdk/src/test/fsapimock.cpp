#include "mocks.h"
#include "../apis/fs.h"
#include "../apis/system.h"
#include "../libraryinterface.h"

namespace tvd {
namespace fs {
namespace test {

namespace impl {

class FsApiMock : public fs::ApiInterface_1_0 {
public:
	virtual ~FsApiMock() {}

	virtual const std::string &getUserProfileDir() {
		if (usrProfile.empty()) {
			usrProfile = tvd::test::getHtmlShellWrapperMock()->getApi<system::ApiInterface_1_0>("systemapi", 1, 0)->getCmdSwitch("usr-profile");
			if (usrProfile.empty()) {
				usrProfile = "/tmp/htmlshellsdk/usr_profile";
			}
		}
		return usrProfile;
	}

	virtual const std::string &getSystemProfileDir() {
		if (sysProfile.empty()) {
			sysProfile = tvd::test::getHtmlShellWrapperMock()->getApi<system::ApiInterface_1_0>("systemapi", 1, 0)->getCmdSwitch("sys-profile");
			if (sysProfile.empty()) {
				sysProfile = "/tmp/htmlshellsdk/sys_profile";
			}
		}
		return sysProfile;
	}

	std::string usrProfile;
	std::string sysProfile;
};

static FsApiMock *mockIface = NULL;

}

void *getMockInterface() {
	if (!impl::mockIface) {
		impl::mockIface = new impl::FsApiMock();
	}
	return impl::mockIface;
}

void destroyMockInterface() {
	if (impl::mockIface) {
		delete impl::mockIface;
		impl::mockIface = NULL;
	}
}

}
}
}
