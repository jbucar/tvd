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

#pragma once

#include "../../window.h"
#include <windows.h>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace canvas {
namespace win32 {

class Dispatcher;
class VideoOverlay;

class Window : public canvas::Window {
public:
	explicit Window( Dispatcher *disp );
	virtual ~Window();

	//	Layer methods
	virtual canvas::Surface *lockLayer();
	virtual bool supportFullScreen() const;
	virtual bool supportVideoOverlay() const;

	//	Implementation
	HWND handle() const;
	LRESULT wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

protected:
	//	Initialization
	virtual bool init();
	virtual void fin();

	//	Aux canvas layer
	virtual bool initLayer( canvas::Canvas *canvas );
	virtual void finLayer( canvas::Canvas *canvas );
	virtual void renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> &dirtyRegions );
	bool createMemory( const Size &s, HDC &mem, HBITMAP &bitmap, void **ptrData );
	void destroyMemory( HDC &mem, HBITMAP &bitmap );

	virtual Size screenSize() const;
	virtual void setSizeImpl( const Size &size );
	virtual void fullScreenImpl( bool enable );
	virtual void iconifyImpl( bool enable );

	//	Overlay methods
	virtual canvas::VideoOverlay *createOverlayInstance( int zIndex ) const;
	virtual void updateOverlayStack();

	//	Input events
	void dispatchMouseNotify( unsigned int button, const Point &p, bool isPress );
	void onMButtonDblclk( util::DWORD flags, const Point &p );
	void onMButtonDown( util::DWORD flags, const Point &p );
	void onMButtonUp( util::DWORD flags, const Point &p );
	void onLButtonDown( util::DWORD flags, const Point &p );
	void onLButtonUp( util::DWORD flags, const Point &p );
	void onRButtonDown( util::DWORD flags, const Point &p );
	void onRButtonUp( util::DWORD flags, const Point &p );
	void onKey( util::DWORD wParam, util::DWORD lParam, bool isUp );

	//	Window events
	void onSize( WPARAM wParam, LPARAM lparam );

private:
	Dispatcher *_disp;
	Surface *_surface;

	HWND _hwnd;
	HDC _dc;

	HDC _hdcMemory;
	HBITMAP _hBitmap;

	HDC _hdcMemoryFull;
	HBITMAP _hBitmapFull;

	VideoOverlay *_canvas;
};

}
}

