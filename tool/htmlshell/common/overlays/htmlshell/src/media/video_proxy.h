#pragma once

#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
class Layer;
}

namespace tvd {
namespace video {

void init();
void fin();

void setFullScreen();
void setAxis( int x, int y, int w, int h );
gfx::Size getVideoResolution();
void showFrame( unsigned long pts );
gfx::Rect computeVideoBounds( cc::Layer *layer );

}
}
