#include "decoder.h"
#include "aml.h"
#include "decoder/packet.h"
#include "decoder/h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stropts.h>
#include <string.h>
#include <stropts.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <assert.h>

extern "C" {
#include "amports/amstream.h"
#include "amports/vformat.h"
}


#define ES_VIDEO_DEVICE_FILE "/dev/amstream_vbuf"
#define CTRL_DEVICE_FILE     "/dev/amvideo"
#define TRICKMODE_NONE       0x00
#define PTS_FREQ             90000
#define AV_SYNC_THRESH       PTS_FREQ*30
#define EXTERNAL_PTS         (1)
#define SYNC_OUTSIDE         (2)

namespace aml {

namespace impl {

typedef struct {
    unsigned int    format;  ///< video format, such as H264, MPEG2...
    unsigned int    width;   ///< video source width
    unsigned int    height;  ///< video source height
    unsigned int    rate;    ///< video source frame duration
    unsigned int    extra;   ///< extra data information of video stream
    unsigned int    status;  ///< status of video stream
    unsigned int    ratio;   ///< aspect ratio of video source
    void *          param;   ///< other parameters for video decoder
    unsigned long long    ratio64;   ///< aspect ratio of video source
} dec_sysinfo_t;

typedef struct vframe_states {
	int vf_pool_size;
	int buf_free_num;
	int buf_recycle_num;
    int buf_avail_num;
} vframe_states_t;

namespace dummy {

static bool updateHeader( Packet * /*info*/, Packet * /*pkt*/ ) {
	return true;
}

static bool updateFrame( Packet * /*pkt*/ ) {
	return true;
}

}	//	namespace dummy

static Codec codecs[] = {
	{ "h264", VFORMAT_H264, &h264::updateHeader, &h264::updateFrame },
	{ "mpeg", VFORMAT_MPEG12, &dummy::updateHeader, &dummy::updateFrame },
	{ NULL, -1, NULL, NULL }
};

}	//	namespace impl


Decoder::Decoder()
{
	_esFD = -1;
	_ctrlFD = -1;
	_hwCodec = NULL;
	_fmt = codec::h264;
	_hw = hwdec::unknown;
}

Decoder::~Decoder()
{
}

bool Decoder::initialize( hwdec::type hw, codec::type fmt ) {
	assert(_esFD == -1);

	//	Setup codec
	if (!impl::codecs[fmt].name) {
		printf( "[aml] Decoder: format not available: hw=%d, fmt=%d\n", hw, fmt );
		return false;
	}

	//	Setup
	_hwCodec = &impl::codecs[fmt];
	_fmt = fmt;
	_hw = hw;
	printf( "[aml] Hw decoder initialization: hw=%d, fmt=%d, codec=%s\n", hw, fmt, _hwCodec->name );

	if (hw == hwdec::render) {
		//	On video plane
		sysfs::set("/sys/class/vfm/map", "rm all");
		sysfs::set("/sys/class/ppmgr/vtarget", "0");
		sysfs::set("/sys/class/vfm/map", "add default_osd osd amvideo");
		sysfs::set("/sys/class/vfm/map", "add default decoder ppmgr amvideo");
		sysfs::set("/sys/class/video/disable_video", 0);
	}
	else if (hw == hwdec::v4l) {
		//	Get frame via v4l
		sysfs::set("/sys/class/vfm/map", "rm all");
		sysfs::set("/sys/class/ppmgr/vtarget", "1");
		sysfs::set("/sys/module/amlvideodri/parameters/freerun_mode", 1);
		sysfs::set("/sys/class/vfm/map", "add default decoder ppmgr amlvideo");
		sysfs::set("/sys/class/video/disable_video", 1);
	}

	{	//	Open es video device
		bool noblock = true;
		int flags = O_WRONLY | (noblock ? O_NONBLOCK : 0);
		_esFD = open(ES_VIDEO_DEVICE_FILE, flags);
		if (_esFD < 0) {
			printf("Cannot open ES video controller: errno=%d error=%s\n", errno, strerror(errno));
			finalize();
			return false;
		}
	}

	//	Set video format
	if (ioctl(_esFD, AMSTREAM_IOC_VFORMAT, _hwCodec->format) < 0) {
		printf("Set video format failed\n");
		finalize();
		return false;
	}

	//	Set video ID
	if (ioctl(_esFD, AMSTREAM_IOC_VID, 1) < 0) {
		printf("Set video ID failed\n");
		finalize();
		return false;
	}

	{	//	Set video system information
		impl::dec_sysinfo_t sysinfo;
		memset(&sysinfo,0,sizeof(impl::dec_sysinfo_t));
		sysinfo.param = (void*)(EXTERNAL_PTS | SYNC_OUTSIDE);
		if (ioctl(_esFD, AMSTREAM_IOC_SYSINFO, (unsigned long)&sysinfo) < 0) {
			printf("Set video info failed\n");
			finalize();
			return false;
		}
	}

	//	Change video buffer size
	// r = codec_h_control(pcodec->handle, AMSTREAM_IOC_VB_SIZE, pcodec->vbuf_size);

	//	Init stream.
	if (ioctl(_esFD, AMSTREAM_IOC_PORT_INIT, 0) < 0) {
		printf("Port init failed\n");
		finalize();
		return false;
	}

	if (_hw == hwdec::render) {	//	TODO: Esto es necesario? si no se usa sync con el video ....
		//	Open ctrl stream
		_ctrlFD = open( CTRL_DEVICE_FILE, O_RDWR);
		if (_ctrlFD < 0) {
			printf("Cannot open cntl file\n");
			finalize();
			return false;
		}

		//	Set the AV sync threshold which defines the max time difference between A/V
		unsigned long tmp=AV_SYNC_THRESH;
		if (ioctl(_ctrlFD, AMSTREAM_IOC_AVTHRESH, (unsigned long)tmp) < 0) {
			printf("AV thresh failed\n");
			finalize();
			return false;
		}

		//	Set sync threshold control which defines the starting system time (hold video or not) when playing
		unsigned long syncthresh = 0;
		if (ioctl(_ctrlFD, AMSTREAM_IOC_SYNCTHRESH, (unsigned long)syncthresh) < 0) {
			printf("AV sync thresh failed\n");
			finalize();
			return false;
		}
	}

	// disable tsync, we are playing video disconnected from audio.
	video::setSyncMode( false );

	//	Enable video layer
	video::enable(true);

	return true;
}

void Decoder::finalize() {
	if (_ctrlFD >= 0) {
		close(_ctrlFD);
		_ctrlFD = -1;
	}

	if (_esFD >= 0) {
		close(_esFD);
		_esFD = -1;
	}
	_hwCodec = NULL;
}

bool Decoder::reset() {
	hwdec::type hw = _hw;
	codec::type fmt = _fmt;

	finalize();
	return initialize( hw, fmt );
}

void Decoder::videoLimit( int vlimit ) {
	sysfs::set("/sys/module/amlvideodri/parameters/vid_limit", vlimit );
}

bool Decoder::setHeader( const unsigned char *buf, int size ) {
	Packet hdr, pkt;
	hdr.assign(buf,size,size,false);
	if ((*_hwCodec->updateHeader)( &hdr, &pkt )) {
		return write( pkt.data, pkt.size );
	}
	printf( "[aml] Decoder: update header failed\n" );
	return false;
}

bool Decoder::decodeFrame( const unsigned char *buf, int size, int64_t pts ) {
	if (decodeFrame( buf, size )) {
		return setPTS( pts );
	}
	return false;
}

bool Decoder::decodeFrame( const unsigned char *buf, int size ) {
	const unsigned char header[] = { 0x00, 0x00, 0x00, 0x01 };
	if (!write( header, 4 )) {
		return false;
	}

	//	Send to decoder
	if (!write( buf, size )) {
		return false;
	}

	return true;
}

bool Decoder::setPTS( int64_t pts ) {
	//	Set video PTS
	if (pts) {
		unsigned long param = (unsigned long)(pts);	//	TODO: Check overflow
		if (ioctl(_esFD, AMSTREAM_IOC_TSTAMP, param) < 0) {
			printf( "[aml] Decoder: cannot set pts\n" );
			return false;
		}
	}

	return true;
}

int Decoder::picturesAvailables() {
	if (_hw == hwdec::render) {
		impl::vframe_states_t states;
		if (ioctl( _ctrlFD, AMSTREAM_IOC_VF_STATUS, (unsigned long)&states) < 0) {
			printf( "[aml] Decoder: cannot get vf states\n" );
			return -1;
		}
		return states.buf_avail_num;
	}
	else {
		return -1;
	}
}

bool Decoder::getVideoInfo( FrameInfo &info ) {
	//	Get status
    struct am_io_param am_io;
	memset(&am_io,0,sizeof(am_io));
    if (ioctl(_esFD, AMSTREAM_IOC_VDECSTAT, (unsigned long)&am_io) < 0) {
		printf( "[aml] Decoder: cannot get vdec stata\n" );
		return false;
	}

	if (am_io.vstatus.status & 0x3F) {
		//	Update frame info
		info.width = am_io.vstatus.width;
		info.height = am_io.vstatus.height;
		info.fps = am_io.vstatus.fps;

		// printf( "[aml::Decoder] VDEC status: size=(%u,%u), fps=%u, erros=%u, status=%08x\n",
		// 	vdec.width, vdec.height, vdec.fps, vdec.error_count, vdec.status );

		return true;
	}

	return false;
}

bool Decoder::canFeed() {
	//	Get status
    struct am_io_param am_io;
    if (ioctl(_esFD, AMSTREAM_IOC_VB_STATUS, (unsigned long)&am_io) < 0) {
		printf( "[aml] Decoder: cannot get video buffer stata\n" );
		return false;
	}

	    // printf( "[aml] Decoder: buffer status; size=%d, data=%d, free=%d\n",
		//     am_io.status.size, am_io.status.data_len, am_io.status.free_len );

	return am_io.status.data_len < 2*(am_io.status.size/3) ? true : false;
}

void Decoder::drawFrame( int x, int y, int w, int h, int64_t pts ) {	//	In canvas coordinates
	printf( "[aml] Decoder: draw frame; rect=(%d,%d,%d,%d), PTS=%lld\n", x, y, w, h, pts );

	video::setAxis( x, y, x+w, y+h );

	if (ioctl(_esFD, AMSTREAM_IOC_SET_PCRSCR, pts) < 0) {
		printf( "[aml] Decoder: cannot set pcr pts\n" );
		return;
	}
}

void Decoder::showFrame( int64_t pts ) {	//	In canvas coordinates
	//printf( "[aml] Decoder: show frame: PTS=%lld\n", pts );

	if (ioctl(_esFD, AMSTREAM_IOC_SET_PCRSCR, pts) < 0) {
		printf( "[aml] Decoder: cannot set pcr pts\n" );
		return;
	}
}

bool Decoder::write( const unsigned char *buf, int size ) {
	if (size > 0) {
		int len=0;
		while (len != size) {
			int write_bytes = ::write( _esFD, (void *)(buf+len), size-len );
			if (write_bytes < 0 || write_bytes > size) {
				printf( "[aml] Decoder: write codec data failed; write_bytes=%d, errno=%d, bytes=%d\n",
					write_bytes, errno, size );
				return false;
			}
			len += write_bytes;
		}
	}
	return true;
}

}

