#pragma once

#include "content/public/browser/access_token_store.h"

namespace tvd {

class BrowserContext;

class AccessTokenStore : public content::AccessTokenStore {
public:
	explicit AccessTokenStore( BrowserContext *browserCtx );

private:
	~AccessTokenStore() override;

	void GetRequestContextOnUIThread( BrowserContext *browserCtx );
	void RespondOnOriginatingThread( const LoadAccessTokensCallbackType &callback );

	// AccessTokenStore implementation:
	void LoadAccessTokens( const LoadAccessTokensCallbackType &callback ) override;
	void SaveAccessToken( const GURL& server_url, const base::string16& access_token) override;

	AccessTokenSet _accessTokens;
	BrowserContext *_browserCtx;
	scoped_refptr<net::URLRequestContextGetter> _sysRequestCtx;

	DISALLOW_COPY_AND_ASSIGN(AccessTokenStore);
};

}
