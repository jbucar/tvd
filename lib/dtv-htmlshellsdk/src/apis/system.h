#pragma once

#define SYSTEM_API "systemapi"
#define SYSTEM_API_VERSION_MAJOR 1u
#define SYSTEM_API_VERSION_MINOR 0u

#include <string>

namespace tvd {
namespace system {

class ApiInterface_1_0 {
public:
	virtual std::string getCmdSwitch( const std::string &name )=0;
	virtual void shutdown( unsigned exitCode )=0;
};

}
}
