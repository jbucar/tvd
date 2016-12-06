#pragma once

#include "base/compiler_specific.h"
#include "ui/gfx/display.h"
#include "ui/gfx/screen.h"

namespace aura {
class WindowTreeHost;
}

namespace tvd {

class Screen : public gfx::Screen {
public:
	Screen( aura::WindowTreeHost *host, const gfx::Rect &screen_bounds );
	~Screen() override;

protected:
	// gfx::Screen overrides:
	gfx::Point GetCursorScreenPoint() override;
	gfx::NativeWindow GetWindowUnderCursor() override;
	gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override;
	int GetNumDisplays() const override;
	std::vector<gfx::Display> GetAllDisplays() const override;
	gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const override;
	gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override;
	gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override;
	gfx::Display GetPrimaryDisplay() const override;
	void AddObserver(gfx::DisplayObserver* observer) override;
	void RemoveObserver(gfx::DisplayObserver* observer) override;

private:
	aura::WindowTreeHost *_host;
	gfx::Display _display;

	DISALLOW_COPY_AND_ASSIGN(Screen);
};

}  // namespace tvd
