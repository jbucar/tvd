#pragma once

#include "ui/ozone/public/surface_factory_ozone.h"

namespace ui {
class SurfaceOzoneEGL;
}

namespace tvd {

class SurfaceFactory : public ui::SurfaceFactoryOzone {
public:
	explicit SurfaceFactory( gfx::AcceleratedWidget rootWidget );
	~SurfaceFactory() override;

	intptr_t GetNativeDisplay() override;
	scoped_ptr<ui::SurfaceOzoneEGL> CreateEGLSurfaceForWidget( gfx::AcceleratedWidget widget ) override;
	const int32_t* GetEGLSurfaceProperties( const int32_t *desired_list ) override;
	bool LoadEGLGLES2Bindings( AddGLLibraryCallback addCB, SetGLGetProcAddressProcCallback setCB ) override;

private:
	gfx::AcceleratedWidget _rootWidget;
};

}
