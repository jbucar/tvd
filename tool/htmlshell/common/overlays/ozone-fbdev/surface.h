#pragma once

#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/public/surface_ozone_egl.h"

namespace tvd {

class Surface : public ui::SurfaceOzoneEGL {
public:
	explicit Surface( gfx::AcceleratedWidget win );
	~Surface() override;

	intptr_t GetNativeWindow() override;
	bool OnSwapBuffers() override;
	void OnSwapBuffersAsync( const ui::SwapCompletionCallback &callback ) override;
	bool ResizeNativeWindow( const gfx::Size &viewport_size ) override;
	scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() override;

private:
	gfx::AcceleratedWidget _win;
};

}
