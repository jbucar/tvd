#include "v4l.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stropts.h>
#include <string.h>
#include <linux/videodev2.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


#define V4LDEVICE_NAME  "/dev/video10"
#define IOCTL(request,args)	ioctl( request, args, #request )

namespace aml {
namespace v4l {

Device::Device()
	: _fd(-1), _buffers(0), _vf(NULL)
{
}

Device::~Device()
{
}

bool Device::init( int width, int height, int fmt ) {
	_fd = open(V4LDEVICE_NAME, O_RDWR | O_NONBLOCK);
	//_fd = open(V4LDEVICE_NAME, O_RDWR);
	if (_fd < 0) {
		printf("[v4l] device opend failed!: %s(%d)\n", strerror(errno), errno);
		return false;
	}
	printf("[v4l] init\n");

	{	//	Setup format
		struct v4l2_format v4lfmt;
//		struct v4l2_cropcap cropcap;
//
//		memset(&cropcap, 0, sizeof (cropcap));
//		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//
//		if (!IOCTL(VIDIOC_CROPCAP, &cropcap)) {
//			fin();
//			return false;
//		}

		v4lfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4lfmt.fmt.pix.width  = width;
		v4lfmt.fmt.pix.height = height;
		v4lfmt.fmt.pix.pixelformat = fmt;
		if (!IOCTL(VIDIOC_S_FMT, &v4lfmt)) {
			fin();
			return false;
		}
	}

	//	Map MMAP buffers
	if (!mapBuffers(4)) {
		fin();
		return false;
	}

	{	//	Start stream
		int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (!IOCTL( VIDIOC_STREAMON, &type)) {
			return false;
		}
	}

	return true;
}

void Device::fin() {
	if (_vf) {
		{	//	Stop stream
			int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			IOCTL(VIDIOC_STREAMOFF,&type);
		}
		for (size_t i=0; i<_buffers; i++) {
			munmap(_vf[i].vbuf, _vf[i].length);
		}
		free(_vf);
		_vf = NULL;
		_buffers = 0;
	}

	if (_fd >= 0) {
		close(_fd);
		_fd = -1;
	}
}

bool Device::ioctl( int request, void *arg, const char *reqstr ) {
	int ret = ::ioctl( _fd, request, arg );
	if (ret < 0) {
		printf( "[v4l] ioctl failed: request=%s, %s(%d)\n", reqstr, strerror(errno), errno );
		return false;
	}
	return true;
}

bool Device::mapBuffers( int cant ) {
	_buffers = 0;

	//	Request buffers
	struct v4l2_requestbuffers req;
	req.count = cant;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (!IOCTL(VIDIOC_REQBUFS, &req)) {
		return false;
	}

	_vf = (vframebuf_t *)calloc(sizeof(vframebuf_t),req.count);
	if (!_vf) {
		printf("[v4l] malloc framebuf error\n" );
		return false;
	}

	//	Alloc buffers
	struct v4l2_buffer vbuf;
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	for (size_t i=0; i<req.count; i++) {
		vbuf.index = i;
		if (!IOCTL(VIDIOC_QUERYBUF, &vbuf)) {
			return false;
		}

		_vf[i].index = i;
		_vf[i].offset = vbuf.m.offset;
		_vf[i].pts = 0;
		_vf[i].duration = 0;
		_vf[i].length = vbuf.length;
		_vf[i].vbuf = (char *)::mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, vbuf.m.offset);
		if (_vf[i].vbuf == NULL || _vf[i].vbuf == MAP_FAILED) {
			printf("[v4l] mmap failed: index=%d, error=%s\n", i, strerror(errno));
			return false;
		}

		//printf("mmaped buf %d,off=%d,vbuf=%p,len=%d\n", vf[i].index,vf[i].offset,vf[i].vbuf,vf[i].length);
		_buffers++;
	}

	printf( "[v4l] mmap ok: need=%d, available=%d, buffers=%d\n", cant, req.count, _buffers );
	return true;
}

vframebuf_t *Device::dequeue() {
	struct v4l2_buffer vbuf;
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	vbuf.index = _buffers;
	if (!IOCTL(VIDIOC_DQBUF, &vbuf)) {
		return NULL;
	}

	vframebuf_t *vf = &_vf[vbuf.index];

	//	Check index
	if (vbuf.index >= _buffers) {
		printf("[v4l] Invalid index: bufIndex=%d, _buffers=%d\n", vbuf.index, _buffers );
		queue( vf); //	HACK!!!
		return NULL;
	}

	printf("[v4l] dequeue: index=%d, pts=(%ld,%ld)\n", vbuf.index, vbuf.timestamp.tv_sec, vbuf.timestamp.tv_usec );

	vf->pts = /*vbuf.timestamp.tv_sec * 1000000 +*/ (int64_t)vbuf.timestamp.tv_usec * 100ll / 9ll;
	return vf;
}

bool Device::queue( vframebuf_t *vf ) {
	struct v4l2_buffer vbuf;
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	vbuf.index = vf->index;
	return IOCTL(VIDIOC_QBUF, &vbuf);
}

}	//	namespace v4l
}	//	namespace aml

