#pragma once

#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "base/native_library.h"
#include "base/files/file_path.h"

namespace tvd {

class Library {
public:
	// Construction
	static Library *createFromPath( const base::FilePath &libPath );
	virtual ~Library();

	bool init( HtmlShellWrapper *wrapper );
	void fin();

	std::string name() const;
	std::string version() const;

private:
	bool _initialized;
	base::NativeLibrary _handle;
	InitFn _init;
	FinFn _fin;
	LibWrapper *_libWrapper;

	Library( base::NativeLibrary handle, InitFn init, FinFn fin );
};

}
