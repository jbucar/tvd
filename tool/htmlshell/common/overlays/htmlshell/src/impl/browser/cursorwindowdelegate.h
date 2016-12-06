#pragma once

#include "base/observer_list.h"
#include "ui/aura/client/cursor_client.h"
#include "ui/aura/window_delegate.h"
#include "ui/gfx/geometry/vector2d.h"

namespace tvd {

class CursorWindowDelegate : public aura::WindowDelegate,
                             public aura::client::CursorClient {
public:
	explicit CursorWindowDelegate( aura::Window *rootWin );
	~CursorWindowDelegate() override;

	void moveCursor( const gfx::Point &location );

	// Overridden from aura::client::CursorClient:
	void SetCursor( gfx::NativeCursor cursor ) override;
	void ShowCursor() override;
	void HideCursor() override;
	bool IsCursorVisible() const override;
	gfx::NativeCursor GetCursor() const override;
	void SetCursorSet(ui::CursorSetType cursor_set) override;
	ui::CursorSetType GetCursorSet() const override;
	void EnableMouseEvents() override;
	void DisableMouseEvents() override;
	bool IsMouseEventsEnabled() const override;
	void SetDisplay(const gfx::Display& display) override;
	void LockCursor() override;
	void UnlockCursor() override;
	bool IsCursorLocked() const override;
	void AddObserver( aura::client::CursorClientObserver *observer ) override;
	void RemoveObserver( aura::client::CursorClientObserver *observer ) override;
	bool ShouldHideCursorOnKeyEvent(const ui::KeyEvent& event) const override;

	// Overridden from aura::WindowDelegate:
	gfx::Size GetMinimumSize() const override;
	gfx::Size GetMaximumSize() const override;
	void OnBoundsChanged( const gfx::Rect &old_bounds, const gfx::Rect &new_bounds ) override;
	gfx::NativeCursor GetCursor( const gfx::Point &point ) override;
	int GetNonClientComponent( const gfx::Point &point ) const override;
	bool ShouldDescendIntoChildForEventHandling( aura::Window *child, const gfx::Point &location ) override;
	bool CanFocus() override;
	void OnCaptureLost() override;
	void OnPaint( const ui::PaintContext &context ) override;
	void OnDeviceScaleFactorChanged( float device_scale_factor ) override;
	void OnWindowDestroying( aura::Window *window ) override;
	void OnWindowDestroyed( aura::Window *window ) override;
	void OnWindowTargetVisibilityChanged( bool visible ) override;
	bool HasHitTestMask() const override;
	void GetHitTestMask( gfx::Path *mask ) const override;

private:
	int _locked;
	gfx::NativeCursor _cursor;
	aura::Window *_rootWin;
	aura::Window *_win;
	gfx::Vector2d _cursorOffset;
	base::ObserverList<aura::client::CursorClientObserver> _observers;

	DISALLOW_COPY_AND_ASSIGN(CursorWindowDelegate);
};

}
