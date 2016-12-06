#include "media/ozone/media_ozone_platform.h"
#include "ui/gfx/native_widget_types.h"
#include <bcm_host.h>
#include <EGL/eglplatform.h>
#include <stdio.h>

namespace tvd {

gfx::AcceleratedWidget createAcceleratedWidget( int w, int h ) {
	EGL_DISPMANX_WINDOW_T *win = (EGL_DISPMANX_WINDOW_T*) malloc( sizeof(EGL_DISPMANX_WINDOW_T) );
	CHECK(win) << "Fail to allocate EGL_DISPMANX_WINDOW_T structure";

	bcm_host_init();

	DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open(0);
	DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

	VC_RECT_T dest = { 0, 0, w, h };
	VC_RECT_T src = { 0, 0, w << 16, h << 16 };

	win->element = vc_dispmanx_element_add(update, display, 0, &dest, 0, &src, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);
	win->width = w;
	win->height = h;
	vc_dispmanx_update_submit_sync(update);

	return (gfx::AcceleratedWidget) win;
}


void destroyAcceleratedWidget( gfx::AcceleratedWidget *wgt ) {
	free((EGL_DISPMANX_WINDOW_T*) *wgt);
	*wgt = 0;
}

media::MediaOzonePlatform* createMediaOzonePlatform() {
	return new media::MediaOzonePlatform();
}

}
