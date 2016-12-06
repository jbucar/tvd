#pragma once

#include "../decrypter.h"

namespace tuner {
namespace ca {
namespace dummy {

class Decrypter : public ca::Decrypter {
public:
	explicit Decrypter( ID );
	virtual ~Decrypter();

protected:
	virtual bool decodePayload( bool evenKey, util::BYTE *ptr, size_t len );
	virtual void updateKeys( const Keys &keys );
};

}
}
}

