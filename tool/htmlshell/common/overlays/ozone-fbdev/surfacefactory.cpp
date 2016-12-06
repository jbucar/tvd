#include "surfacefactory.h"
#include "surface.h"
#include "ui/ozone/common/egl_util.h"
#include "third_party/khronos/EGL/egl.h"

namespace tvd {

SurfaceFactory::SurfaceFactory( gfx::AcceleratedWidget rootWidget )
	: _rootWidget(rootWidget)
{}

SurfaceFactory::~SurfaceFactory()
{}

intptr_t SurfaceFactory::GetNativeDisplay() {
	return (intptr_t)EGL_DEFAULT_DISPLAY;
}

scoped_ptr<ui::SurfaceOzoneEGL> SurfaceFactory::CreateEGLSurfaceForWidget( gfx::AcceleratedWidget widget ) {
	LOG(INFO) << "CreateEGLSurfaceForWidget: widget=0x" << std::hex << widget;
	return make_scoped_ptr<ui::SurfaceOzoneEGL>(new Surface(_rootWidget));
}

bool SurfaceFactory::LoadEGLGLES2Bindings( AddGLLibraryCallback addCB, SetGLGetProcAddressProcCallback setCB ) {
	return ui::LoadDefaultEGLGLES2Bindings(addCB, setCB);
}

const int32_t *SurfaceFactory::GetEGLSurfaceProperties( const int32_t *desired_list ) {
	static const int32_t broken_props[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE,
	};
	return broken_props;
}

}
