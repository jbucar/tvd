#include "screen.h"
#include "ui/aura/env.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"

namespace tvd {

Screen::Screen( aura::WindowTreeHost *host, const gfx::Rect &screen_bounds )
    : _host(host)
{
	_display.set_id(2000);
	_display.SetScaleAndBounds(1.0f, screen_bounds);
}

Screen::~Screen()
{
}

gfx::Point Screen::GetCursorScreenPoint() {
	return aura::Env::GetInstance()->last_mouse_location();
}

gfx::NativeWindow Screen::GetWindowUnderCursor() {
	return GetWindowAtScreenPoint(GetCursorScreenPoint());
}

gfx::NativeWindow Screen::GetWindowAtScreenPoint(const gfx::Point& point) {
	return _host->window()->GetTopWindowContainingPoint(point);
}

int Screen::GetNumDisplays() const {
	return 1;
}

std::vector<gfx::Display> Screen::GetAllDisplays() const {
	return std::vector<gfx::Display>(1, _display);
}

gfx::Display Screen::GetDisplayNearestWindow( gfx::NativeWindow window ) const {
	return _display;
}

gfx::Display Screen::GetDisplayNearestPoint( const gfx::Point &point ) const {
	return _display;
}

gfx::Display Screen::GetDisplayMatching( const gfx::Rect &match_rect ) const {
	return _display;
}

gfx::Display Screen::GetPrimaryDisplay() const {
	return _display;
}

void Screen::AddObserver( gfx::DisplayObserver *observer ) {
}

void Screen::RemoveObserver( gfx::DisplayObserver *observer ) {
}

}  // namespace tvd
