#include "video_proxy.h"
#include "../util.h"
#include "cc/layers/layer.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/box_f.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

namespace tvd {
namespace video {

namespace impl {

	// File paths:
	static const char kVideoAxis[] = "/sys/class/video/axis";
	static const char kFrameWidth[] = "/sys/class/video/frame_width";
	static const char kFrameHeight[] = "/sys/class/video/frame_height";
	static const char kShowFrame[] = "/sys/class/tsync/pts_pcrscr";

	// File descriptors:
	static int videoAxis = -1;
	static int frame_w = -1;
	static int frame_h = -1;
	static int showFrame = -1;

	static int videoLayerWidth = -1;
	static int videoLayerHeight = -1;
	static int currentX = -1;
	static int currentY = -1;
	static int currentW = -1;
	static int currentH = -1;

}

static void openFile( const char *fPath, int &fd, bool rw=false ) {
	if (fd < 0) {
		fd = rw ? open(fPath, O_CREAT | O_RDWR | O_TRUNC, 0644) : open(fPath, O_RDONLY);
	}
}

static void closeFile( int &fd ) {
	if (fd >= 0) {
		close(fd);
		fd = -1;
	}
}

static void setAxisImpl( int x, int y, int w, int h ) {
	if (x != impl::currentX ||
		y != impl::currentY ||
		w != impl::currentW ||
		h != impl::currentH)
	{
		char video_axis[256] = {0};
		sprintf(video_axis, "%d %d %d %d", x, y, w, h );
		write( impl::videoAxis, video_axis, strlen(video_axis) );
		printf( "[aml] Set video axis: %d,%d,%d,%d\n", x, y, w, h );

		impl::currentX = x;
		impl::currentY = y;
		impl::currentW = w;
		impl::currentH = h;
	}
}

static bool readInt( int fd, int &value ) {
	char str[100];
	int len = read(fd, str, 100);
	lseek(fd, 0, SEEK_SET);
	str[len-1] = '\0';
	return sscanf(str, "%d", &value) == 1;
}

void init() {
	// Get window size
	gfx::Size winSize = util::getWindowSize();
	impl::videoLayerWidth = winSize.width();
	impl::videoLayerHeight = winSize.height();

	// Open required files
	openFile(impl::kVideoAxis, impl::videoAxis, true);
	openFile(impl::kFrameWidth, impl::frame_w);
	openFile(impl::kFrameHeight, impl::frame_h);
	impl::showFrame = open(impl::kShowFrame, O_WRONLY);
}

void fin() {
	closeFile( impl::videoAxis );
	closeFile( impl::frame_w );
	closeFile( impl::frame_h );
	closeFile( impl::showFrame );
}

void setFullScreen() {
	setAxisImpl( 0, 0, impl::videoLayerWidth, impl::videoLayerHeight );
}

void setAxis( int x, int y, int w, int h ) {
	if (impl::videoAxis >= 0) {
		int x1 = (x < 0) ? 0 : x;
		int y1 = (y < 0) ? 0 : y;
		int w1 = x1+w;
		if (w1 > impl::videoLayerWidth) {
			w1 = impl::videoLayerWidth;
		}
		int h1 = y1+h;
		if (h1 > impl::videoLayerHeight) {
			h1 = impl::videoLayerHeight;
		}
		setAxisImpl( x1, y1, w1, h1 );
	}
}

gfx::Size getVideoResolution() {
	int w, h;
	bool res = readInt(impl::frame_w, w);
	res &= readInt(impl::frame_h, h);
	if (!res) {
		w = 1280;
		h = 720;
	}
	return gfx::Size(w,h);
}

void showFrame( unsigned long pts ) {
	if (impl::showFrame >= 0) {
		char pts_str[64];
		sprintf(pts_str, "0x%lx", pts);
		write(impl::showFrame, pts_str, strlen(pts_str));
	}
}

gfx::Rect computeVideoBounds( cc::Layer *layer ) {
	gfx::BoxF box(layer->bounds().width(), layer->bounds().height(), 0.f);
	gfx::Transform tf;

	for (; layer; layer = layer->parent()) {
		int tx = layer->transform_origin().x();
		int ty = layer->transform_origin().y();
		int tz = layer->transform_origin().z();

		gfx::PointF pos = layer->position();
		if (layer->parent()) {
			gfx::Transform tmp;
			tmp.Translate3d(tx + pos.x(), ty + pos.y(), tz);
			tmp.PreconcatTransform(layer->transform());
			tmp.Translate3d(-tx, -ty, -tz);
			tf.ConcatTransform(tmp);
			continue;
		}

		tf.TransformBox(&box);
		tf.MakeIdentity();

		box.set_origin(box.origin() + gfx::Vector3dF(-tx, -ty, -tz));
		box.set_origin(box.origin() + gfx::Vector3dF(tx + pos.x(), ty + pos.y(), tz));
	}

	if (!tf.IsIdentity()) {
		tf.TransformBox(&box);
	}

	return gfx::Rect(std::lround(box.x()), std::lround(box.y()), std::lround(box.width()), std::lround(box.height()));
}

}
}
