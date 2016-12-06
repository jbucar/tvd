#include "decrypter.h"
#include "../../demuxer/ts.h"
#include "generated/config.h"
#include "impl/dummy.h"
#if CSA_USE_DVBCSA
#	include "impl/dvbcsa.h"
#endif
#include "generated/config.h"
#include <util/cfg/configregistrator.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

namespace tuner {
namespace ca {

#if CSA_USE_DVBCSA
#	define DEFAULT_CSA "dvbcsa"
#else
#	define DEFAULT_CSA "dummy"
#endif

REGISTER_INIT_CONFIG( tuner_ca_csa ) {
	root().addValue( "csa", "Decrypt algorith to use", std::string(DEFAULT_CSA) );
}

Decrypter *Decrypter::create( ID srvID, const std::string &useParam/*=""*/ ) {
	Decrypter *csa = NULL;
	const std::string &use = useParam.empty() ? util::cfg::getValue<std::string> ("tuner.ca.csa") : useParam;
	LINFO("ca", "Using decrypter: use=%s", use.c_str() );

#if CSA_USE_DVBCSA
	if (use == "dvbcsa") {
		csa = new dvbcsa::Decrypter( srvID );
	}
#endif

	if (!csa) {
		csa = new dummy::Decrypter( srvID );
	}

	return csa;
}

Decrypter::Decrypter( ID srvID )
	: _srvID(srvID)
{
}

Decrypter::~Decrypter()
{
}

void Decrypter::updateKeys( ID srvID, const Keys &keys ) {
	if (srvID == _srvID) {
		updateKeys( keys );
	}
}

bool Decrypter::decrypt( util::Buffer *pkt ) {
	BYTE *ptr = pkt->bytes();
	int len = pkt->length();
	int off = 0;

	while (off < len && len-off >= TS_PACKET_SIZE) {
		BYTE *ts = ptr+off;

		//	Check for Transport Error Indicator (TES)
		if (TS_HAS_ERROR(ts) || ts[0] != TS_SYNC) {
			LWARN( "ca", "TS has error or not sync!" );
			return false;
		}

		//	Check for scrambling
		BYTE sc = TS_SCRAMBLING(ts);
		if (sc) {
			if (sc == 2 || sc == 3) {       //      even/odd key
				int payloadOffset = TS_HEAD_SIZE;

				//	Adaptation field exists?
				if (TS_HAS_ADAPTATION(ts)) {
					payloadOffset += 1+TSA_LEN(ts);
				}

				//	Check payload offset
				if (TS_HAS_PAYLOAD(ts) && payloadOffset < TS_PACKET_SIZE) {
					if (!decodePayload( (sc == 2), ts+payloadOffset, TS_PACKET_SIZE-payloadOffset )) {
						return false;
					}
				}

				//	Set ts not scrambled
				ts[3] = ts[3] &~ 0xc0;
			}
			else {
				LWARN( "ca", "Unknown scrambling, continue!" );
				return false;
			}
		}

		off += TS_PACKET_SIZE;
	}

	return true;
}

}
}

