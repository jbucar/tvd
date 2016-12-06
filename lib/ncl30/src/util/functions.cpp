#include "functions.h"
#include <util/log.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>

namespace ncl_util {

#define CHECK_RANGE(var,min,max)				\
	{ if (var < min) var = min; else if (var > max) var = max; }

double NaN() {
	return -1.0;
}

double infinity() {
	return std::numeric_limits<double>().infinity();
}

bool isNaN( double value ) {
	return value < 0;
}

bool isInfinity( double value ) {
	return value == std::numeric_limits<double>().infinity();
}

int getPixelValue( const std::string &value, bool throwException ) {

	int pValue = 0;

	std::string actualValue = value;
	boost::trim( actualValue );
	boost::trim_right_if( actualValue, boost::is_any_of( "px" ) );

	try {
		pValue = boost::lexical_cast<int>( boost::trim_copy( actualValue ) );
	} catch (boost::bad_lexical_cast &) {
		if (throwException) {
			throw;
		}
	}

	return pValue;
}

float getPercentualValue( const std::string &value, bool throwException ) {

	float pValue = 0.0;

	std::string actualValue = value;
	boost::trim( actualValue );
	boost::trim_right_if( actualValue, boost::is_any_of( "%" ) );
	try {
		pValue = boost::lexical_cast<float>( actualValue ) / 100;
		if (!throwException) {
			CHECK_RANGE( pValue, 0, 1 );
		}
	} catch (boost::bad_lexical_cast &) {
		if (throwException) {
			throw;
		}
	}

	return pValue;
}

bool isPercentualValue( const std::string &value ) {
	std::string v = value;
	boost::trim( v );
	return v.find( "%" ) == v.size() - 1;
}

bool isValidNumericValue( const std::string &value ) {
	bool result = true;
	try {
		if (isPercentualValue( value )) {
			float percentualValue = getPercentualValue( value, true );
			if ((percentualValue > 1) || (percentualValue < 0)) {
				LERROR("functions", "The value %s must be between 0 and 100", value.c_str());
				result = false;
			}
		} else {
			int pxValue = getPixelValue( value, true );
			if (pxValue < 0) {
				LERROR("functions", "The value %s is invalid", value.c_str());
				result = false;
			}
		}
	} catch (boost::bad_lexical_cast &) {
		LERROR("functions", "The value %s is invalid", value.c_str());
		result = false;
	}
	return result;
}

//Case 1: string is a number.
//Case 2: string is a number terminated in 's'.
//Case 3: string is hours:minutes:seconds.fraction
bool parseTimeString(const std::string& value, util::DWORD &ms) {
	bool result=false;
	double seconds;
	unsigned int hours, minutes;
	char bytes[3];

	if (sscanf( value.c_str(), "%u:%u:%lf", &hours, &minutes, &seconds ) == 3) {
		//	Case 3
		if (hours < 24 && minutes < 60 && seconds < 60) {
			ms = boost::math::iround((hours * 3600 + minutes * 60 + seconds) * 1000);
			result=true;
		}
	}
	else if (sscanf( value.c_str(), "%lf%2s", &seconds, bytes ) == 2) {
		//	Case 2		
		if (bytes[0] == 's' && bytes[1] == '\0') {
			ms = boost::math::iround(seconds * 1000);
			result=true;
		}
	}
	else if (sscanf( value.c_str(), "%lf", &seconds ) == 1) {
		//	Case 1
		ms = boost::math::iround(seconds * 1000);
		result=true;
	}

	if (!result) {
		LERROR("functions", "The format of time is invalid. value=%s", value.c_str() );
	}

	return result;
}

}
