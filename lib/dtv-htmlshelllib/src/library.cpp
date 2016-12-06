#include "library.h"
#include <util/assert.h>
#include <util/main.h>
#include <util/mcr.h>

namespace tvd {

namespace impl {
static HtmlShellLibrary *instance = NULL;
}

// static
LibWrapper *HtmlShellLibrary::onLibraryInitialized( HtmlShellWrapper *wrapper, HtmlShellLibrary *lib, const std::string &name, const std::string &version ) {
	DTV_ASSERT(lib);
	DTV_ASSERT(!impl::instance);

	util::main::init(name, version);

	LTRACE("HtmlShellLibrary", "On library initialized: %s", name.c_str());

	LibWrapper *libWrapper = lib->initialize(wrapper);
	if (libWrapper) {
		impl::instance = lib;
	} else {
		LERROR("HtmlShellLibrary", "Fail to initialize %s instance", name.c_str());
		util::main::fin();
	}

	return libWrapper;
}

// static
void HtmlShellLibrary::onLibraryFinalized() {
	DTV_ASSERT(impl::instance);
	LTRACE("HtmlShellLibrary", "On library finalized: %s", util::main::name().c_str());

	impl::instance->finalize();
	DEL(impl::instance);

	util::main::fin();
}

HtmlShellLibrary::HtmlShellLibrary()
{
	_shell = NULL;
	_wrapper = NULL;
}

HtmlShellLibrary::~HtmlShellLibrary()
{
	DTV_ASSERT(!_wrapper);
	DTV_ASSERT(!_shell);
}

LibWrapper *HtmlShellLibrary::initialize( HtmlShellWrapper *shell ) {
	LINFO("HtmlShellLibrary", "Initializing library %s", util::main::name().c_str());

	DTV_ASSERT(!_wrapper);
	DTV_ASSERT(!_shell);
	DTV_ASSERT(shell);

	_shell = shell;
	if (init()) {
		_wrapper = new LibWrapper(util::main::name(), util::main::version());
	} else {
		_shell = NULL;
	}
	return _wrapper;
}

void HtmlShellLibrary::finalize() {
	LINFO("HtmlShellLibrary", "Finalizing library %s", util::main::name().c_str());
	fin();
	DEL(_wrapper);
	_shell = NULL;
}

bool HtmlShellLibrary::init() {
	return true;
}

void HtmlShellLibrary::fin() {
}

}
