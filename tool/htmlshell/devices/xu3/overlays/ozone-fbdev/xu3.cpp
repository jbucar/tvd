#include "media/ozone/media_ozone_platform.h"
#include "ui/gfx/native_widget_types.h"
#include <EGL/eglplatform.h>
#include <stdio.h>

namespace tvd {

gfx::AcceleratedWidget createAcceleratedWidget( int w, int h ) {
	fbdev_window *win = (fbdev_window*) malloc( sizeof(fbdev_window) );
	CHECK(win) << "Fail to allocate fbdev_window structure";
	win->width = w;
	win->height = h;
	return (gfx::AcceleratedWidget) win;
}

void destroyAcceleratedWidget( gfx::AcceleratedWidget *wgt ) {
	free((fbdev_window*) *wgt);
	*wgt = 0;
}

media::MediaOzonePlatform* createMediaOzonePlatform() {
	return new media::MediaOzonePlatform();
}

}
