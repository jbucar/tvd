#include "dtv-htmlshellsdk/src/libraryinterface.h"

namespace tvd {

// Test lib init successfully
static LibWrapper *libWrapper = NULL;

// Library api implementation
LibWrapper *init( HtmlShellWrapper * /*wrapper*/ ) {
	libWrapper = new LibWrapper("InitSuccess", "1.0");
	return libWrapper;
}

void fin() {
	delete libWrapper;
	libWrapper = NULL;
}

}
