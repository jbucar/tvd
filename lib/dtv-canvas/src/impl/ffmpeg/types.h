#pragma once

struct AVInputFormat;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVPacket;
struct AVFrame;
struct AVCodec;
struct AudioParams;
struct SwsContext;

namespace canvas {
namespace ffmpeg {

namespace sync {
	enum type { audio, video, external };
}

namespace stream {
	enum type { audio, video, subtitle, last };
}

class Stream;

}
}

