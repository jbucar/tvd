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

#include "window.h"
#include "videooverlay.h"
#include "system.h"
#include "dispatcher.h"
#include "../../surface.h"
#include "../../canvas.h"
#include "../../point.h"
#include "../../inputmanager.h"
#include "generated/config.h"
#if CANVAS_2D_USE_GL
#include "../gl/surface.h"
#include "../gl/canvas.h"
#include <GL/glew.h>
#include <GL/gl.h>
#endif
#if CANVAS_2D_USE_CAIRO
#include "../cairo/surface.h"
#include "../cairo/canvas.h"
#include <cairo/cairo.h>
#include <cairo/cairo-win32.h>
#endif
#include <util/assert.h>
#include <util/mcr.h>
#include <util/log.h>

#define SHOW_LAST_ERROR(str) { \
		LPVOID lpMsgBuf; \
		DWORD dw = GetLastError(); \
		FormatMessage( \
			FORMAT_MESSAGE_ALLOCATE_BUFFER |  \
			FORMAT_MESSAGE_FROM_SYSTEM | \
			FORMAT_MESSAGE_IGNORE_INSERTS, \
			NULL, \
			dw, \
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
			(LPTSTR) &lpMsgBuf, \
			0, NULL ); \
		LERROR( "win32", str, dw, lpMsgBuf ); \
		LocalFree(lpMsgBuf); \
	}

namespace canvas {
namespace win32 {

util::key::type getKeyCode( util::DWORD wParam );

namespace impl {

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

}

Window::Window( Dispatcher *disp )
	: _disp(disp)
{
	_hwnd = NULL;
	_dc = NULL;

	_surface = NULL;

#if CANVAS_2D_USE_CAIRO
	_hdcMemory = NULL;
	_hBitmap = NULL;

	_hdcMemoryFull = NULL;
	_hBitmapFull = NULL;
#endif

	_canvas = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	LDEBUG( "win32", "Initialize window" );

	WNDCLASSEX wclass;
	memset(&wclass, 0, sizeof(WNDCLASSEX));
	wclass.cbSize        = sizeof(WNDCLASSEX);
	wclass.lpfnWndProc   = impl::MainWndProc;
	wclass.style         = CS_VREDRAW | CS_HREDRAW;
	wclass.hCursor       = NULL;
	wclass.lpszClassName = "tvd";
	wclass.hInstance     = NULL;
	wclass.hIcon         = (HICON)::LoadImage( ::GetModuleHandle(NULL), icon().c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE );
	wclass.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	wclass.lpszMenuName  = NULL;
	wclass.cbClsExtra    = 0;
	wclass.cbWndExtra    = 0;

	if (!::RegisterClassEx( &wclass )) {
		LERROR( "win32", "Cannot register class: name=%s", wclass.lpszClassName );
		return false;
	}

	// Create the window
	_hwnd = ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		wclass.lpszClassName,
		title().c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		300, 300, 10, 10,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if (!_hwnd) {
		LERROR( "win32", "Can't create main window" );
		return false;
	}

	// Set this as user data
	DTV_ASSERT(_disp);
	_disp->setupWindow( this );

	//	Setup size
	initSize();

#if CANVAS_2D_USE_GL
	// Create a new PIXELFORMATDESCRIPTOR (PFD)
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable opengl support and drawing to a window
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

	//	Check if our PFD is valid and get a pixel format back
	int nPixelFormat = ChoosePixelFormat(_dc, &pfd);
	if (!nPixelFormat) {
		LERROR( "win32", "Can't find a suitable pixel format" );
		return false;
	}

	//	Try and set the pixel format based on our PFD
	if (!SetPixelFormat( _dc, nPixelFormat, &pfd )) {
		LERROR( "win32", "Can't set the pixel format selected." );
		return false;
	}

	//	Create context
	_rc = wglCreateContext(_dc);
	if (!_rc) {
		LERROR( "win32", "Can't create a GL rendering context." );
		return false;
	}
#endif

	return true;
}

void Window::fin() {
	LDEBUG( "win32", "Finalize window" );

#if CANVAS_2D_USE_GL
	wglDeleteContext( _rc );
	_rc = NULL;
#endif
	DestroyWindow( _hwnd );
	_hwnd = NULL;
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	LDEBUG( "win32", "Init layer" );

#if CANVAS_2D_USE_GL
	// Make current context
	if (!wglMakeCurrent(_dc, _rc)) {
		LERROR( "win32", "Can't activate the GL rendering context." );
		return false;
	}

	//	Setup gl
	if (!canvas::gl::initGL()) {
		return false;
	}

	{	//	Create layer
		const Size &size = canvas->size();
		_surface = new canvas::gl::Surface( canvas, Rect(0,0,size.w,size.h) );
		if (!_surface) {
			LERROR("win32", "Cannot create canvas surface");
			return false;
		}
	}
#endif

#if CANVAS_2D_USE_CAIRO
	const Size &size = canvas->size();
	void *ptrData = NULL;

	{	//	Create and initialize canvas video overlay
		_canvas = (VideoOverlay *)createVideoOverlay( 100 );
		_canvas->setLayered( true );
		Rect rect(0,0,size.w,size.h);
		if (!_canvas->initialize( rect )) {
			LERROR("win32", "Cannot initialize canvas layer");
			return false;
		}

		// Get A Device Context
		_dc = GetDC(_canvas->handle());
		if (!_dc) {
			LERROR( "win32", "Can't create a device context" );
			return false;
		}

		if (!createMemory( size, _hdcMemory, _hBitmap, &ptrData )) {
			LERROR("win32", "Cannot create DIB memory for canvas layer");
			return false;
		}
	}

	{	//	Create surface layer
		int stride = cairo_format_stride_for_width( CAIRO_FORMAT_ARGB32, size.w );

		//	Setup data
		ImageData img;
		memset(&img,0,sizeof(ImageData));
		img.size = canvas->size();
		img.length = stride*size.h;
		img.data = (BYTE *)ptrData;
		img.bitsPerPixel = 32;
		img.bytesPerPixel = 4;
		img.stride = stride;
		img.dataOffset = 0;

		//	Create surface for layer
		_surface = canvas->createSurface( &img );
		if (!_surface) {
			LERROR("win32", "Cannot create canvas surface");
			return false;
		}
	}
#endif

	return true;
}

void Window::finLayer( canvas::Canvas * /*canvas*/ ) {
	LDEBUG( "win32", "Fin layer" );

#if CANVAS_2D_USE_GL
	wglMakeCurrent( NULL, NULL );
#endif
#if CANVAS_2D_USE_CAIRO
	destroyMemory( _hdcMemory, _hBitmap );
	destroyMemory( _hdcMemoryFull, _hBitmapFull );

	::ReleaseDC( _hwnd, _dc );
	_dc = NULL;

	_canvas->finalize();
	canvas::VideoOverlay *tmp = _canvas;
	destroyVideoOverlay(tmp);
	_canvas=NULL;
#endif

	DEL(_surface);
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Window::renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	const Rect &scaled = targetWindow();
	LDEBUG( "win32", "Render layer: scaled=(%d,%d,%d,%d)", scaled.x, scaled.y, scaled.w, scaled.h );

	DTV_ASSERT( surface == _surface );
	UNUSED( surface );

#if CANVAS_2D_USE_GL
	//	Make current context
	if (!wglMakeCurrent(_dc, _rc)) {
		LERROR( "win32", "Can't activate the GL rendering context." );
		return;
	}

	//	Render
	const Size &s = size();
	_surface->renderToFB( 0, scaled, s, true );

	//	Swap
	if (::SwapBuffers(_dc) == FALSE) {
		LERROR("win32", "Unable to swap buffers.");
	}
#endif

#if CANVAS_2D_USE_CAIRO
	HDC tmp;
	POINT dstPoint;
	SIZE dstSize;

	if (isFullScreen()) {
		if (!_hdcMemoryFull) {
			void *ptrData = NULL;
			if (!createMemory( size(), _hdcMemoryFull, _hBitmapFull, &ptrData )) {
				LERROR("win32", "Cannot create DIB memory for full screen layer");
				return;
			}
		}

		//	Blit to full screen memory
		const Size &cSize = getCanvasSize();
		BOOL bRet = ::StretchBlt( _hdcMemoryFull, scaled.x, scaled.y, scaled.w, scaled.h, _hdcMemory, 0, 0, cSize.w, cSize.h, SRCCOPY );
		if (!bRet) {
			LERROR("win32", "Cannot create DIB for layer window");
			return;
		}

		dstPoint.x = 0;
		dstPoint.y = 0;
		dstSize.cx = size().w;
		dstSize.cy = size().h;
		tmp = _hdcMemoryFull;
	}
	else {
		dstPoint.x = scaled.x;
		dstPoint.y = scaled.y;
		dstSize.cx = scaled.w;
		dstSize.cy = scaled.h;
		tmp = _hdcMemory;
	}

	BLENDFUNCTION blend = {0};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	LDEBUG( "win32", "Render layer3: target=(%d,%d,%d,%d)",	dstPoint.x, dstPoint.y, dstSize.cx, dstSize.cy );

	POINT ptSrc = { 0, 0 };
	BOOL bRet = ::UpdateLayeredWindow( _canvas->handle(), _dc, &dstPoint, &dstSize, tmp, &ptSrc, 0, &blend, ULW_ALPHA );
	if (!bRet) {
		SHOW_LAST_ERROR( "[Render] Warning, UpdateLayeredWindows error %d: %s\n" );
	}
#endif
}

bool Window::createMemory( const Size &s, HDC &mem, HBITMAP &bitmap, void **ptrData ) {
	//	Create memory
	mem = ::CreateCompatibleDC(_dc);

	//	Setup bitmap info
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = s.w;
	bmi.bmiHeader.biHeight = -s.h;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = s.w * s.h * 4;

	//	Create our DIB section and select the bitmap into the dc
	bitmap = CreateDIBSection( mem, &bmi, DIB_RGB_COLORS, ptrData, NULL, 0x0 );
	if (!bitmap || !(*ptrData)) {
		return false;
	}

	::SelectObject( mem, bitmap );

	return true;
}

void Window::destroyMemory( HDC &mem, HBITMAP &bitmap ) {
	if (bitmap) {
		::DeleteObject(bitmap);
		bitmap = NULL;
	}

	if (mem) {
		::DeleteDC(mem);
		mem = NULL;
	}
}

Size Window::screenSize() const {
	HMONITOR hmon = ::MonitorFromWindow( _hwnd,	MONITOR_DEFAULTTONEAREST );
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (!::GetMonitorInfo(hmon, &mi)) {
		LERROR("win32", "Unable to get screen rect.");
		return Size();
	}

	LDEBUG( "win32", "Screen size: (%d,%d,%d,%d)", mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom );
	Size s(mi.rcMonitor.right - mi.rcMonitor.left,mi.rcMonitor.bottom - mi.rcMonitor.top);
	return s;
}

void Window::setSizeImpl( const Size &size ) {
	UINT flags = SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW;
	int x = 0;
	int y = 0;
	int w = size.w;
	int h = size.h;

	if (!isFullScreen()) {
		Size s = screenSize();
		x = (s.w - size.w) / 2;
		y = (s.h - size.h) / 2;

		//	Calculate caption and border
		h += GetSystemMetrics(SM_CYCAPTION);
	}

	if (::SetWindowPos( _hwnd, NULL, x, y, w, h, flags ) == FALSE) {
		LERROR("win32", "Unable to set window size.");
	}
}

bool Window::supportFullScreen() const {
	return true;
}

void Window::fullScreenImpl( bool enable ) {
	if (enable) {
		//	Remove caption and sys menu
		::SetWindowLong( _hwnd, GWL_STYLE, WS_VISIBLE );
	}
	else {
		//	Add caption and sys menu
		::SetWindowLong( _hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU );
	}
}

void Window::iconifyImpl( bool enable ) {
	::ShowWindow( _hwnd, enable ? SW_MINIMIZE : SW_RESTORE );
}

bool Window::supportVideoOverlay() const {
	return true;
}

canvas::VideoOverlay *Window::createOverlayInstance( int zIndex ) const {
	return new VideoOverlay( (Window *)this, zIndex );
}

void Window::updateOverlayStack() {
	LDEBUG( "win32", "Update overlay stack" );
	const std::vector<VideoOverlayPtr> &ovs = overlays();
	HWND param = NULL;
	BOOST_FOREACH( VideoOverlayPtr ptr, ovs ) {
		VideoOverlay *ov = dynamic_cast<VideoOverlay *>(ptr);
		DTV_ASSERT(ov);
		ov->updateStack(param);
		param = ov->handle();
	}
	if (::SetWindowPos( _hwnd, param, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE ) == FALSE) {
		LWARN("win32", "Unable to apply Z order.");
	}
}

#define EV_MOUSE(ev,f,p1,p2) \
	case ev: { f( (util::DWORD)p1, Point(LOWORD(p2),HIWORD(p2)) ); break; }

#define EV_MOUSE_EX(ev,f,p1,p2) \
	case ev: { f( GET_KEYSTATE_WPARAM(p1), GET_WHEEL_DELTA_WPARAM(p1), Point(LOWORD(p2), HIWORD(p2)) ); break; }

#define EV_KEY(ev,f,p1,p2,up)	  \
	case ev: { f( (util::DWORD)p1, (util::DWORD)p2, up ); break; }

LRESULT Window::wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch (msg) {
		EV_MOUSE( WM_LBUTTONDOWN,   onLButtonDown, wParam, lParam )
		EV_MOUSE( WM_LBUTTONUP,     onLButtonUp,   wParam, lParam )
		EV_MOUSE( WM_RBUTTONDOWN,   onRButtonDown, wParam, lParam )
		EV_MOUSE( WM_RBUTTONUP,     onRButtonUp,   wParam, lParam )
		EV_MOUSE( WM_MBUTTONDOWN,   onMButtonDown, wParam, lParam )
		EV_MOUSE( WM_MBUTTONUP,     onMButtonUp,   wParam, lParam )
		EV_KEY( WM_KEYDOWN,         onKey,         wParam, lParam, false )
		EV_KEY( WM_KEYUP,           onKey,         wParam, lParam, true )
		case WM_SIZE:
			if (hWnd == _hwnd) {
				onSize( wParam, lParam );
			}
			break;
		default:
			break;
	};

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void Window::onKey( util::DWORD wParam, util::DWORD /*lParam*/, bool isUp ) {
    util::key::type key = getKeyCode( wParam);
    if (key != util::key::null) {
	    _disp->sys()->input()->dispatchKey( key, isUp );
    }
}

void Window::dispatchMouseNotify( unsigned int button, const Point &p, bool isPress ) {
	input::ButtonEvent evt;
	evt.button = button;
	evt.x = p.x;
	evt.y = p.y;
	evt.isPress = isPress;
	input::dispatchButtonEvent( _disp->sys(), &evt );
}

void Window::onMButtonDblclk( util::DWORD /*flags*/, const Point &/*p*/ ) {
}

void Window::onMButtonDown( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 2, p, true);
}

void Window::onMButtonUp( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 2, p, false);
}

void Window::onLButtonDown( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 1, p, true);
}

void Window::onLButtonUp( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 1, p, false);
}

void Window::onRButtonDown( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 3, p, true);
}

void Window::onRButtonUp( util::DWORD /*flags*/, const Point &p ) {
	dispatchMouseNotify( 3, p, false);
}

void Window::onSize( WPARAM wParam, LPARAM lParam ) {
	LDEBUG( "win32", "On size changed: wparam=%d, lparam=(%d,%d)", wParam, LOWORD(lParam), HIWORD(lParam) );
	if (isIconified() && wParam == 0) {	//	Restored
		iconify(false);
	}
}

HWND Window::handle() const {
	return _hwnd;
}

}
}

