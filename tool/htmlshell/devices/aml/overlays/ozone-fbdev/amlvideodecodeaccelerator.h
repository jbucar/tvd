#pragma once

#include "base/logging.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/shared_memory.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/threading/non_thread_safe.h"
#include "base/threading/thread.h"
#include "content/common/content_export.h"
#include "media/base/bitstream_buffer.h"
#include "media/video/picture.h"
#include "media/video/video_decode_accelerator.h"
#include "media/filters/h264_parser.h"
#include <queue>

namespace aml {

class Decoder;

class AmlVideoDecodeAccelerator : public media::VideoDecodeAccelerator {
public:
	AmlVideoDecodeAccelerator();
	~AmlVideoDecodeAccelerator() override;

	// media::VideoDecodeAccelerator implementation.
	bool Initialize(const media::VideoDecodeAccelerator::Config& config, Client* client) override;
	void Decode(const media::BitstreamBuffer& bitstream_buffer) override;
	void AssignPictureBuffers( const std::vector<media::PictureBuffer>& buffers) override;
	void ReusePictureBuffer(int32_t picture_buffer_id) override;
	void Flush() override;
	void Reset() override;
	void Destroy() override;
	bool CanDecodeOnIOThread() override;

protected:
	void processQueue( bool sendFlush );
	void clearQueue();
	void resetQueue();
	void processItem( const media::BitstreamBuffer &buffer );
	void startDecoderThread();
	Decoder *decoder() const;

private:
	Client *_client;
	media::H264Parser _parser;
	base::MessageLoop *_loop;
	base::Thread _thread;
	base::Lock _lock;
	std::queue<media::BitstreamBuffer> _queue;
	scoped_refptr<base::SingleThreadTaskRunner> _mtTaskRunner;
	scoped_refptr<base::SingleThreadTaskRunner> _dtTaskRunner;
	DISALLOW_COPY_AND_ASSIGN(AmlVideoDecodeAccelerator);
};

}  // namespace aml
