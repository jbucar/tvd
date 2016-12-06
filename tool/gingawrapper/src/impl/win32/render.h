#pragma once

#include "../../types.h"
#include <canvas/remote/server.h>
#include <windows.h>

namespace lifia {

class Render : public canvas::remote::Server {
public:
	Render( Wrapper *wrapper, util::task::Dispatcher *disp );
	virtual ~Render();

	virtual void enable( bool enableLayer );

protected:
	virtual bool init( int zIndex );
	virtual void fin();
	virtual void render( const std::vector<canvas::Rect> &dirtyRegions, void *mem, size_t memSize );
	virtual util::task::Dispatcher *disp() const;

private:
	Wrapper *_wrapper;
	util::task::Dispatcher *_disp;
	bool _enable;
	HDC _hdcScreen;
	HDC _hdcMemory;
	HBITMAP _hBitmap;
	VOID *_bits;
};

}
