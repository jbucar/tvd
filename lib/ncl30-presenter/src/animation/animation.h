#pragma once

#include <canvas/types.h>
#include <util/types.h>
#include <boost/function.hpp>
#include <string>
#include <vector>
#include <map>

namespace player {
class System;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
class FormatterRegion;
}
}

namespace animation {

namespace bptgnmp = ::br::pucrio::telemidia::ginga::ncl::model::presentation;

typedef boost::function<void( void )> Callback;

class Animation
{
public:
	Animation( int duration, double by, player::System *system );
	virtual ~Animation();

	bool start( bptgnmp::FormatterRegion *formatter );
	bool step();
	void stop();
	void onEnd( const Callback &callback );
	void addProperty( const std::string &property, double startValue, double endValue );

protected:
	bool init();
	bool applyStep();

private:
	bptgnmp::FormatterRegion *_formatter;
	int _remainingSteps;
	int _duration;
	double _by;
	std::map<std::string, std::vector<double> > _values;
	Callback _onStop;
	::util::id::Ident _timer;
	::util::DWORD _startTime;
	player::System *_sys;

};

}
}
}
}
}
}
