#pragma once

#include <vector>
#include <string>

namespace aml {

namespace sysfs {

//	sysfs operations
bool set(const char *path, const char *val);
bool get(const char *path, char *valstr, const int size);
bool set(const char *path, const int val);
bool get(const char *path, int &val);

}	//	namespace sysfs

namespace video {

//	Video operations
bool enable( bool state );
bool setDemux( const char *src );
bool setSyncMode( bool enable );
bool setAxis( int x, int y, int w, int h );

}	//	namespace video

namespace layer {

bool enable( bool state );

}	//	namespace layer

namespace display {

namespace vo {
	enum type { cvbs=0, hdmi };
}

//	Display operations
bool getModes( std::vector<std::string> &caps );
bool getMode( std::string &mode );
bool setMode( const std::string &mode );
bool setAxis( int x, int y, int w, int h );
void resetFreeScale();

}	//	namespace display

}	//	namespace aml

