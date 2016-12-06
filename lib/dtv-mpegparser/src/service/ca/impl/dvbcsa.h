#pragma once

#include "../decrypter.h"

struct dvbcsa_key_s;

namespace tuner {
namespace ca {
namespace dvbcsa {

class Decrypter : public ca::Decrypter {
public:
	explicit Decrypter( ID );
	virtual ~Decrypter();

protected:
	virtual bool decodePayload( bool evenKey, util::BYTE *ptr, size_t len );
	virtual void updateKeys( const Keys &keys );

private:
	struct dvbcsa_key_s *_csaKey[2];
};

}
}
}

