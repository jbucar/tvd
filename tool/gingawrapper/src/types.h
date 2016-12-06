#pragma once

#include <util/keydefs.h>
#include <vector>
#include <string>

#ifdef _WIN32
#	ifdef LIFIA_EXPORTS
#		define LIFIA_API __declspec(dllexport)
#	else
#		define LIFIA_API __declspec(dllimport)
#	endif
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#	define LAYER_TYPE HWND
#else
#	define LAYER_TYPE long int
#	define LIFIA_API
#   define WINAPI
#	define CALLBACK
#endif

namespace lifia {

typedef int (CALLBACK *TSCallback)( unsigned char *Data, int iLength, void *user_data );
typedef bool (*SetupTSCallback)( TSCallback fnc, void *method_data, void *user_data );
typedef void (*ShowLayer)( bool needShow, void *user_data );
typedef void (*ReserveKeys)( const std::vector<util::key::type> &keys, void *user_data );
typedef void (*Notify)( void *user_data );
typedef void (*ResizeVideo)( int x, int y, int w, int h, void *user_data );

typedef struct {
	void *user_ptr;
	LAYER_TYPE layer;
	SetupTSCallback setupTSCallback;
	Notify notify;
	ShowLayer showLayer;
	ResizeVideo resizeVideo;
	ReserveKeys reserveKeys;
	int videoWidth;
	int videoHeight;
} Wrapper;

//	Initialization
LIFIA_API bool init( Wrapper *wrapper );
LIFIA_API void fin();

//	Task notifications
LIFIA_API void onNotification();

//	Play service
LIFIA_API void play( int freq, int srvID );
LIFIA_API void stop();

//	Keys
LIFIA_API bool dispatchKey( util::key::type key, bool isUp );

//	Applications
LIFIA_API void getApplications( std::vector<std::string> &apps );
LIFIA_API void startApplication( int id );

}

