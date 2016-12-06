#include "cursorwindowdelegate.h"
#include "ui/aura/client/aura_constants.h"
#include "ui/aura/client/cursor_client_observer.h"
#include "ui/aura/client/window_tree_client.h"
#include "ui/aura/window.h"
#include "ui/base/cursor/cursors_aura.h"
#include "ui/base/cursor/ozone/bitmap_cursor_factory_ozone.h"
#include "ui/base/hit_test.h"
#include "ui/compositor/paint_recorder.h"

namespace tvd {


CursorWindowDelegate::CursorWindowDelegate( aura::Window *rootWin )
	: _locked(0), _cursor(ui::kCursorPointer), _rootWin(rootWin), _win(nullptr)
{
	_win = new aura::Window(this);
	_win->set_owned_by_parent(true);
	_win->SetType(ui::wm::WINDOW_TYPE_NORMAL);
	_win->SetTransparent(true);
	_win->SetName("HTMLSHELL_CURSOR");
	_win->Init(ui::LAYER_TEXTURED);
	aura::client::ParentWindowWithContext(_win, _rootWin, gfx::Rect());
	_win->layer()->SetMasksToBounds(true);
	_win->SetBounds(gfx::Rect(10,10,32,32));
	_win->set_ignore_events(true);

	aura::client::SetCursorClient(_rootWin, this);
}

CursorWindowDelegate::~CursorWindowDelegate() {
	aura::client::SetCursorClient(_rootWin, nullptr);
	_win = nullptr;
}

void CursorWindowDelegate::moveCursor( const gfx::Point &location ) {
	gfx::Rect bounds = _win->bounds();
	bounds.set_origin(location - _cursorOffset);
	_win->SetBounds(bounds);
}

void CursorWindowDelegate::SetCursor( gfx::NativeCursor cursor ) {
	if (_locked == 0 && _cursor != cursor) {
		_cursor = cursor;
		if (_win->IsVisible()) {
			gfx::Rect bounds = _win->bounds();
			bounds.set_origin(gfx::Point(0,0));
			_win->SchedulePaintInRect(bounds);
		}
	}
}

void CursorWindowDelegate::ShowCursor() {
	if (_locked == 0) {
		_win->Show();
		FOR_EACH_OBSERVER(aura::client::CursorClientObserver, _observers, OnCursorVisibilityChanged(true));
	}
}

void CursorWindowDelegate::HideCursor() {
	if (_locked == 0) {
		_win->Hide();
		FOR_EACH_OBSERVER(aura::client::CursorClientObserver, _observers, OnCursorVisibilityChanged(false));
	}
}

bool CursorWindowDelegate::IsCursorVisible() const {
	return _win->IsVisible();
}

gfx::NativeCursor CursorWindowDelegate::GetCursor() const {
	return _cursor;
}

void CursorWindowDelegate::SetCursorSet(ui::CursorSetType cursor_set) {
}

ui::CursorSetType CursorWindowDelegate::GetCursorSet() const {
	return ui::CURSOR_SET_NORMAL;
}

void CursorWindowDelegate::EnableMouseEvents() {
}

void CursorWindowDelegate::DisableMouseEvents() {
}

bool CursorWindowDelegate::IsMouseEventsEnabled() const {
	return true;
}

void CursorWindowDelegate::SetDisplay(const gfx::Display& display) {
}

void CursorWindowDelegate::LockCursor() {
	_locked++;
}

void CursorWindowDelegate::UnlockCursor() {
	if (_locked > 0) {
		_locked--;
	}
}

bool CursorWindowDelegate::IsCursorLocked() const {
	return _locked > 0;
}

void CursorWindowDelegate::AddObserver( aura::client::CursorClientObserver *observer ) {
	_observers.AddObserver(observer);
}

void CursorWindowDelegate::RemoveObserver( aura::client::CursorClientObserver *observer ) {
	_observers.RemoveObserver(observer);
}

bool CursorWindowDelegate::ShouldHideCursorOnKeyEvent(const ui::KeyEvent& event) const {
	return false;
}

// Overridden from WindowDelegate:
gfx::Size CursorWindowDelegate::GetMinimumSize() const {
	return gfx::Size();
}

gfx::Size CursorWindowDelegate::GetMaximumSize() const {
	return gfx::Size(32,32);
}

void CursorWindowDelegate::OnBoundsChanged( const gfx::Rect &old_bounds, const gfx::Rect &new_bounds ) {
}

gfx::NativeCursor CursorWindowDelegate::GetCursor( const gfx::Point &point ) {
	return _cursor;
}

int CursorWindowDelegate::GetNonClientComponent( const gfx::Point &point ) const {
	return HTCAPTION;
}

bool CursorWindowDelegate::ShouldDescendIntoChildForEventHandling( aura::Window *child, const gfx::Point &location ) {
	return true;
}

bool CursorWindowDelegate::CanFocus() {
	return false;
}

void CursorWindowDelegate::OnCaptureLost() {
}

void CursorWindowDelegate::OnPaint( const ui::PaintContext &context ) {
	if (_cursor.native_type() != ui::kCursorNull) {
		ui::PlatformCursor pCursor = ui::CursorFactoryOzone::GetInstance()->GetDefaultCursor(_cursor.native_type());
		if (pCursor) {
			scoped_refptr<ui::BitmapCursorOzone> bCursor = ui::BitmapCursorFactoryOzone::GetBitmapCursor(pCursor);
			gfx::ImageSkia img = gfx::ImageSkia::CreateFrom1xBitmap(bCursor->bitmap());

			// Relocate the cursor window with the new cursor hotspot
			gfx::Rect origBounds = _win->bounds() + _cursorOffset;
			_cursorOffset = bCursor->hotspot().OffsetFromOrigin();
			gfx::Rect newBounds = origBounds - _cursorOffset;
			newBounds.set_size(img.size());
			_win->SetBounds(newBounds);

			// Paint the new cursor
			ui::PaintRecorder recorder(context, img.size());
			recorder.canvas()->TileImageInt(img, 0, 0, img.width(), img.height());
		}
	}
}

void CursorWindowDelegate::OnDeviceScaleFactorChanged( float device_scale_factor ) {
}

void CursorWindowDelegate::OnWindowDestroying( aura::Window *window ) {
}

void CursorWindowDelegate::OnWindowDestroyed( aura::Window *window ) {
}

void CursorWindowDelegate::OnWindowTargetVisibilityChanged( bool visible ) {
}

bool CursorWindowDelegate::HasHitTestMask() const {
	return false;
}

void CursorWindowDelegate::GetHitTestMask( gfx::Path *mask ) const {
}

}
