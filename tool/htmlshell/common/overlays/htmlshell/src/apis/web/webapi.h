#pragma once

#include "../shellapi.h"
#include "dtv-htmlshellsdk/src/apis/web.h"
#include <map>

namespace tvd {

namespace impl {
class WebApi_1_0;
}

class BrowserClient;

class WebApi : public ShellApi {
public:
	explicit WebApi( BrowserClient *client );
	virtual ~WebApi();

	bool hasJsApi( const std::string &name ) const;
	bool addJsApi( const std::string &name, web::RequestHandler handler );
	bool removeJsApi( const std::string &name );

	bool onMessageReceived( const web::Request &request );
	virtual bool executeCallback( const web::Response &response );

protected:
	virtual bool init() override;
	virtual void fin() override;

	virtual bool registerInterfaces() override;
	virtual uint32_t getLastVersion() const override;

private:
	BrowserClient *_client;
	std::map<std::string,web::RequestHandler> _jsApis;
	impl::WebApi_1_0 *_iface;
};

}
