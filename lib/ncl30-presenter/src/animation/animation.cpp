#include "animation.h"
#include "../../include/model/FormatterRegion.h"
#include <gingaplayer/player.h>
#include <gingaplayer/system.h>
#include <gingaplayer/property/types.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>

namespace prop = player::property::type;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace animation {
Animation::Animation( int duration, double by, player::System *system ) {
	_formatter = NULL;
	_duration = duration;
	_by = by;
	_remainingSteps = 0;
	_sys = system;
}

Animation::~Animation() {
	_formatter = NULL;
	_sys->unregisterTimer( _timer );
}

void Animation::addProperty( const std::string &property, double startValue, double endValue ) {
	if (!_by) {
		_by = 0.04; // ~ 24 FPS
	}
	LDEBUG("Animation", "animation, property=%s, startValue=%f, endValue=%f, duration=%d, by=%f", property.c_str(), startValue, endValue, _duration, _by);

	_remainingSteps = (int) (_duration / _by);
	double increment = (endValue - startValue) / (_remainingSteps * 1.0);

	if (increment != 0.0 && _remainingSteps) {
		LDEBUG("Animation", "animation, steps=%d, increment=%f", _remainingSteps, increment);

		std::string p = property;
		for (int i = 0; i < _remainingSteps; ++i) {
			_values[p].insert( _values[p].begin(), boost::math::iround( startValue + increment * i ) );
		}
		_values[p].insert( _values[p].begin(), endValue );
	}
}

bool Animation::init() {
	return _values.size() > .0;
}

bool Animation::start( bptgnmp::FormatterRegion *formatter ) {
	_formatter = formatter;
	if (init()) {
		_startTime = _formatter->uptime();
		step();
		return true;
	}
	return false;
}

bool Animation::step() {

	// adjust remaining steps according to the remaining time
	::util::DWORD remainingTime = (_duration * 1000) - (_formatter->uptime() - _startTime);
	int stepsInRemainingTime = (int) (remainingTime / (_by * 1000));
	if (_remainingSteps > stepsInRemainingTime) {
		_remainingSteps = stepsInRemainingTime + 1;
	}

	if (_remainingSteps) {
		_remainingSteps -= 1;
		if (applyStep() && _remainingSteps) {
			_timer = _sys->registerTimer( (util::DWORD) (_by * 1000), boost::bind( &bptgnmp::FormatterRegion::stepAnimation, _formatter, this ) );
			return true;
		} else {
			LINFO("Animation", "end, duration=%d ms", (_formatter->uptime() - _startTime) );
			_sys->unregisterTimer( _timer );
			_remainingSteps = 0;
		}
	}
	return false;
}

bool Animation::applyStep() {
	bool result = false;
	_formatter->enabled( false );
	std::map<std::string, std::vector<double> >::iterator it = _values.begin();
	for (; it != _values.end(); ++it) {
		std::string value = boost::lexical_cast<string>( (*it).second[_remainingSteps] );
		std::string property = (*it).first;
		if (property == "transparency") {
			value += "%";
		}
		result = _formatter->setPropertyToPlayer( property, value );
	}
	_formatter->enabled( true );
	_formatter->updateRegionBounds();
	return result;
}

void Animation::onEnd( const Callback &callback ) {
	_onStop = callback;
}

void Animation::stop() {
	if (!_onStop.empty()) {
		_onStop();
	}
}

}
}
}
}
}
}

