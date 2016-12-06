#pragma once

#include <util/tool.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace util {

class Buffer;

namespace task {
	class Dispatcher;
}
namespace io {
	class Dispatcher;
}
}

namespace tuner {

namespace dsmcc {
	class Event;
	class ObjectCarousel;
	class DSMCCDemuxer;
	class StreamEventDemuxer;
	typedef std::vector<Event *> Events;
	typedef std::vector<std::string> Download;
}

	class Provider;
}

typedef boost::function<void ( const boost::shared_ptr<util::Buffer> &buf )> PSICallback;

class dvbtool : public util::Tool {
public:
	dvbtool();
	virtual ~dvbtool();

protected:
	virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );
	virtual int run( util::cfg::cmd::CommandLine &cmd );

	//	OC download
	void onMounted( const boost::shared_ptr<tuner::dsmcc::ObjectCarousel> &oc );
	void downloadObjectCarousel();

	//	Update
	void onDownload( const boost::shared_ptr<tuner::dsmcc::Download> &download );
	void downloadUpdate( int pid );

	//	PSI pid filter
	void onFilterPSISection( int pid, const boost::shared_ptr<util::Buffer> &buf );
	void runFilterPID( int pid );

	//	Signal
	void showSignal();
	
	//	PID scan
	void scanPID( int pid );
	void onPidScanCallback( int pid, const boost::shared_ptr<util::Buffer> &buf );
	void onPidTimeout( int pid );
	void runPIDScan();
	void processPID( int pid );

	//	Aux
	bool filterPID( int pid, const PSICallback &callback );
	util::task::Dispatcher *disp() const;

private:
	util::io::Dispatcher *_io;
	tuner::Provider *_provider;

	int _pidScanLast;
};

