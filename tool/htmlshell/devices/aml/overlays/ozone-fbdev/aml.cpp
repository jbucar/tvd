#include "amlmediaozoneplatform.h"
#include "ui/gfx/native_widget_types.h"
#include <EGL/fbdev_window.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

namespace tvd {

void set_sysfs_str( const char *path, const char *val ) {
	int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd >= 0) {
		write(fd, val, strlen(val));
		close(fd);
	} else {
		LOG(FATAL) << "Cannot set sysfs (string): path=" << path << ", val=" << val;
	}
}

void set_sysfs_int( const char *path, const int val ) {
	char bcmd[16];
	sprintf(bcmd, "%d", val);
	set_sysfs_str(path, bcmd);
}

gfx::AcceleratedWidget createAcceleratedWidget( int w, int h ) {
	fbdev_window *win = (fbdev_window*) malloc( sizeof(fbdev_window) );
	CHECK(win) << "Fail to allocate fbdev_window structure";

	win->width = w;
	win->height = h;
	LOG(INFO) << "Created aml fbdev window of size=" << w << "x" << h;

	set_sysfs_str("/sys/class/display/mode", (h > 720) ? "1080p" : "720p");
	// TODO: run setfb -fb /dev/fb0 -xres w -yres h
	set_sysfs_int("/sys/class/graphics/fb0/blank",0);
	set_sysfs_int("/sys/class/graphics/fb1/blank",1);

	return (gfx::AcceleratedWidget) win;
}

void destroyAcceleratedWidget( gfx::AcceleratedWidget *wgt ) {
	set_sysfs_int("/sys/class/graphics/fb0/blank",1);
	set_sysfs_int("/sys/class/graphics/fb1/blank",1);

	free((fbdev_window*) *wgt);
	*wgt = 0;
}

media::MediaOzonePlatform* createMediaOzonePlatform() {
	return new aml::MediaOzonePlatform();
}

}
