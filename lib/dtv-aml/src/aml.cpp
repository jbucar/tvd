#include "aml.h"
#include <boost/algorithm/string.hpp>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stropts.h>

namespace aml {
namespace sysfs {

bool set(const char *path, const char *val) {
	int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd >= 0) {
		write(fd, val, strlen(val));
		close(fd);
		return true;
	}
	printf( "[aml] Cannot set sysfs (string): path=%s, val=%s\n", path, val );
	return false;
}

bool get(const char *path, char *valstr, const int size) {
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		int len = read(fd, valstr, size - 1);
		valstr[len-1] = '\0';
		close(fd);
		return true;
	}
	printf( "[aml] Cannot get sysfs (string): path=%s\n", path );
	sprintf(valstr, "%s", "fail");
	return false;
}

bool set(const char *path, const int val) {
	int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd >= 0) {
		char bcmd[16];
		sprintf(bcmd, "%d", val);
		write(fd, bcmd, strlen(bcmd));
		close(fd);
		return true;
	}
	printf( "[aml] Cannot set sysfs (int): path=%s, val=%d\n", path, val );
	return false;
}

bool get(const char *path, int &val) {
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		char bcmd[16];
		read(fd, bcmd, sizeof(bcmd));
		val = strtol(bcmd, NULL, 16);
		close(fd);
		return true;
	}
	printf( "[aml] Cannot get sysfs (int): path=%s\n", path );
	return false;
}

}	//	namespace sysfs


namespace video {

bool enable( bool state ) {
	return sysfs::set("/sys/class/video/disable_video", state ? 0 : 1);
}

static bool set_stb_source( const char *src ) {
	int fd;
	const char *path = "/sys/class/stb/source";
	char  bcmd[16];
	fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd >= 0) {
		sprintf(bcmd, "%s", src);
		write(fd, bcmd, strlen(bcmd));
		close(fd);
		return true;
	}
	printf( "[aml] Cannot set source %s\n", src );
	return false;
}

static bool set_stb_demux_source( int demuxID, const char *src ) {
	int fd;
	char path[64];
	sprintf( path, "/sys/class/stb/demux%d_source", demuxID );
	char  bcmd[16];
	fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd >= 0) {
		sprintf(bcmd, "%s", src );
		write(fd, bcmd, strlen(bcmd));
		close(fd);
		return true;
	}
	printf( "[aml] Cannot set stb demux(%d) source %s\n", demuxID, src );
	return false;
}

bool setDemux( const char *src ) {
	if (!set_stb_source( src )) {
		return false;
	}

	if (!set_stb_demux_source( 1, src )) {
		return false;
	}

	return true;
}

bool setSyncMode( bool enable ) {
	return sysfs::set( "/sys/class/tsync/enable", enable ? 1 : 0 );
}

bool setAxis( int x, int y, int w, int h ) {
	printf( "[aml] Set video axis: w=%d,y=%d,w=%d,y=%d\n", x, y, w, h );
	char video_axis[256] = {0};
	sprintf(video_axis, "%d %d %d %d", x, y, w, h );
	sysfs::set("/sys/class/video/axis", video_axis);
	// make sure we are in 'full stretch' so we can stretch
	sysfs::set("/sys/class/video/screen_mode", 1);
	return true;
}

}	//	namespace video

namespace layer {

bool enable( bool state ) {
	bool result=sysfs::set("/sys/class/graphics/fb0/blank", state ? 0 : 1 );
	result &= sysfs::set("/sys/class/graphics/fb1/blank", 1 );
	if (!result) {
		printf( "[aml] Cannot enable canvas layer: state=%d\n", state );
	}
	return result;
}

}

namespace display {

bool setAxis( int x, int y, int w, int h ) {
	printf( "[aml] Set display axis: (%d,%d,%d,%d)\n", x, y, w, h );
	char daxis_str[255] = {0};
	sprintf(daxis_str, "%d %d %d %d 0 0 0 0", x, y, w-1, h-1);
	return sysfs::set("/sys/class/display/axis", daxis_str);
}

static void enableFreeScale() {
	printf( "[aml] Enable free scale\n" );

	// enable OSD free scale using frame buffer size of 720p
	sysfs::set("/sys/class/graphics/fb0/free_scale", 0);
	sysfs::set("/sys/class/graphics/fb1/free_scale", 0);
	sysfs::set("/sys/class/graphics/fb0/scale_width",  1280);
	sysfs::set("/sys/class/graphics/fb0/scale_height", 720);
	sysfs::set("/sys/class/graphics/fb1/scale_width",  1280);
	sysfs::set("/sys/class/graphics/fb1/scale_height", 720);

	// enable video free scale (scaling to 1920x1080 with frame buffer size 1280x720)
	sysfs::set("/sys/class/ppmgr/ppscaler", 0);
	sysfs::set("/sys/class/video/disable_video", 1);
	sysfs::set("/sys/class/ppmgr/ppscaler", 1);
	sysfs::set("/sys/class/ppmgr/ppscaler_rect", "0 0 1920 1080 0");
	sysfs::set("/sys/class/ppmgr/disp", "1280 720");
	//
	sysfs::set("/sys/class/graphics/fb0/scale_width",  1280);
	sysfs::set("/sys/class/graphics/fb0/scale_height", 720);
	sysfs::set("/sys/class/graphics/fb1/scale_width",  1280);
	sysfs::set("/sys/class/graphics/fb1/scale_height", 720);
	//
	sysfs::set("/sys/class/video/disable_video", 2);
	sysfs::set("/sys/class/display/axis", "0 0 1280 720 0 0 0 0");
	sysfs::set("/sys/class/ppmgr/ppscaler_rect", "0 0 1280 720 1");
	//
	sysfs::set("/sys/class/graphics/fb0/free_scale", 1);
	sysfs::set("/sys/class/graphics/fb1/free_scale", 1);
	sysfs::set("/sys/class/graphics/fb0/free_scale_axis", "0 0 1280 720");
}

static void disableFreeScale() {
	printf( "[aml] Disable free scale\n" );

	// turn off frame buffer freescale
	sysfs::set("/sys/class/graphics/fb0/free_scale", 0);
	sysfs::set("/sys/class/graphics/fb1/free_scale", 0);
	sysfs::set("/sys/class/graphics/fb0/free_scale_axis", "0 0 1280 720");

	sysfs::set("/sys/class/ppmgr/ppscaler", 0);
	sysfs::set("/sys/class/video/disable_video", 0);
	sysfs::set("/sys/class/display/axis", "0 0 1280 720 0 0 0 0");
}

void resetFreeScale() {
	std::string mode;
	getMode( mode );
	if (!strncmp( "1080", mode.c_str(), 4 )) {
		enableFreeScale();
	}
	else {
		disableFreeScale();
	}
}

#define MAX_DISPLAY_CAPS 1024
bool getModes( std::vector<std::string> &modes ) {
	const char *path = "/sys/class/amhdmitx/amhdmitx0/disp_cap";
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		bool exit=false;
		while (!exit) {
			int len=0;
			char str[MAX_DISPLAY_CAPS];
			len = read(fd, str, MAX_DISPLAY_CAPS-1);
			if (len > 0) {
				str[len-1] = '\0';
				//LTRACE( "aml", "Display caps=%s", str );
				boost::split( modes, str, boost::is_any_of("\n"));
			}
			else  {
				exit=true;
			}
		}
		close(fd);
		return true;
	}
	printf( "[aml] Cannot read display modes\n" );
	return false;
}

bool setMode( const std::string &mode ) {
	bool ret=sysfs::set("/sys/class/display/mode",mode.c_str());
	if (ret) {
		resetFreeScale();
		return true;
	}
	printf( "[aml] Cannot set display mode: mode=%s\n", mode.c_str() );
	return false;
}

bool getMode( std::string &mode ) {
	char str_mode[255];
	if (sysfs::get("/sys/class/display/mode", str_mode, 255 )) {
		mode = str_mode;
		return true;
	}
	printf( "[aml] Cannot get screen mode\n" );
	return false;
}

}	//	namespace display

}

