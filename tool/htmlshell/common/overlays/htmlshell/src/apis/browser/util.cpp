#include "util.h"
#include "url/gurl.h"
#include "base/files/file_util.h"

namespace tvd {
namespace util {

bool isAllowedUrl( const std::string &urlstr ) {
	GURL url(urlstr);
	bool allowed = !url.is_empty() && url.is_valid();
	bool isFile = url.SchemeIsFile() || url.SchemeIsFileSystem();
	allowed &= url.SchemeIsHTTPOrHTTPS() || isFile || url.SchemeIs("chrome");
	if (allowed && isFile) {
		allowed &= !url.ExtractFileName().empty();
	}
	return allowed;
}

}
}
