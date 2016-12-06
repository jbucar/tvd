#include "application.h"
#include <canvas/remote/server.h>
#include <mpegparser/service/extension/application/extension.h>
#include <mpegparser/service/extension/application/controller/spawner.h>
#include <mpegparser/service/extension/application/application.h>
#include <connector/handler/videoresizehandler.h>
#include <util/keydefs.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace lifia {

canvas::remote::Server *createRender( Wrapper *wrapper, util::task::Dispatcher *disp );

struct FindAppByAppID {
	FindAppByAppID( const tuner::app::ApplicationID &appID ) : _appID( appID ) {}

	virtual bool operator()( Application *app ) const {
		return app->appID() == _appID;
	}

	const tuner::app::ApplicationID &_appID;
};

struct FindAppByID {
	FindAppByID( int id ) : _id( id ) {}

	virtual bool operator()( Application *app ) const {
		return app->id() == _id;
	}

	int _id;
};


//	Application
Application::Application( tuner::app::Application *app )
	: _appID( app->appID() ), _name( app->name() )
{
	_id = -1;
}

Application::Application( const Application &app )
	: _appID( app._appID ), _name( app._name )
{
	_id = app._id;
}

Application::~Application()
{
}

Application &Application::operator=( const Application &app ) {
	_appID  = app._appID;
	_name   = app._name;
	_id     = app._id;
	return (*this);
}

void Application::setID( int id ) {
	_id = id;
}

const tuner::app::ApplicationID &Application::appID() const {
	return _appID;
}

const std::string &Application::name() const {
	return _name;
}

int Application::id() const {
	return _id;
}

//	App controlller
AppController::AppController( Wrapper *wrapper, util::task::Dispatcher *disp )
	: _wrapper( wrapper )
{
	//	Create render with application resolution
	_render = createRender( wrapper, disp );

	{	//	Setup render resolution
		const canvas::Size size(wrapper->videoWidth, wrapper->videoHeight);
		if (!_render->initialize( size, 0 )) {
			throw "Cannot initialize render";
		}
	}

	//	Create spawner
	_spawner = new tuner::app::Spawner();

	//	Setup default video resolution
	setOutputResolution( wrapper->videoWidth, wrapper->videoHeight );
}

AppController::~AppController()
{
	_render->finalize();
	delete _render;

	BOOST_FOREACH( Application *app, _currentApps ) {
		delete app;
	}
	delete _spawner;
}

void AppController::setOutputResolution( int w, int h ) {
	_video.w = w;
	_video.h = h;
}

//	Zapper methods
void AppController::getApplications( std::vector<std::string> &apps ) {
	//	Get applications
	apps.clear();
	Application *app;
	for (int i=0; i<(int)_currentApps.size(); i++) {
		//	Set ID
		app = _currentApps[i];
		app->setID( i );
		apps.push_back( app->name() );
	}
}

void AppController::startApp( int id ) {
	printf( "[lifia::AppController] start begin: id=%d\n", id );
	FindAppByID finder( id );
	std::vector<Application *>::const_iterator it=std::find_if( _currentApps.begin(), _currentApps.end(), finder );
	if (it != _currentApps.end()) {
		printf( "[lifia::AppController] start begin: id=%d, appID=%s\n", id, (*it)->appID().asString().c_str() );
		extension()->start( (*it)->appID() );
	}
	printf( "[lifia::AppController] start end: id=%d\n", id );
}

//	Controller virtual functions
void AppController::onApplicationAdded( tuner::app::Application *app ) {
	printf( "[lifia::AppController] Application added begin\n" );
	_currentApps.push_back( new Application( app ) );
	printf( "[lifia::AppController] Application added end\n" );
}

void AppController::onApplicationRemoved( tuner::app::Application *app ) {
	printf( "[lifia::AppController] Application removed begin\n" );
	FindAppByAppID finder(app->appID());
	std::vector<Application *>::iterator it=std::find_if( _currentApps.begin(), _currentApps.end(), finder );
	if (it != _currentApps.end()) {
		delete (*it);
		_currentApps.erase( it );
	}
	printf( "[lifia::AppController] Application removed end\n" );
}

//	Start/stop application
void AppController::start( tuner::app::RunTask *task ) {
	_spawner->run( task );
}

void AppController::stop( tuner::app::RunTask *task, bool kill ) {
	_spawner->stop( task, kill );
}

//	Reserve keys
void AppController::reserveKeys( const tuner::app::ApplicationID & /*id*/, const tuner::app::Keys &keys, int /*priority*/ ) {
	//printf( "[lifia::AppController] Reserve %d keys\n", keys.size() );
	_wrapper->reserveKeys( keys, _wrapper->user_ptr );
}

bool AppController::sendKey( util::key::type value, bool isUp ) {
	return extension()->dispatchKey( value, isUp );
}

//	Graphic layer
bool AppController::setupLayer( tuner::app::Application * /*app*/, int &width, int &heigth ) {
	width = _video.w;
	heigth = _video.h;
	return true;
}

void AppController::enableLayer( bool st ) {
	_render->enable( st );
}

tuner::app::PlayerAdapter *AppController::createMainPlayerAdapter() const {	//	TODO:
	return NULL;
}

}
