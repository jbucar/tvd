#pragma once

#define WEB_API "webapi"
#define WEB_API_VERSION_MAJOR 1u
#define WEB_API_VERSION_MINOR 0u

#include <boost/function.hpp>
#include <string>

namespace tvd {
namespace web {

struct RequestS {
	int browserId;
	int queryId;
	std::string name;
	std::string params;
};
typedef struct RequestS Request;

struct ResponseS {
	int browserId;
	int queryId;
	bool isSignal;
	std::string error;
	std::string params;
};
typedef struct ResponseS Response;

// Handler typedef
typedef boost::function<bool(const Request &)> RequestHandler;

class ApiInterface_1_0 {
public:
	virtual bool addJsApi( const std::string &name, RequestHandler handler )=0;
	virtual bool removeJsApi( const std::string &name )=0;
	virtual bool executeCallback( const Response &response )=0;
};

}
}
