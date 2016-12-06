#include "networkdelegate.h"
#include "net/base/net_errors.h"
#include "net/base/static_cookie_policy.h"
#include "net/url_request/url_request.h"

namespace tvd {

NetworkDelegate::NetworkDelegate()
{
}

NetworkDelegate::~NetworkDelegate()
{
}

bool NetworkDelegate::OnCanAccessFile( const net::URLRequest &request, const base::FilePath &path ) const {
	return true;
}

int NetworkDelegate::OnBeforeURLRequest( net::URLRequest *request, const net::CompletionCallback &callback, GURL *new_url ) {
	return net::OK;
}

int NetworkDelegate::OnBeforeSendHeaders( net::URLRequest *request, const net::CompletionCallback &callback, net::HttpRequestHeaders *headers ) {
	return net::OK;
}

void NetworkDelegate::OnSendHeaders( net::URLRequest *request, const net::HttpRequestHeaders &headers ) {
}

int NetworkDelegate::OnHeadersReceived(
    net::URLRequest* request,
    const net::CompletionCallback& callback,
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers,
    GURL* allowed_unsafe_redirect_url)
{
	return net::OK;
}

void NetworkDelegate::OnBeforeRedirect( net::URLRequest *request, const GURL &new_location ) {
}

void NetworkDelegate::OnResponseStarted( net::URLRequest *request ) {
}

void NetworkDelegate::OnCompleted( net::URLRequest *request, bool started ) {
}

void NetworkDelegate::OnURLRequestDestroyed( net::URLRequest *request ) {
}

void NetworkDelegate::OnPACScriptError( int line_number, const base::string16 &error ) {
}

NetworkDelegate::AuthRequiredResponse NetworkDelegate::OnAuthRequired(
    net::URLRequest* request,
    const net::AuthChallengeInfo& auth_info,
    const AuthCallback& callback,
    net::AuthCredentials* credentials)
{
	return AUTH_REQUIRED_RESPONSE_NO_ACTION;
}

bool NetworkDelegate::OnCanGetCookies( const net::URLRequest &request, const net::CookieList &cookie_list ) {
	net::StaticCookiePolicy policy(net::StaticCookiePolicy::ALLOW_ALL_COOKIES);
	int rv = policy.CanGetCookies(request.url(), request.first_party_for_cookies());
	return rv == net::OK;
}

bool NetworkDelegate::OnCanSetCookie( const net::URLRequest &request, const std::string &cookie_line, net::CookieOptions *options ) {
	net::StaticCookiePolicy policy(net::StaticCookiePolicy::ALLOW_ALL_COOKIES);
	int rv = policy.CanSetCookie(request.url(), request.first_party_for_cookies());
	return rv == net::OK;
}

}
