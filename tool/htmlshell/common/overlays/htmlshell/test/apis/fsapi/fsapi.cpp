#include "fsapi.h"
#include "dtv-htmlshellsdk/src/apis/fs.h"
#include "../../util.h"
#include "../../../src/switches.h"
#include "../../../src/apis/fs/fsapi.h"
#include "base/command_line.h"
#include "base/logging.h"

namespace test {
extern int argc;
extern char **argv;
}

FsApiTest::FsApiTest()
{
	fsApi = NULL;
}

FsApiTest::~FsApiTest()
{
	CHECK(fsApi == NULL);
}

void FsApiTest::SetUp() {
	fsApi = new tvd::FsApi();
	ASSERT_TRUE( fsApi != NULL );
}

void FsApiTest::TearDown() {
	fsApi->finalize();
	delete fsApi;
	fsApi = NULL;
}

TEST_F( FsApiTest, fs_api_constructed_successfully) {
	ASSERT_STREQ( "fsapi", fsApi->name().c_str() );
}

TEST_F( FsApiTest, fs_api_init_successfully ) {
	ASSERT_TRUE( fsApi->initialize() );
}

TEST_F( FsApiTest, fsapi_get_interface_version_1_0 ) {
	ASSERT_TRUE( fsApi->initialize() );
	ASSERT_TRUE( fsApi->hasInterface(1,0) );
	ASSERT_TRUE( fsApi->getInterface(1,0) != NULL );
}

TEST_F( FsApiTest, fsapi_get_default_interface ) {
	ASSERT_TRUE( fsApi->initialize() );
	ASSERT_TRUE( fsApi->hasInterface(0,0) );
	ASSERT_TRUE( fsApi->getInterface(0,0) );
}

TEST_F( FsApiTest, fsapi_default_interface_is_last_interface ) {
	ASSERT_TRUE( fsApi->initialize() );
	ASSERT_EQ( fsApi->getInterface(0,0), fsApi->getInterface(FS_API_VERSION_MAJOR,FS_API_VERSION_MINOR) );
}

TEST_F( FsApiTest, get_user_profile_dir_default_if_not_setted ) {
	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getDefaultProfileDir("user").value(), iface->getUserProfileDir() );
}

TEST_F( FsApiTest, get_system_profile_dir_default_if_not_setted ) {
	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getDefaultProfileDir("system").value(), iface->getSystemProfileDir() );
}

TEST_F( FsApiTest, get_user_profile_dir_default_if_invalid ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	cmd->AppendSwitchASCII(switches::kUsrProfile, util::getTestProfileDir("invalid").value() );

	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getDefaultProfileDir("user").value(), iface->getUserProfileDir() );
}

TEST_F( FsApiTest, get_system_profile_dir_default_if_invalid ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	cmd->AppendSwitchASCII(switches::kSysProfile, util::getTestProfileDir("invalid").value() );

	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getDefaultProfileDir("system").value(), iface->getSystemProfileDir() );
}

TEST_F( FsApiTest, get_user_profile_dir_use_valid ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	cmd->AppendSwitchASCII(switches::kUsrProfile, util::getTestProfileDir("user").value() );

	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getTestProfileDir("user").value(), iface->getUserProfileDir() );
}

TEST_F( FsApiTest, get_system_profile_dir_use_valid ) {
	base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	cmd->AppendSwitchASCII(switches::kSysProfile, util::getTestProfileDir("system").value() );

	ASSERT_TRUE( fsApi->initialize() );

	tvd::fs::ApiInterface_1_0 *iface = static_cast<tvd::fs::ApiInterface_1_0*>(fsApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );

	ASSERT_EQ( util::getTestProfileDir("system").value(), iface->getSystemProfileDir() );
}
