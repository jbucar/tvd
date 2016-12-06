#include "dtv-htmlshellsdk/src/libraryinterface.h"

namespace tvd {

// Test lib init failure

// Library api implementation
LibWrapper *init( HtmlShellWrapper * /*wrapper*/ ) {
	return NULL;
}

void fin() {
}

}
