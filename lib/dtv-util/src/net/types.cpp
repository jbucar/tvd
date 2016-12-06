#include "types.h"
#include <boost/static_assert.hpp>
#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#endif

namespace util {
namespace net {

BOOST_STATIC_ASSERT((int)type::stream == (int)SOCK_STREAM);
BOOST_STATIC_ASSERT((int)type::dgram == (int)SOCK_DGRAM);
BOOST_STATIC_ASSERT((int)type::raw == (int)SOCK_RAW);

DWORD uhtonl( DWORD param ) {
	return htonl( param );
}

WORD uhtons( WORD param ) {
	return htons( param );
}

int getFamily( family::type t ) {
	static int _families[] = { 0, AF_INET, AF_INET6, 0 };
	return _families[t];
}

}
}

