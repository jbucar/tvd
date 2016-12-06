#include "platform.h"
#include "cursordelegate.h"
#include "surfacefactory.h"
#include "window.h"
#include "htmlshell/src/util.h"
#include "media/ozone/media_ozone_platform.h"
#include "ui/base/cursor/ozone/bitmap_cursor_factory_ozone.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/ozone/common/native_display_delegate_ozone.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/events/ozone/device/device_manager.h"

namespace tvd {

Platform::Platform()
{
	LOG(INFO) << "Creating ozone-fbdev platform";
	gfx::Size winSize = util::getWindowSize();
	_accelWidget = createAcceleratedWidget( winSize.width(), winSize.height() );
}

Platform::~Platform()
{
	LOG(INFO) << "Destroying ozone-fbdev platform";
	destroyAcceleratedWidget(&_accelWidget);
}

ui::SurfaceFactoryOzone *Platform::GetSurfaceFactoryOzone(){
	return _surfaceFactory.get();
}

ui::CursorFactoryOzone *Platform::GetCursorFactoryOzone() {
	return _cursorFactory.get();
}

ui::GpuPlatformSupport *Platform::GetGpuPlatformSupport() {
	return _gpuPlatformSupport.get();
}

ui::GpuPlatformSupportHost *Platform::GetGpuPlatformSupportHost() {
	return _gpuPlatformSupportHost.get();
}

scoped_ptr<ui::NativeDisplayDelegate> Platform::CreateNativeDisplayDelegate() {
	return scoped_ptr<ui::NativeDisplayDelegate>(new ui::NativeDisplayDelegateOzone());
}

ui::InputController *Platform::GetInputController() {
	return _eventFactory->input_controller();
}

scoped_ptr<ui::SystemInputInjector> Platform::CreateSystemInputInjector() {
	return nullptr;
}

ui::OverlayManagerOzone *Platform::GetOverlayManager() {
	return _overlayMgr.get();
}

void Platform::InitializeUI() {
	LOG(INFO) << "Platform::InitializeUI: this=" << this;
	_deviceMgr = ui::CreateDeviceManager();
	_overlayMgr.reset(new ui::StubOverlayManager());
	_cursorDelegate.reset(new CursorDelegate());
	if (!_surfaceFactory.get()) {
		_surfaceFactory.reset(new SurfaceFactory(_accelWidget));
	}
	ui::KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(make_scoped_ptr(new ui::StubKeyboardLayoutEngine()));
	_eventFactory.reset(new ui::EventFactoryEvdev(_cursorDelegate.get(), _deviceMgr.get(), ui::KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()));
	_cursorFactory.reset(new ui::BitmapCursorFactoryOzone());
	_gpuPlatformSupportHost.reset(ui::CreateStubGpuPlatformSupportHost());
}

void Platform::InitializeGPU() {
	LOG(INFO) << "Platform::InitializeGPU: this=" << this;
	if (!_surfaceFactory.get()) {
		_surfaceFactory.reset(new SurfaceFactory(_accelWidget));
	}
	_gpuPlatformSupport.reset(ui::CreateStubGpuPlatformSupport());
}

scoped_ptr<ui::PlatformWindow> Platform::CreatePlatformWindow( ui::PlatformWindowDelegate *delegate, const gfx::Rect &bounds ) {
	delegate->OnAcceleratedWidgetAvailable(_accelWidget, 1.f);
	return make_scoped_ptr<ui::PlatformWindow>(new Window(delegate, bounds));
}

base::ScopedFD Platform::OpenClientNativePixmapDevice() const {
	return base::ScopedFD();
}

}
