#pragma once

#define FS_API "fsapi"
#define FS_API_VERSION_MAJOR 1u
#define FS_API_VERSION_MINOR 0u

namespace tvd {
namespace fs {

class ApiInterface_1_0 {
public:
	virtual const std::string &getUserProfileDir()=0;
	virtual const std::string &getSystemProfileDir()=0;
};

}
}
