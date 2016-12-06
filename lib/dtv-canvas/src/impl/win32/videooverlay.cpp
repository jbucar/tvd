/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "videooverlay.h"
#include "window.h"
#include "../../color.h"
#include "../../surface.h"
#include <util/assert.h>
#include <util/log.h>

namespace canvas {
namespace win32 {

VideoOverlay::VideoOverlay( Window *win, int zIndex )
	: canvas::VideoOverlay( zIndex ), _win(win), _hwnd(NULL), _layered(false)
{
}

VideoOverlay::~VideoOverlay()
{
}

bool VideoOverlay::create() {
	LDEBUG("win32", "Create video overlay");

	const Rect &rect = windowBounds();

	_hwnd = ::CreateWindowEx(
		_layered ? (WS_EX_LAYERED | WS_EX_TRANSPARENT) : WS_EX_TRANSPARENT,
		"tvd",
		NULL,
		WS_DISABLED | WS_CHILD,
		0, 0, 300, 300,
		_win->handle(),
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if (!_hwnd) {
		LERROR( "win32", "Can't create video overlay window" );
		return false;
	}

	return true;
}

void VideoOverlay::destroy() {
	LDEBUG("win32", "Destroy video overlay");
	::DestroyWindow( _hwnd );
	_hwnd = NULL;
}

void VideoOverlay::show() {
	LDEBUG("win32", "Show video overlay");
	::ShowWindow( _hwnd, SW_SHOW );
}

void VideoOverlay::hide() {
	LDEBUG("win32", "Hide video overlay");
	::ShowWindow( _hwnd, SW_HIDE );
}

bool VideoOverlay::moveImpl( const Point &point ) {
	LDEBUG("win32", "Move video overlay: point=(%d,%d)", point.x, point.y );
	if (::SetWindowPos( _hwnd, NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW ) == FALSE) {
		LERROR("win32", "Unable to resize window.");
		return false;
	}
	return true;
}

bool VideoOverlay::resizeImpl( const Size &size ) {
	LDEBUG("win32", "Resize video overlay: size=(%d,%d)", size.w, size.h );
	if (::SetWindowPos( _hwnd, NULL, 0, 0, size.w, size.h, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW ) == FALSE) {
		LERROR("win32", "Unable to resize window.");
		return false;
	}
	return true;
}

bool VideoOverlay::moveResizeImpl( const Rect &rect ) {
	LDEBUG("win32", "Move and resize video overlay: rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h );
	if (::SetWindowPos( _hwnd, NULL, rect.x, rect.y, rect.w, rect.h, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW ) == FALSE) {
		LERROR("win32", "Unable to resize/move window.");
		return false;
	}
	return true;
}

void VideoOverlay::updateStack( HWND hwnd ) {
	LDEBUG("win32", "Update stack: hwnd=%p", hwnd );
	if (::SetWindowPos( _hwnd, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE ) == FALSE) {
		LWARN("win32", "Unable to apply Z order.");
	}
}

bool VideoOverlay::getDescription( VideoDescription &desc ) {
	LDEBUG("win32", "Get description video overlay");
	desc = _hwnd;
	return true;
}

canvas::Window *VideoOverlay::win() const {
	return _win;
}

void VideoOverlay::setLayered( bool layered ) {
	_layered = layered;
}

HWND VideoOverlay::handle() const {
	return _hwnd;
}


}
}

