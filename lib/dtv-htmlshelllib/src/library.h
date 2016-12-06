#pragma once

#include <htmlshellsdk/libraryinterface.h>
#include <util/log.h>
#include <string>
#include <stdint.h>

// Library entry points
#define MAKE_HTMLSHELL_LIBRARY(CLS,PRJ) \
namespace tvd {\
LIFIA_API LibWrapper *init( HtmlShellWrapper *wrapper ) { \
	return tvd::HtmlShellLibrary::onLibraryInitialized(wrapper, new CLS(), PRJ ## _NAME, PRJ ## _VERSION); \
} \
LIFIA_API void fin() { \
	tvd::HtmlShellLibrary::onLibraryFinalized(); \
} \
}

namespace tvd {

class LibWrapper;
class HtmlShellWrapper;

class HtmlShellLibrary {
public:
	HtmlShellLibrary();
	virtual ~HtmlShellLibrary();

	static LibWrapper *onLibraryInitialized( HtmlShellWrapper *wrapper, HtmlShellLibrary *lib, const std::string &name, const std::string &version );
	static void onLibraryFinalized();

	LibWrapper *initialize( HtmlShellWrapper *shell );
	void finalize();

protected:
	virtual bool init();
	virtual void fin();

	template<typename T>
	bool loadApi( const std::string &name, uint32_t major, uint32_t minor, T **var );

private:
	LibWrapper *_wrapper;
	HtmlShellWrapper *_shell;
};

template<typename T>
bool HtmlShellLibrary::loadApi( const std::string &name, uint32_t major, uint32_t minor, T **var ) {
	LOG_PUBLISH( DEBUG, "htmlshelllib", "HtmlShellLibrary", "Loading HtmlShell api: %s-%d.%d", name.c_str(), major, minor);

	if (!_shell->hasApi(name, major, minor)) {
		LOG_PUBLISH( DEBUG, "htmlshelllib", "HtmlShellLibrary", "HtmlShell does not have api: %s-%u.%u", name.c_str(), major, minor);
		return false;
	}

	*var = _shell->getApi<T>(name, major, minor);
	if (!*var) {
		LOG_PUBLISH( DEBUG, "htmlshelllib", "Fail to get api: %s-%u.%u", name.c_str(), major, minor);
		return false;
	}

	return true;
}

}
