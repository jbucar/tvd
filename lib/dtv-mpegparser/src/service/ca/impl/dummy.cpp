#include "dummy.h"
#include <util/assert.h>
#include <string.h>

namespace tuner {
namespace ca {
namespace dummy {

Decrypter::Decrypter( ID srvID )
	: ca::Decrypter( srvID )
{
}

Decrypter::~Decrypter()
{
}

void Decrypter::updateKeys( const Keys & /*keys*/ ) {
}

bool Decrypter::decodePayload( bool /*evenKey*/, util::BYTE * /*ptr*/, size_t /*len*/ ) {
	return true;
}

}
}
}

