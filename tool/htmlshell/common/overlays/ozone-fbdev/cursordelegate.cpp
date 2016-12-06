#include "cursordelegate.h"
#include "htmlshell/src/util.h"
#include "ui/gfx/geometry/rect.h"

namespace tvd {

CursorDelegate::CursorDelegate()
{
}

CursorDelegate::~CursorDelegate()
{
}

void CursorDelegate::MoveCursor( const gfx::Vector2dF &delta ) {
	doMoveCursor(_location.x()+delta.x(), _location.y()+delta.y());
}

void CursorDelegate::MoveCursorTo( gfx::AcceleratedWidget widget, const gfx::PointF &location ) {
	doMoveCursor(location.x(), location.y());
}

void CursorDelegate::MoveCursorTo( const gfx::PointF &location ) {
	doMoveCursor(location.x(), location.y());
}

gfx::PointF CursorDelegate::GetLocation() {
	return _location;
}

bool CursorDelegate::IsCursorVisible() {
	return true;
}

gfx::Rect CursorDelegate::GetCursorConfinedBounds() {
	static gfx::Rect bounds = gfx::Rect(tvd::util::getWindowSize());
	return bounds;
}

void CursorDelegate::doMoveCursor( double x, double y ) {
	static gfx::Size winSize = tvd::util::getWindowSize();
	_location.set_x(std::min(std::max(x,0.0),(double)winSize.width()));
	_location.set_y(std::min(std::max(y,0.0),(double)winSize.height()));
}

}
