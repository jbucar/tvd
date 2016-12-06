#pragma once

#include <vector>
#include <string>

namespace aml {
namespace v4l {

typedef struct vframebuf {
	char *vbuf;
	int index;
	int offset;
	int length;
	long long int pts;
	int duration;
} vframebuf_t;

class Device {
public:
	Device();
	virtual ~Device();

	bool init( int width, int height, int fmt );
	void fin();

	vframebuf_t *dequeue();
	bool queue( vframebuf_t *vf );

	int fd() const { return _fd; }

protected:
	bool mapBuffers( int cant );
	bool ioctl( int request, void *arg, const char *str );

private:
	int _fd;
	size_t _buffers;
	vframebuf_t *_vf;
};

}	//	namespace v4l
}	//	namespace aml

