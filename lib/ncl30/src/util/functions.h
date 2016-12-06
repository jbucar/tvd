#include <util/types.h>
#include <string>
#include <list>

namespace ncl_util {

float getPercentualValue( const std::string &value, bool throwException = false);
bool isPercentualValue( const std::string &value );

int getPixelValue( const std::string &value, bool throwException = false);

double NaN();
double infinity();
bool isNaN( double value );
bool isInfinity( double value );
bool isValidNumericValue( const std::string &values);
bool parseTimeString( const std::string &value, util::DWORD &ms);

}
