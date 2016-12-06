#include "video_proxy.h"

namespace tvd {
namespace video {

void init() {
}

void fin() {
}

void setFullScreen() {
}

void setAxis( int /*x*/, int /*y*/, int /*w*/, int /*h*/ ) {
}

gfx::Size getVideoResolution() {
	return gfx::Size(1280,720);
}

gfx::Rect computeVideoBounds( cc::Layer * /*layer*/ ) {
	return gfx::Rect(0,0,1280,720);
}

void showFrame( unsigned long /*pts*/ ) {
}

}
}
