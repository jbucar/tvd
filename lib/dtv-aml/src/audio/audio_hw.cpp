#include "audio_hw.h"
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define AMAUDIO_CTRL_DEVICE          "/dev/amaudio_ctl"
#define AMAUDIO_IOC_MAGIC            'A'
#define AMAUDIO_IOC_SET_LEFT_MONO    _IOW(AMAUDIO_IOC_MAGIC, 0x0e, int)
#define AMAUDIO_IOC_SET_RIGHT_MONO   _IOW(AMAUDIO_IOC_MAGIC, 0x0f, int)
#define AMAUDIO_IOC_SET_STEREO       _IOW(AMAUDIO_IOC_MAGIC, 0x10, int)
#define AMAUDIO_IOC_SET_CHANNEL_SWAP _IOW(AMAUDIO_IOC_MAGIC, 0x11, int)

#define hw_print(format,...) fprintf(stderr,"[audio] %s " format, __FUNCTION__, ##__VA_ARGS__)

namespace aml {
namespace audio {
namespace hw {

bool ctrl( command_t cmd ) {
	int fd = open(AMAUDIO_CTRL_DEVICE, O_RDONLY);
	if (fd < 0) {
		hw_print("hw::ctrl error: cannot open device %s\n", AMAUDIO_CTRL_DEVICE);
		return false;
	}

	hw_print("hw::ctrl cmd=%d\n", cmd );
	bool result=false;
	switch (cmd) {
		case hw::CHANNELS_SWAP:
			result=ioctl(fd, AMAUDIO_IOC_SET_CHANNEL_SWAP, 0) >= 0;
			break;
		case hw::LEFT_CHANNEL_MONO:
			result=ioctl(fd, AMAUDIO_IOC_SET_LEFT_MONO, 0) >= 0;
			break;
		case hw::RIGHT_CHANNEL_MONO:
			result=ioctl(fd, AMAUDIO_IOC_SET_RIGHT_MONO, 0) >= 0;
			break;
		case hw::STEREO_MODE:
			result=ioctl(fd, AMAUDIO_IOC_SET_STEREO, 0) >= 0;
			break;
		default:
			hw_print("hw::trl error: unknown command %d\n", cmd);
			break;
	};

	close(fd);
	return result;
}

}
}
}

