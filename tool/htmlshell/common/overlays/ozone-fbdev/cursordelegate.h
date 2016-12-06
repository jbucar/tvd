#pragma once

#include "ui/events/ozone/evdev/cursor_delegate_evdev.h"

namespace tvd {

class CursorDelegate : public ui::CursorDelegateEvdev {
public:
	CursorDelegate();
	~CursorDelegate() override;

	// ui::CursorDelegateEvdev implementation:
	gfx::PointF GetLocation() override;
	bool IsCursorVisible() override;
	gfx::Rect GetCursorConfinedBounds() override;
	void MoveCursor( const gfx::Vector2dF &delta ) override;
	void MoveCursorTo( gfx::AcceleratedWidget widget, const gfx::PointF &location ) override;
	void MoveCursorTo( const gfx::PointF &location ) override;

protected:
	void doMoveCursor( double x, double y );

private:
	gfx::PointF _location;
};

}
