#include "amlvideodecodeaccelerator.h"
#include "src/decoder.h"
#include "base/memory/singleton.h"
#include "base/bind.h"
#include <sys/stat.h>
#include <fcntl.h>

#define RETURN_ON_FAILURE(result, log, error)	  \
	do { \
		if (!(result)) { \
			LOG(ERROR) << log; \
			_client->NotifyError(error); \
			return; \
		} \
	} while (0)

namespace aml {

class DecoderSingleton {
public:
	~DecoderSingleton() {
		CHECK(_ref == 0);
		delete _decoder;
	}

	static DecoderSingleton* GetInstance() {
		return base::Singleton<DecoderSingleton>::get();
	}

	bool init() {
		base::AutoLock auto_lock(_lock);
		if (!_ref) {
			_ref++;
			return _decoder->initialize( hwdec::render, codec::h264 );
		}
		return false;
	}

	void fin() {
		CHECK(_ref == 1);
		base::AutoLock auto_lock(_lock);
		_decoder->finalize();
		_ref--;
	}

	Decoder *decoder() const {
		return _decoder;
	}

private:
	Decoder *_decoder;
	int _ref;
	base::Lock _lock;

	DecoderSingleton() {
		_decoder = new Decoder();
		_ref = 0;
	};

	friend struct base::DefaultSingletonTraits<DecoderSingleton>;
	DISALLOW_COPY_AND_ASSIGN(DecoderSingleton);
};


AmlVideoDecodeAccelerator::AmlVideoDecodeAccelerator()
	: _client(NULL), _loop(base::MessageLoop::current()), _thread("aml_decoder")
{
}

AmlVideoDecodeAccelerator::~AmlVideoDecodeAccelerator()
{
	CHECK_EQ(_loop, base::MessageLoop::current());
}

// media::VideoDecodeAccelerator implementation.
bool AmlVideoDecodeAccelerator::Initialize(const media::VideoDecodeAccelerator::Config& config, Client* client) {
	CHECK_EQ(_loop, base::MessageLoop::current());
	CHECK(config.profile >= media::H264PROFILE_MIN && config.profile <= media::H264PROFILE_MAX);

	if (DecoderSingleton::GetInstance()->init()) {
		LOG(INFO) << "Initialize";
		_client = client;
		_mtTaskRunner = base::MessageLoop::current()->task_runner();
		startDecoderThread();
		return true;
	}
	LOG(WARNING) << "Fail to initialize!";
	return false;
}

void AmlVideoDecodeAccelerator::Destroy() {
	CHECK_EQ(_loop, base::MessageLoop::current());
	LOG(INFO) << "Destroy";
	_thread.Stop();
	DecoderSingleton::GetInstance()->fin();
}

void AmlVideoDecodeAccelerator::Flush() {
	_dtTaskRunner->PostTask(FROM_HERE, base::Bind(&AmlVideoDecodeAccelerator::processQueue, base::Unretained(this), true));
}

void AmlVideoDecodeAccelerator::Reset() {
	_dtTaskRunner->PostTask(FROM_HERE, base::Bind(&AmlVideoDecodeAccelerator::resetQueue, base::Unretained(this)));
}

bool AmlVideoDecodeAccelerator::CanDecodeOnIOThread() {
	return true;
}

void AmlVideoDecodeAccelerator::Decode(const media::BitstreamBuffer& bitstream_buffer) {
	RETURN_ON_FAILURE(_client,"Call to Decode() during invalid state.", ILLEGAL_STATE);
	{
		base::AutoLock auto_lock(_lock);
		_queue.push(bitstream_buffer);
	}
	_dtTaskRunner->PostTask(FROM_HERE, base::Bind(&AmlVideoDecodeAccelerator::processQueue, base::Unretained(this), false));
}

void AmlVideoDecodeAccelerator::AssignPictureBuffers( const std::vector<media::PictureBuffer>& buffers) {
	LOG(INFO) << "Ignoring AssignPictureBuffers!";
}

void AmlVideoDecodeAccelerator::ReusePictureBuffer(int32_t picture_buffer_id) {
	LOG(INFO) << "Ignoring ReusePictureBuffer!";
}

void AmlVideoDecodeAccelerator::processQueue( bool flush ) {
	CHECK(_dtTaskRunner->BelongsToCurrentThread());
	std::queue<media::BitstreamBuffer> queueToProcess;

	{
		base::AutoLock auto_lock(_lock);
		if (!_queue.empty()) {
			if (!decoder()->canFeed()) {
				LOG(INFO) << "Cant feed, wait: items=" << _queue.size();

				//	Try again in 5ms
				_dtTaskRunner->PostDelayedTask(FROM_HERE, base::Bind(&AmlVideoDecodeAccelerator::processQueue, base::Unretained(this), flush), base::TimeDelta::FromMilliseconds(100));
				return;
			}
			std::swap(_queue, queueToProcess);
		}
	}

	while (!queueToProcess.empty()) {
		processItem( queueToProcess.front() );
		queueToProcess.pop();
	}

	if (flush) {
		VLOG(1) << "Flush end!";
		_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyFlushDone,base::Unretained(_client)));
	}
}

void AmlVideoDecodeAccelerator::processItem( const media::BitstreamBuffer &buffer ) {
	//	Set frame PTS
	decoder()->setPTS( buffer.presentation_timestamp().InMicroseconds() );

	// Map the bitstream buffer.
	base::SharedMemory memory(buffer.handle(), true);
	size_t size = buffer.size();
	RETURN_ON_FAILURE(memory.Map(size),"Failed to SharedMemory::Map()",UNREADABLE_INPUT);
	const uint8_t* buf = static_cast<uint8_t*>(memory.memory());

	// NALUs are stored with Annex B format in the bitstream buffer (start codes),
	// but Amlogic expects custom header :)
	// 1. Locate relevant NALUs and compute the size of the translated data.
	_parser.SetStream(buf, size);
	media::H264NALU nalu;
	while (true) {
		media::H264Parser::Result result = _parser.AdvanceToNextNALU(&nalu);
		if (result == media::H264Parser::kEOStream) {
			break;
		}
		CHECK_EQ(result, media::H264Parser::kOk);

		//	Try decode buffer
		if (!decoder()->decodeFrame( nalu.data, nalu.size )) {
			LOG(ERROR) << "Error decoding frame!";
			_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyError, base::Unretained(_client), UNREADABLE_INPUT));
		}
	}

	//	Get pictures
	FrameInfo frameInfo;
	//int picts = decoder()->picturesAvailables();
	if (decoder()->getVideoInfo( frameInfo ) && frameInfo.width > 0) {
		//	Send picture ready
		_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::PictureReady, base::Unretained(_client),
			media::Picture(-1, buffer.id(),gfx::Rect(0,0,frameInfo.width,frameInfo.height), false)));
	}

	//	End bitstream buffer!
	_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyEndOfBitstreamBuffer,base::Unretained(_client),buffer.id()));
}

void AmlVideoDecodeAccelerator::clearQueue() {
	CHECK(_dtTaskRunner->BelongsToCurrentThread());

	while (!_queue.empty()) {
		int32_t bufferID = _queue.front().id();
		_queue.pop();

		if (bufferID != -1) {
			_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyEndOfBitstreamBuffer, base::Unretained(_client), bufferID));
		}
	}

	//	Reset queue
	std::queue<media::BitstreamBuffer> empty_queue;
	std::swap(_queue, empty_queue);
}

void AmlVideoDecodeAccelerator::resetQueue() {
	base::AutoLock auto_lock(_lock);
	clearQueue();
	if (!decoder()->reset()) {
		LOG(WARNING) << "Reset failure!";
		_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyError, base::Unretained(_client), PLATFORM_FAILURE));
	}
	VLOG(1) << "Reset done!";
	_mtTaskRunner->PostTask(FROM_HERE, base::Bind(&Client::NotifyResetDone, base::Unretained(_client)));
}

void AmlVideoDecodeAccelerator::startDecoderThread() {
	_thread.Start();
	_dtTaskRunner = _thread.task_runner();
}

Decoder *AmlVideoDecodeAccelerator::decoder() const {
	return DecoderSingleton::GetInstance()->decoder();
}

}  // namespace aml
