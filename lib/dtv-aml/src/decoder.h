#pragma once

#include <string>
#include <stdint.h>

namespace aml {

namespace codec {
enum type { h264, mpeg };
}

namespace hwdec {
enum type { unknown, render, v4l };
}

struct FrameInfoS {
	int width;
	int height;
	int fps;
};
typedef struct FrameInfoS FrameInfo;

class Packet;

namespace impl {

struct Codec {
	const char *name;
	int format;
	bool (*updateHeader)( Packet *info, Packet *pkt );
	bool (*updateFrame)( Packet *pkt );
};

}

class Decoder {
public:
	Decoder();
	virtual ~Decoder();

	bool initialize( hwdec::type hw, codec::type fmt );
	void finalize();
	bool reset();
	void videoLimit( int vlimit );

	bool setHeader( const unsigned char *buf, int size );
	bool decodeFrame( const unsigned char *buf, int size );
	bool decodeFrame( const unsigned char *buf, int size, int64_t pts );
	bool canFeed();
	int picturesAvailables();
	bool getVideoInfo( FrameInfo &info );
	void showFrame( int64_t pts );
	void drawFrame( int x, int y, int w, int h, int64_t pts );
	bool setPTS( int64_t pts );

protected:
	bool write( const unsigned char *buf, int size );

private:
	int _esFD;
	int _ctrlFD;
	impl::Codec *_hwCodec;
	codec::type _fmt;
	hwdec::type _hw;
};

}

