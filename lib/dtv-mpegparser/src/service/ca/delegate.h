#pragma once

#include "types.h"
#include <boost/function.hpp>

namespace tuner {

class Service;

namespace ca {

class Conditional;
class Decrypter;

class Delegate {
public:
	explicit Delegate( Conditional * );
	virtual ~Delegate();

	//	Notifications
	typedef boost::function<void (ID, Keys *)> UpdateKeys;
	void onKeys( const UpdateKeys &cb );

	//	Decrypter
	virtual int canDecrypt( ID srvID ) const;
	virtual Decrypter *createDecrypter( ID srvID );

	//	Start
	virtual void start();
	virtual void stop();

	//	Notifications
	virtual void onCatChanged();
	virtual void onStart( Service * /*srv*/ );
	virtual void onStop( Service * /*srv*/ );

protected:
	void updateKeys( ID srvID, const Keys &keys );
	Conditional *ca() const;

private:
	Conditional *_ca;
	UpdateKeys _updateKeys;
};

}
}

