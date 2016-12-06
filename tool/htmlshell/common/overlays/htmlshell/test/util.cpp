#include "util.h"
#include "base/path_service.h"

namespace util {

base::FilePath getTestResourceDir( const std::string &name ) {
	base::FilePath dir;
	PathService::Get(base::DIR_EXE, &dir);
	return getTestDir(name);
}

base::FilePath getTestDir( const std::string &name ) {
	base::FilePath dir;
	PathService::Get(base::DIR_EXE, &dir);
	return dir.Append("test").Append(name);
}

base::FilePath getTestProfileDir( const std::string &name ) {
	return getTestDir("profiles").Append(name);
}

base::FilePath getDefaultProfileDir( const std::string &name ) {
	base::FilePath dir;
	PathService::Get(base::DIR_EXE, &dir);
	return dir.Append("profiles").Append(name);
}

}
