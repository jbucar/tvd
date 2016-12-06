#include "types.h"
#include "ginga.h"
#include <util/registrator.h>
#include <util/log.h>
#include <util/main.h>
#include <util/assert.h>
#include <stdio.h>

namespace lifia {

namespace impl {
	static Ginga *ginga = NULL;
}

//	Initialization
LIFIA_API bool init( Wrapper *wrapper ) {
	DTV_ASSERT(!impl::ginga);
	util::main::init( "gingawrapper", "1.0.0" );
	util::log::setLevel("all", "all", "debug");
	try {
		impl::ginga = new Ginga( wrapper );
	} catch(...) {
		printf( "Cannot initialize Ginga middleware\n" );
		return false;
	}
	return true;
}

LIFIA_API void fin() {
	delete impl::ginga;
	impl::ginga = NULL;
	util::main::fin();
}

//	Notifications
LIFIA_API void onNotification() {
	if (impl::ginga) {
		impl::ginga->onNotification();
	}
}

//	Play service
LIFIA_API void play( int freq, int srvID ) {
	DTV_ASSERT(impl::ginga);
	impl::ginga->play( freq, srvID );
}

LIFIA_API void stop() {
	DTV_ASSERT(impl::ginga);
	impl::ginga->stop();
}

//	Keys
LIFIA_API bool dispatchKey( int key, bool isUp ) {
	DTV_ASSERT(impl::ginga);
	return impl::ginga->dispatchKey( (util::key::type)key, isUp );
}

//	Applications
LIFIA_API void getApplications( std::vector<std::string> &apps ) {
	DTV_ASSERT(impl::ginga);
	impl::ginga->getApplications( apps );
}

LIFIA_API void startApplication( int id ) {
	DTV_ASSERT(impl::ginga);
	impl::ginga->startApplication( id );
}


}
