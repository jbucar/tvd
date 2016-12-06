#include "dvbcsa.h"
#include <util/assert.h>
extern "C" {
	#include <dvbcsa/dvbcsa.h>
}
#include <string.h>

#define KEY_EVEN 0
#define KEY_ODD  1

namespace tuner {
namespace ca {
namespace dvbcsa {

Decrypter::Decrypter( ID srvID )
	: ca::Decrypter( srvID )
{
	//	Create dvbcsa keys
	_csaKey[KEY_EVEN] = dvbcsa_key_alloc();
	_csaKey[KEY_ODD] = dvbcsa_key_alloc();

	{	//	Clean keys
		Keys keys;
		memset(keys.even, 0, CW_KEY_SIZE);
		memset(keys.odd, 0, CW_KEY_SIZE);
		updateKeys( keys );
	}
}

Decrypter::~Decrypter()
{
	dvbcsa_key_free( _csaKey[KEY_EVEN] );
	dvbcsa_key_free( _csaKey[KEY_ODD] );
}

void Decrypter::updateKeys( const Keys &keys ) {
	DTV_ASSERT(_csaKey[KEY_EVEN] );
	DTV_ASSERT(_csaKey[KEY_ODD] );

	//	Update key
	dvbcsa_key_set( keys.even, _csaKey[KEY_EVEN] );
	dvbcsa_key_set( keys.odd, _csaKey[KEY_ODD] );
}

bool Decrypter::decodePayload( bool evenKey, util::BYTE *ptr, size_t len ) {
	DTV_ASSERT(_csaKey[KEY_EVEN] );
	DTV_ASSERT(_csaKey[KEY_ODD] );

	int index = evenKey ? KEY_EVEN : KEY_ODD;
	dvbcsa_decrypt( _csaKey[index], ptr, len );
	return true;
}

}
}
}

