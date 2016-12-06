#include "library.h"
#include "base/logging.h"

namespace tvd {

// Static
Library *Library::createFromPath( const base::FilePath &libPath ) {
	Library *lib = NULL;

	base::NativeLibraryLoadError error;
	base::NativeLibrary libHnd = base::LoadNativeLibrary( libPath, &error);
	if (libHnd) {
		InitFn init = (InitFn) base::GetFunctionPointerFromNativeLibrary( libHnd, "init" );
		FinFn fin = (FinFn) base::GetFunctionPointerFromNativeLibrary( libHnd, "fin" );

		if (init && fin) {
			lib = new Library(libHnd, init, fin);
		} else {
			LOG(ERROR) << "Fail to create library from path: " << libPath.value();
			if (!init) {
				LOG(ERROR) << "Fail to create library from path: init function not found!";
			}
			if (!fin) {
				LOG(ERROR) << "Fail to create library from path: fin function not found!";
			}
			base::UnloadNativeLibrary(libHnd);
		}
	} else {
		LOG(ERROR) << "Fail to create library from path: " << libPath.value() << ", error: " << error.ToString();
	}

	return lib;
}

Library::Library( base::NativeLibrary handle, InitFn init, FinFn fin )
{
	_initialized = false;
	_handle = handle;
	_init = init;
	_fin = fin;
	_libWrapper = NULL;
}

Library::~Library()
{
	CHECK( !_initialized );
}

bool Library::init( HtmlShellWrapper *wrapper ) {
	if (_initialized) {
		LOG(ERROR) << "Library already initialized!";
		return false;
	}
	_libWrapper = _init(wrapper);
	_initialized = (_libWrapper != NULL);
	return _initialized;
}

void Library::fin() {
	if (!_initialized) {
		LOG(ERROR) << "Library has not been initialized!";
	} else {
		_fin();
		_init = NULL;
		_fin = NULL;
		base::UnloadNativeLibrary(_handle);
		_handle = NULL;
		_libWrapper = NULL;
		_initialized = false;
	}
}

std::string Library::name() const {
	CHECK( _initialized );
	return _libWrapper->name();
}

std::string Library::version() const {
	CHECK( _initialized );
	return _libWrapper->version();
}

}
