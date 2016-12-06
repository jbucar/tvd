#pragma once

#include "types.h"
#include <mpegparser/service/extension/application/controller/controller.h>
#include <map>

namespace util {
namespace task {
	class Dispatcher;
}
}

namespace tuner {
	namespace app {
		class Spawner;
	}
}

namespace canvas {
namespace remote {
	class Server;
}
}

namespace lifia {

class Render;

class Application {
public:
	Application( tuner::app::Application *app );
	Application( const Application &app );
	virtual ~Application( void );

	Application &operator=( const Application &app );
	void setID( int id );

	const tuner::app::ApplicationID &appID() const;
	const std::string &name() const;
	int id() const;

private:
	tuner::app::ApplicationID _appID;
	std::string _name;
	int _id;
};

class AppController : public tuner::app::Controller {
public:
	AppController( Wrapper *wrapper, util::task::Dispatcher *disp );
	virtual ~AppController();

	//	Zapper methods
	void startApp( int id );
	void getApplications( std::vector<std::string> &apps );
	void setOutputResolution( int w, int h );
	bool sendKey( util::key::type value, bool isUp );

	//	Controller virtual functions (service thread)
	virtual void onApplicationAdded( tuner::app::Application *app );
	virtual void onApplicationRemoved( tuner::app::Application *app );
	virtual void reserveKeys( const tuner::app::ApplicationID &id, const tuner::app::Keys &keys, int priority );

	//	Start/stop application
	virtual void start( tuner::app::RunTask *task );
	virtual void stop( tuner::app::RunTask *task, bool kill );

	//	Graphic layer
	virtual bool setupLayer( tuner::app::Application *app, int &width, int &heigth );
	virtual void enableLayer( bool layer );

protected:
	typedef struct {
		int w;
		int h;
	} Resolution;
	virtual tuner::app::PlayerAdapter *createMainPlayerAdapter() const;

private:
	Wrapper *_wrapper;
	Resolution _video;
	canvas::remote::Server *_render;
	tuner::app::Spawner *_spawner;
	std::vector<Application *> _currentApps;
};

}
