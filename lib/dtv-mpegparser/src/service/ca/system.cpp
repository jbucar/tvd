#include "system.h"
#include "../../demuxer/ts.h"
#include "../../demuxer/psi/rawdemuxer.h"
#include "../../demuxer/psi/ecmdemuxer.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace ca {

System::System( FilterDelegate *filter, const desc::ca::Descriptor &desc )
{
	_filter = filter;
	_systemID = desc.systemID;

	_emm.pid = desc.pid;
	_emm.data = desc.data;
	_emm.started = false;
	_emm.isECM = false;
}

System::~System()
{
	stop(_emm);
}

ID System::systemID() const {
	return _systemID;
}

bool System::isAvailable( ID srvID ) const {
	if (srvID == ID(-1)) {
		return true;
	}
	else {
		ECMServices::const_iterator it=_services.find( srvID );
		return (it == _services.end()) ? false : true;
	}
}

bool System::startEMM( const RawCallback &cb ) {
	return start( _emm, cb );
}

bool System::startECM( ID srvID, const RawCallback &cb ) {
	ECMServices::iterator it=_services.find( srvID );
	if (it == _services.end()) {
		LWARN( "ca", "ECM not available for this caID: caID=%04x, srvID=%04x", _systemID, srvID );
		return false;
	}

	Info &ecm = it->second;
	return start( ecm, cb );
}

bool System::start( Info &info, const RawCallback &cb ) {
	if (info.started) {
		//	TODO: Add cb to receive data
		return false;
	}

	if (info.pid == TS_PID_NULL) {
		LWARN( "ca", "Invalid %s pid for caID: caID=%04x, pid=%04x", info.isECM ? "ECM" : "EMM", _systemID, info.pid );
		return false;
	}

	LINFO( "ca", "Starting %s: caID=%04x, pid=%04x", info.isECM ? "ECM" : "EMM", _systemID, info.pid );
	if (!info.isECM) {
		info.started = filter()->startFilter(createDemuxer<RawDemuxer, Raw>( info.pid, cb ));
	}
	else {
		ECMDemuxer *demux = new ECMDemuxer( info.pid, _systemID );
		demux->onParsed( cb );
		info.started = filter()->startFilter(demux);
	}

	return info.started;
}

void System::stop( Info &info ) {
	if (!info.started) {
		return;
	}

	if (info.pid == TS_PID_NULL) {
		LWARN( "ca", "Invalid %s pid for caID: caID=%04x, pid=%04x", info.isECM ? "ECM" : "EMM", _systemID, info.pid );
		return;
	}

	LINFO( "ca", "Stopping %s: caID=%04x, pid=%04x", info.isECM ? "ECM" : "EMM", _systemID, info.pid );
	filter()->stopFilter( info.pid );
	info.started = false;
}

void System::updateECM( ID srvID, const desc::ca::Descriptor &desc, bool start ) {
	ECMServices::iterator it=_services.find( srvID );
	if (start) {
		//	Add service
		Info ecm;
		ecm.started = false;
		ecm.isECM = true;
		ecm.pid = desc.pid;
		ecm.data = desc.data;
		_services[srvID] = ecm;
	}
	else if (it != _services.end()) {
		Info &ecm = it->second;
		stop(ecm);
		ecm.pid = TS_PID_NULL;
		ecm.data.clear();
	}
}

//	Getters
FilterDelegate *System::filter() const {
	return _filter;
}

}
}

