#include "tsdelegate.h"
#include "player.h"
#include "../../ca/decrypter.h"
#include "../../ca/conditional.h"
#include "../../../demuxer/ts.h"
#include "../../../demuxer/psi/pmt.h"
#include "../../../stream/basicpipe.h"
#include "../../../provider/filter.h"
#include "../../../muxer/ts/muxer.h"
#include "../../../muxer/ts/out/output.h"
#include <util/buffer.h>
#include <util/string.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

namespace tuner {
namespace player {
namespace ts {

Delegate::Delegate( muxer::ts::Output *out )
{
	DTV_ASSERT(out);
	_muxer = new muxer::ts::Muxer( out );
	_pipe = NULL;
	_csa = NULL;
	_streamID = -1;
}

Delegate::Delegate( int streamID, muxer::ts::Output *out )
{
	DTV_ASSERT(out);
	_muxer = new muxer::ts::Muxer( out );
	_pipe = NULL;
	_csa = NULL;
	_streamID = streamID;
	DTV_ASSERT(streamID != -1);
}

Delegate::~Delegate()
{
	delete _muxer;
}

//	Initialization
bool Delegate::init() {
	LDEBUG( "ts::Delegate", "Initialize" );

	if (!_muxer->initialize()) {
		LERROR( "ts::Delegate", "Cannot create/initialize ts output" );
		return false;
	}

	//	Make pipe
	_pipe = new stream::BasicPipe( "ts_player_fifo", 3000, TS_PACKET_SIZE*32 );
	_pipe->enable(false);

	//	Start muxer thread
	_thread = boost::thread( boost::bind( &Delegate::reader, this ) );

	return true;
}

void Delegate::fin() {
	LDEBUG( "ts::Delegate", "Finalize" );

	//	Exit thread
	_pipe->exit();
	_thread.join();
	_muxer->finalize();
	DEL(_pipe);
}

//	Getters
const std::string Delegate::url() const {
	return _muxer->output()->url();
}

//	Start/stop
StreamInfo *Delegate::canPlay( const ElementaryInfo & /*info*/, ID /*tag*/ ) const {
	DTV_ASSERT(_streamID != -1);
	const Stream *s = player()->get( _streamID, esType() );
	if (s) {
		return s->sInfo;
	}
	return NULL;
}

void Delegate::start() {
	LDEBUG( "ts::Delegate", "Started" );

	//	Alloc decrypter for service
	ca::Conditional *ca = player()->ca();
	if (ca) {
		_csa = ca->allocDecrypter( player()->srvID() );
	}

	{	//	Setup ts muxer
		ID tsID = 0xABCD;
		desc::Descriptors desc;
		std::vector<ElementaryInfo> elems;
		const Stream *s;
		ID pcrPID = TS_PID_NULL;

		if (_streamID != -1) {
			//	Add elementary stream
			const Stream *s = player()->get( _streamID, esType() );
			if (s) {
				elems.push_back( s->info );
				pcrPID = s->info.pid;
			}
		}
		else {
			//	Add all elementary streams
			s = player()->getVideo();
			if (s) {
				elems.push_back( s->info );
			}
			s = player()->getAudio();
			if (s) {
				elems.push_back( s->info );
			}
			s = player()->getSubtitle();
			if (s) {
				elems.push_back( s->info );
			}

			pcrPID = player()->pcrPID();
		}

		//	Create pmt
		Pmt pmt(
			0x1FFF,  // PID
			1,       // Version
			0xE1E2,  // ProgramID
			pcrPID, // PCR
			desc,
			elems
		);

		//_muxer->enableStats(true);

		//	Start muxer
		_muxer->start( &pmt, tsID );
	}

	//	Enable pipe
	_pipe->enable(true);
	_pipe->notify( stream::mask::user );
}

void Delegate::stop() {
	LDEBUG( "ts::Delegate", "Stopped" );

	//	Disable pipe (and reset data)
	_pipe->enable(false);
	_pipe->notify( stream::mask::user );

	if (_csa) {
		ca::Conditional *ca = player()->ca();
		ca->freeDecrypter( _csa );
		_csa = NULL;
	}

	_muxer->stop();
}

pes::mode::type Delegate::mode() const {
	return pes::mode::ts;
}

stream::Pipe *Delegate::pipe( pes::type::type /*esType*/ ) const {
	return _pipe;
}

void Delegate::reader() {
	util::DWORD mask;
	bool exit=false;

	LINFO( "ts::Delegate", "Begin thread" );

	while (!exit) {
		bool stop=false;
		while (!stop && !exit) {
			util::Buffer *tsPkt = _pipe->get( mask );
			if (tsPkt) {
				bool process = _csa ? _csa->decrypt( tsPkt ) : true;
				if (process) {
					//	Process TS packets
					bool ok=_muxer->process( tsPkt );
					_pipe->free( tsPkt );

					if (!ok) {
						LERROR( "ts::Delegate", "Cannot write buffer content" );
						break;
					}
				}
			}

			//	Check for pipe notifications
			if (mask) {
				LDEBUG( "ts::Delegate", "Pipe notification: mask=%08x", mask );

				if (mask & stream::mask::exit) {
					exit=true;
				}

				if (mask & stream::mask::user) {
					stop=true;
				}
			}
		}
	}

	LINFO( "ts::Delegate", "End thread" );
}

}
}
}

