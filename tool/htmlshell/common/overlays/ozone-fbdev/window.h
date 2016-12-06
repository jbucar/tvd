#pragma once

#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/platform_window/platform_window.h"

namespace ui {
class PlatformWindowDelegate;
}

namespace tvd {

class Window : public ui::PlatformWindow,
               public ui::PlatformEventDispatcher {
public:
	Window( ui::PlatformWindowDelegate *delegate, const gfx::Rect &bounds );
	~Window() override;

	// PlatformWindow:
	gfx::Rect GetBounds() override;
	void SetBounds( const gfx::Rect &bounds ) override;
	void SetTitle(const base::string16& title) override {}
	void Show() override {}
	void Hide() override {}
	void Close() override {}
	void SetCapture() override {}
	void ReleaseCapture() override {}
	void ToggleFullscreen() override {}
	void Maximize() override {}
	void Minimize() override {}
	void Restore() override {}
	void SetCursor( ui::PlatformCursor cursor ) override {}
	void MoveCursorTo( const gfx::Point &location ) override {}
	void ConfineCursorToBounds( const gfx::Rect &bounds ) override {}
	ui::PlatformImeController *GetPlatformImeController() override;

	// PlatformEventDispatcher:
	bool CanDispatchEvent( const ui::PlatformEvent &event ) override;
	uint32_t DispatchEvent( const ui::PlatformEvent &event ) override;

private:
	ui::PlatformWindowDelegate *_delegate;
	gfx::Rect _bounds;

	DISALLOW_COPY_AND_ASSIGN(Window);
};

}
