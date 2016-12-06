#pragma once

#include "types.h"
#include <util/keydefs.h>
#include <string>
#include <vector>

namespace tuner {
	class ServiceProvider;
	class Provider;
}

namespace lifia {

class AppController;
class MyDispatcher;

class Ginga {
public:
	Ginga( Wrapper *wrapper );
	virtual ~Ginga();

	void onNotification();

	void play( int freq, int srvID );
	void stop();

	void getApplications( std::vector<std::string> &apps );
	void startApplication( int index );

	bool dispatchKey( util::key::type key, bool isUp );

protected:
	void startFrequency( int freq );
	void stopFrequency();
	void startService( int serviceID );
	void stopServices( void );
	util::key::type mapKey( int key );

private:
	MyDispatcher *_dispatcher;
	tuner::Provider *_provider;
	tuner::ServiceProvider *_services;
	AppController *_app;
	int _freq;
	int _srvID;
};

}	//	namespace ginga
