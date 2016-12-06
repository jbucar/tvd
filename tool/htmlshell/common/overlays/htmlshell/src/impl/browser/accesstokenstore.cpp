#include "accesstokenstore.h"
#include "browsercontext.h"
#include "content/public/browser/browser_thread.h"
#include "net/url_request/url_request_context_getter.h"

namespace tvd {

AccessTokenStore::AccessTokenStore( BrowserContext *browserCtx )
	: _browserCtx(browserCtx),
	  _sysRequestCtx(nullptr)
{
}

AccessTokenStore::~AccessTokenStore()
{
}

void AccessTokenStore::GetRequestContextOnUIThread( BrowserContext *browserCtx ) {
	_sysRequestCtx = browserCtx->GetRequestContext();
}

void AccessTokenStore::RespondOnOriginatingThread( const LoadAccessTokensCallbackType &callback ) {
	callback.Run(_accessTokens, _sysRequestCtx.get());
	_sysRequestCtx = NULL;
}

void AccessTokenStore::LoadAccessTokens(const LoadAccessTokensCallbackType& callback) {
	content::BrowserThread::PostTaskAndReply(content::BrowserThread::UI, FROM_HERE,
		base::Bind(&AccessTokenStore::GetRequestContextOnUIThread, this, _browserCtx),
		base::Bind(&AccessTokenStore::RespondOnOriginatingThread, this, callback));
}

void AccessTokenStore::SaveAccessToken( const GURL &server_url, const base::string16 &access_token ) {
	if (_accessTokens[server_url] != access_token) {
		_accessTokens[server_url] = access_token;
	}
}

}
