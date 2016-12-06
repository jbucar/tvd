#include "muxer.h"
#include "out/output.h"
#include "../../demuxer/psi/pmt.h"
#include "../../demuxer/psi/psi.h"
#include <util/functions.h>
#include <util/string.h>
#include <util/url.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/date_time/gregorian/gregorian_types.hpp>

#define HI_BYTE(value) util::BYTE((value & 0xFF00) >> 8)
#define LO_BYTE(value) util::BYTE(value & 0x00FF)

#define ALLOC_BUFFER(b)	  \
	b = new util::Buffer( TS_PACKET_SIZE ); \
	b->resize( TS_PACKET_SIZE );

namespace tuner {
namespace muxer {
namespace ts {

namespace bg = boost::gregorian;

Muxer::Muxer( Output *out )
	: _out(out)
{
	DTV_ASSERT(out);

	_withPSI = true;

	_srvPID = 0;
	_pat = NULL;
	_pmt = NULL;

	_enableStats = false;
}

Muxer::~Muxer()
{
	delete _out;

	DTV_ASSERT(!_pat);
	DTV_ASSERT(!_pmt);
}

bool Muxer::initialize() {
	if (!output()->initialize()) {
		LWARN( "ts::Muxer", "Cannot initialize output" );
		return false;
	}
	return true;
}

void Muxer::finalize() {
	_out->finalize();
}

void Muxer::start( Pmt *pmt, ID tsID ) {
	if (_withPSI) {
		makePAT( tsID, pmt );
		makePMT( pmt );

		//	Force write sections on begin
		_lastPSI = pt::microsec_clock::local_time();
		_lastPSI -= bg::days(1);
	}
}

void Muxer::stop() {
	if (_enableStats) {
		printStats();
	}

	if (_withPSI) {
		DEL(_pat);
		DEL(_pmt);
	}
}

bool Muxer::process( util::Buffer *buf ) {
	if (_enableStats) {
		doStats( buf );
	}

	if (_withPSI) {
		pt::ptime now = pt::microsec_clock::local_time();
		int msElapsed = (int)(now - _lastPSI).total_milliseconds();
		if (msElapsed > 500) {
			if (!writePSI( _pat )) { return false; }
			if (!writePSI( _pmt )) { return false; }

			_lastPSI = now;
		}
	}

	return write( buf );
}

void Muxer::enablePSI( bool st ) {
	_withPSI = st;
}

void Muxer::enableStats( bool st ) {
	_enableStats = st;
}

//	Output
Output *Muxer::output() const {
	return _out;
}

bool Muxer::write( util::Buffer *buf ) {
	return _out->write( buf );
}

bool Muxer::writePSI( util::Buffer *buf ) {
	//	Write PAT
	if (!write( buf )) {
		LWARN( "ts::Muxer", "Cannot write to output" );
		return false;
	}
	updateCC( buf->bytes() );
	return true;
}

//	Stats
void Muxer::printStats() {
	std::string stats= util::format( "Muxer stats: pids seen=%d", _stats.size() );
	for (Stats::iterator it=_stats.begin(); it!=_stats.end(); ++it) {
		stats += util::format( " pid=(%04x,%llu)", it->first, it->second );
	}
	LDEBUG( "ts::Muxer", "%s", stats.c_str() );

	_stats.clear();
}

void Muxer::doStats( util::Buffer *buf ) {
	size_t off = 0;
	util::BYTE *ptr = buf->bytes();
	size_t len = buf->length();

	while (off < len && len-off > TS_PACKET_SIZE) {
		DTV_ASSERT( (*ptr) == 0x47 );	//	Sync
		ID pid = TS_PID( ptr );

		const Stats::iterator it = _stats.find( pid );
		if (it != _stats.end()) {
			(*it).second++;
		}
		else {
			LDEBUG( "ts::Muxer", "Add pid for stats: pid=%04x", pid );
			_stats[pid] = 1;
		}

		off += TS_PACKET_SIZE;
	}
}

//	Muxer methods
void Muxer::beginPacket( BYTE *ptr, int &i, ID pid ) {
	//	TS header
	ptr[i++] = 0x47;	//	Sync byte
	ptr[i++] = util::BYTE(0x40 | ((pid >> 8) & 0x1F));	//	Transport error indicator (1) + Payload unit start (1) + Transport priority (1) + PID (5)
	ptr[i++] = LO_BYTE(pid);
	ptr[i++] = 0x10;	//	Scrambled control (2) + Adaption field (2), Countinuity counter (4)

	//	Pointer field
	ptr[i++] = 0x00;
}

void Muxer::endPacket( BYTE *buf, int &pos )  {
	int len  = pos - (TS_HEAD_SIZE + 1 + 3) + 4;

	//	Set LEN
	BYTE *ptr = buf + (TS_HEAD_SIZE + 1);	//	section ptr
	ptr[1] = (util::BYTE)((HI_BYTE(len) & 0x0F) | 0xb0);
	ptr[2] = LO_BYTE(len);

	//	Set CRC
	DWORD calc = util::crc_calc( util::DWORD(-1), ptr, len+PSI_GENERIC_HEADER-PSI_CRC_SIZE );
	BYTE *ptrCRC = (BYTE *)&calc;
	buf[pos++] = ptrCRC[3];
	buf[pos++] = ptrCRC[2];
	buf[pos++] = ptrCRC[1];
	buf[pos++] = ptrCRC[0];

	//	Fill unused space
	memset( buf+pos, 0xff, TS_PACKET_SIZE-pos );
}

void Muxer::updateCC( util::BYTE *ptr ) {
	util::BYTE cc = TS_CONTINUITY(ptr);
	cc = (cc + 1) & 0x0f;
	ptr[3] = 0x10 | cc;
}

void Muxer::makePAT( ID tsID, Pmt *pmt ) {
	DTV_ASSERT(!_pat);

	//	Setup PAT
	ALLOC_BUFFER(_pat);
	BYTE *ptr = _pat->bytes();
	int i=0;

	beginPacket( ptr, i, TS_PID_PAT );

	{	//	PAT data
		ptr[i++] = 0x00;	//	tableID
		ptr[i++] = 0xb0;	//	section syntax indicator (1=1), '0' (1=0), reserved1 (2='11'), len (12)
		ptr[i++] = 0x00;	//	len (Filled in method endSection)
		ptr[i++] = HI_BYTE(tsID);	//	TS ID
		ptr[i++] = LO_BYTE(tsID);	//	TS ID
		ptr[i++] = 0xc3;	//	reserved (2), version (5=1), current next indicator (1)
		ptr[i++] = 0x00;	//	# sec
		ptr[i++] = 0x00;	//	last sec
	}

	//	List of programs
	//	NIT
	ptr[i++] = 0x00;
	ptr[i++] = 0x00;
	ptr[i++] = 0x1F;
	ptr[i++] = 0xFF;

	{	//	Service
		ID srvID = pmt->programID();
		_srvPID = selectPID(pmt);

		ptr[i++] = HI_BYTE(srvID);
		ptr[i++] = LO_BYTE(srvID);
		ptr[i++] = HI_BYTE(_srvPID);
		ptr[i++] = LO_BYTE(_srvPID);
	}

	endPacket( ptr, i );
}

void Muxer::makePMT( Pmt *pmt ) {
	DTV_ASSERT(!_pmt);

	//	Setup PMT
	ALLOC_BUFFER(_pmt);
	BYTE *ptr = _pmt->bytes();
	int i=0;

	{	//	Begin packet
		beginPacket( ptr, i, _srvPID );
	}

	{	//	PMT data
		ID srvID = pmt->programID();

		ptr[i++] = PSI_TID_PMT;	//	tableID
		ptr[i++] = 0xb0;	//	section syntax indicator (1=1), '0' (1=0), reserved1 (2='11'), len (12)
		ptr[i++] = 0x00;	//	len (Filled in method endSection)
		ptr[i++] = HI_BYTE(srvID);	//	programID
		ptr[i++] = LO_BYTE(srvID);	//	programID
		ptr[i++] = 0xc3;	//	reserved (2), version (5=1), current next indicator (1)
		ptr[i++] = 0x00;	//	# sec
		ptr[i++] = 0x00;	//	last sec
	}

	{	//	PCR pid
		ID pcrPID = pmt->pcrPID();
		ptr[i++] = HI_BYTE(pcrPID);
		ptr[i++] = LO_BYTE(pcrPID);
	}

	//	Program info len (descriptors)
	ptr[i++] = 0x00;
	ptr[i++] = 0x00;

	//	Loop streams begin
	const std::vector<ElementaryInfo> &elements = pmt->elements();
	BOOST_FOREACH( const ElementaryInfo &info, elements ) {
		addStream( ptr, i, info );
	}
	//	Loop streams end

#ifdef _DEBUG
	pmt->show();
#endif

	endPacket( ptr, i );
}

ID Muxer::selectPID( Pmt *pmt ) {
	std::vector<ID> usedPids;

	//	Add all pids used
	usedPids.push_back( pmt->pcrPID() );
	const std::vector<ElementaryInfo> &elements = pmt->elements();
	BOOST_FOREACH( const ElementaryInfo &info, elements ) {
		usedPids.push_back( info.pid );
	}

	for (ID tmp=0x100; tmp<TS_PID_NULL; tmp++) {
		std::vector<ID>::const_iterator it=std::find(
			usedPids.begin(),
			usedPids.end(),
			tmp
		);
		if (it != usedPids.begin()) {
			return tmp;
		}
	}
	DTV_ASSERT(false);
	return TS_PID_NULL;
}

void Muxer::addStream( BYTE *ptr, int &i, const ElementaryInfo &info ) {
	ptr[i++] = info.streamType;
	ptr[i++] = HI_BYTE(info.pid);
	ptr[i++] = LO_BYTE(info.pid);

	{	//	Descriptors
		size_t dLen = info.descriptors.length();
		util::BYTE *desc = info.descriptors.bytes();
		ptr[i++] = HI_BYTE(dLen);
		ptr[i++] = LO_BYTE(dLen);
		for (size_t x=0; x<dLen; x++) {
			ptr[i++] = desc[x];
		}
	}
}

}
}
}

