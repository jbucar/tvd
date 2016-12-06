#include "dtv-htmlshellsdk/src/libraryinterface.h"

namespace tvd {

// Test lib that does not follow the api

// Library api implementation
LibWrapper *initialize( HtmlShellWrapper * /*wrapper*/ ) {
	return NULL;
}

void fin() {
}

}
