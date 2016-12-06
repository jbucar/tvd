#pragma once

#include "ui/ozone/public/ozone_platform.h"
#include "ui/gfx/native_widget_types.h"

namespace media {
class MediaOzonePlatform;
}

namespace ui {
class EventFactoryEvdev;
class DeviceManager;
}

namespace tvd {

gfx::AcceleratedWidget createAcceleratedWidget( int w, int h );
void destroyAcceleratedWidget( gfx::AcceleratedWidget *wgt );
media::MediaOzonePlatform *createMediaOzonePlatform();

class CursorDelegate;
class SurfaceFactory;

class Platform : public ui::OzonePlatform {
public:
	Platform();
	~Platform() override;

	// ui::OzonePlatform implementation:
	ui::SurfaceFactoryOzone *GetSurfaceFactoryOzone() override;
	ui::CursorFactoryOzone *GetCursorFactoryOzone() override;
	ui::GpuPlatformSupport *GetGpuPlatformSupport() override;
	ui::GpuPlatformSupportHost *GetGpuPlatformSupportHost() override;
	scoped_ptr<ui::NativeDisplayDelegate> CreateNativeDisplayDelegate() override;
	ui::InputController *GetInputController() override;
	scoped_ptr<ui::SystemInputInjector> CreateSystemInputInjector() override;
	ui::OverlayManagerOzone* GetOverlayManager() override;
	base::ScopedFD OpenClientNativePixmapDevice() const override;

	void InitializeUI() override;
	void InitializeGPU() override;

	scoped_ptr<ui::PlatformWindow> CreatePlatformWindow( ui::PlatformWindowDelegate *delegate, const gfx::Rect &bounds ) override;

private:
	gfx::AcceleratedWidget _accelWidget;
	scoped_ptr<CursorDelegate> _cursorDelegate;
	scoped_ptr<SurfaceFactory> _surfaceFactory;
	scoped_ptr<ui::DeviceManager> _deviceMgr;
	scoped_ptr<ui::EventFactoryEvdev> _eventFactory;
	scoped_ptr<ui::CursorFactoryOzone> _cursorFactory;
	scoped_ptr<ui::GpuPlatformSupport> _gpuPlatformSupport;
	scoped_ptr<ui::GpuPlatformSupportHost> _gpuPlatformSupportHost;
	scoped_ptr<ui::OverlayManagerOzone> _overlayMgr;

	DISALLOW_COPY_AND_ASSIGN(Platform);
};

}
