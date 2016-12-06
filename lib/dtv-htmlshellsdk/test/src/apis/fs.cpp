#include "apis.h"
#include "../../../src/libraryinterface.h"
#include "../../../src/apis/fs.h"
#include "../../../src/test/mocks.h"

class FsTest : public SdkApiTest {
};

TEST_F( FsTest, fail_to_get_api_if_not_exist ) {
	ASSERT_TRUE( _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 1, 0) == NULL);
}

TEST_F( FsTest, get_api_1_0_successfully ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 1, 0) != NULL );
}

TEST_F( FsTest, fail_to_get_api_with_wrong_version ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 1, 1) == NULL);
	ASSERT_TRUE( _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 2, 0) == NULL);
}

TEST_F( FsTest, default_profiles_dirs ) {
	tvd::test::createMockApis();
	tvd::fs::ApiInterface_1_0 *api = _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_STREQ( "/tmp/htmlshellsdk/usr_profile", api->getUserProfileDir().c_str() );
	ASSERT_STREQ( "/tmp/htmlshellsdk/sys_profile", api->getSystemProfileDir().c_str() );
}

TEST_F( FsTest, custom_profiles_dirs ) {
	tvd::test::createMockApis();
	tvd::system::test::setCmdSwitch("usr-profile", "/some/dir1");
	tvd::system::test::setCmdSwitch("sys-profile", "/some/dir2");

	tvd::fs::ApiInterface_1_0 *api = _shell->getApi<tvd::fs::ApiInterface_1_0>(FS_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_STREQ( "/some/dir1", api->getUserProfileDir().c_str() );
	ASSERT_STREQ( "/some/dir2", api->getSystemProfileDir().c_str() );
}
