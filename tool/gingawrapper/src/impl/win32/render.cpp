#include "render.h"
#include <boost/lexical_cast.hpp>

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
		printf( str,  dw, lpMsgBuf ); \
		LocalFree(lpMsgBuf); \
	}

namespace lifia {

canvas::remote::Server *createRender( Wrapper *wrapper, util::task::Dispatcher *disp ) {
	return new Render( wrapper, disp );
}

Render::Render( Wrapper *wrapper, util::task::Dispatcher *disp )
: _wrapper(wrapper), _disp(disp)
{
}

Render::~Render()
{
}

void Render::enable( bool enableLayer ) {
	_enable = enableLayer;
	if (!enableLayer) {
		std::vector<canvas::Rect> dummy;
		render( dummy, NULL, 0 );
	}
	_wrapper->showLayer( enableLayer, _wrapper->user_ptr );
}

bool Render::init( int /*zIndex*/ ) {
	printf( "[Render] Init\n" );

	_enable = false;
	_hdcScreen = ::GetDC(_wrapper->layer);
	_hdcMemory = ::CreateCompatibleDC(_hdcScreen);

    //	Setup bitmap info
	BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = _wrapper->videoWidth;
	bmi.bmiHeader.biHeight = -_wrapper->videoHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = _wrapper->videoWidth * _wrapper->videoHeight * 4;

    //	Create our DIB section and select the bitmap into the dc
	_hBitmap = CreateDIBSection( _hdcMemory, &bmi, DIB_RGB_COLORS, &_bits, NULL, 0x0);
    SelectObject(_hdcMemory, _hBitmap );
	return true;
}

void Render::fin() {
	printf( "[Render] Fin\n" );
	::DeleteObject(_hBitmap);
	_hBitmap = NULL;
	::DeleteDC(_hdcMemory);
	_hdcMemory = NULL;
	::ReleaseDC(NULL, _hdcScreen);
	_hdcScreen = NULL;
}

void Render::render( const std::vector<canvas::Rect> &dirtyRegions, void *mem, size_t memSize ) {
	//	Copy memory from shared to DIB
	if (_enable) {
		memcpy( _bits, mem, memSize );
	}
	else {
		memset( _bits, 0, memSize );
	}

	BLENDFUNCTION blend = {0};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;
	SIZE sizeWnd = {_wrapper->videoWidth, _wrapper->videoHeight};
	POINT ptSrc = {0, 0};
	BOOL bRet = ::UpdateLayeredWindow( _wrapper->layer, _hdcScreen, NULL, &sizeWnd, _hdcMemory, &ptSrc, 0, &blend, ULW_ALPHA );
	if (!bRet) {
		SHOW_LAST_ERROR( "[Render] Warning, UpdateLayeredWindows error %d: %s\n" );
	}
}

util::task::Dispatcher *Render::disp() const {
	return _disp;
}

}
