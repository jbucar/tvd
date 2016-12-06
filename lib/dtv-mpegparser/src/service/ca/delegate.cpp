#include "delegate.h"
#include "decrypter.h"
#include <util/assert.h>

namespace tuner {
namespace ca {

Delegate::Delegate( Conditional *ca )
	: _ca(ca)
{
}

Delegate::~Delegate()
{
}

int Delegate::canDecrypt( ID /*srvID*/ ) const {
	return -1;
}

Decrypter *Delegate::createDecrypter( ID srvID ) {
	return Decrypter::create( srvID );
}

//	Notifications
void Delegate::onKeys( const UpdateKeys &cb ) {
	_updateKeys = cb;
}

void Delegate::start() {
}

void Delegate::stop() {
}

//	Notifications
void Delegate::onCatChanged() {
}

void Delegate::onStart( Service * /*srv*/ ) {
}

void Delegate::onStop( Service * /*srv*/ ) {
}

void Delegate::updateKeys( ID srvID, const Keys &keys ) {
	DTV_ASSERT(!_updateKeys.empty());
	_updateKeys( srvID, (Keys *)&keys );
}

Conditional *Delegate::ca() const {
	return _ca;
}

}
}

