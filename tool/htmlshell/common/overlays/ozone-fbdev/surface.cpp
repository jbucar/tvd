#include "surface.h"
#include "base/logging.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/vsync_provider.h"

namespace tvd {

Surface::Surface( gfx::AcceleratedWidget win )
	: _win(win)
{
}

Surface::~Surface()
{}

intptr_t Surface::GetNativeWindow() {
	return (intptr_t) _win;
}

bool Surface::OnSwapBuffers() {
	return true;
}

void Surface::OnSwapBuffersAsync( const ui::SwapCompletionCallback &callback ) {
	callback.Run(gfx::SwapResult::SWAP_ACK);
}

bool Surface::ResizeNativeWindow( const gfx::Size &viewport_size ) {
	LOG(INFO) << "Surface::ResizeNativeWindow width=" << viewport_size.width() << ", height=" << viewport_size.height();
	return true;
}

scoped_ptr<gfx::VSyncProvider> Surface::CreateVSyncProvider() {
	return scoped_ptr<gfx::VSyncProvider>();
}

}
