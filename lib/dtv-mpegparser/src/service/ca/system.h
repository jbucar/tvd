#pragma once

#include "types.h"
#include "../../demuxer/descriptors/ca.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <map>

namespace tuner {
namespace ca {

class FilterDelegate;

class System {
public:
	System( FilterDelegate *, const desc::ca::Descriptor &desc );
	virtual ~System();

	//	Types
	typedef boost::shared_ptr<util::Buffer> RawData;
	typedef boost::function<void (const RawData &)> RawCallback;

	ID systemID() const;
	bool isAvailable( ID srvID ) const;
	bool startEMM( const RawCallback &cb );
	bool startECM( ID srvID, const RawCallback &cb );

	void updateECM( ID srvID, const desc::ca::Descriptor &desc, bool start );

protected:
	typedef struct {
		ID pid;
		std::vector<util::BYTE> data;
		bool isECM;
		bool started;
	} Info;

	typedef std::map<ID,Info> ECMServices;

	bool start( Info &info, const RawCallback &cb );
	void stop( Info &info );
	FilterDelegate *filter() const;

private:
	FilterDelegate *_filter;
	ID _systemID;
	Info _emm;
	ECMServices _services;
};

}
}

