#include "window.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace tvd {

Window::Window( ui::PlatformWindowDelegate *delegate, const gfx::Rect &bounds )
	: _delegate(delegate), _bounds(bounds)
{
	ui::PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
}

Window::~Window() {
	ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
}

gfx::Rect Window::GetBounds() {
	return _bounds;
}

void Window::SetBounds( const gfx::Rect &bounds ) {
	_bounds = bounds;
	_delegate->OnBoundsChanged(bounds);
}

bool Window::CanDispatchEvent( const ui::PlatformEvent &event ) {
	return true;
}

uint32_t Window::DispatchEvent( const ui::PlatformEvent &event ) {
	ui::Event *ev = static_cast<ui::Event*>(event);
	_delegate->DispatchEvent(ev);
	return ui::POST_DISPATCH_STOP_PROPAGATION;
}

ui::PlatformImeController *Window::GetPlatformImeController() {
	return nullptr;
}

}
