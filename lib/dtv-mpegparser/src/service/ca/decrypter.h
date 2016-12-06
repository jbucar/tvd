#pragma once

#include "types.h"

#define CW_KEY_SIZE 8

namespace util {
	class Buffer;
}

namespace tuner {
namespace ca {

class Decrypter {
public:
	explicit Decrypter( ID srvID );
	virtual ~Decrypter();

	bool decrypt( util::Buffer *pkt );
	void updateKeys( ID srvID, const Keys &keys );

	static Decrypter *create( ID srvID, const std::string &use="" );

protected:
	//	Decrypt aux
	virtual bool decodePayload( bool evenKey, util::BYTE *ptr, size_t len )=0;
	virtual void updateKeys( const Keys &keys )=0;

private:
	ID _srvID;
};

}
}

