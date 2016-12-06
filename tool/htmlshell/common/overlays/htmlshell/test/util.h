#pragma once

#include "util.h"
#include "base/files/file_path.h"

namespace util {

base::FilePath getTestResourceDir( const std::string &name );
base::FilePath getTestDir( const std::string &name );
base::FilePath getTestProfileDir( const std::string &name );
base::FilePath getDefaultProfileDir( const std::string &name );

}
