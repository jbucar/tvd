#include "extension.h"
#include "generated/config.h"
#include "generated/htmlshell.pb.h"
#include <htmlshellsdk/apis/browser.h>
#include <htmlshellsdk/apis/fs.h>
#include <htmlshellsdk/apis/web.h>
#include <htmlshellsdk/apis/system.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>

#define URL_PARAMETER "tvd-remote-server-url"
#define MAJOR_VERSION 1
#define MINOR_VERSION 3
#define MESSAGE_HEADER sizeof(util::DWORD)
#define MSG_SIZE(ptr) GET_DWORD(ptr)

MAKE_HTMLSHELL_LIBRARY(remote::Extension,HTMLSHELLREMOTE);

namespace remote {

Extension::Extension()
	: _socket(new Socket(_io))
{
	_browserApi = NULL;
	_listener = NULL;

	_webApi = NULL;
	_systemApi = NULL;

	_retryConnectTimeout = 500;
	_maxConnectRetry     = 5;
	_dataRead = new util::Buffer(1024);
	_dataWrite = new util::Buffer(1024);
	_msg = new util::Buffer(1024);
}

Extension::~Extension()
{
	delete _msg;
	delete _dataRead;
	delete _dataWrite;
}

// tvd::HtmlShellLibrary methods:
bool Extension::init() {
	LINFO("Extension", "Initializing htmlshell remote extension");

	{	//	Load APIs
		bool result = true;
		result &= loadApi(BROWSER_API, 1, 0, &_browserApi);
		result &= loadApi(WEB_API, 1, 0, &_webApi);
		result &= loadApi(SYSTEM_API, 1, 0, &_systemApi);
		if (!result) {
			return false;
		}
	}

	{	//	Check parameters
		std::string url = _systemApi->getCmdSwitch("tvd-remote-server-url");
		if (url.empty()) {
			LERROR("Extension", "Invalid " URL_PARAMETER " parameter" );
			return false;
		}
		LINFO("Extension", "Using server url=%s", url.c_str() );
	}

	//	Setup browser API callbacks
	_listener = new tvd::browser::ListenerForwarder<Extension>(this);
	_browserApi->addListener(_listener);

	//	Setup Web API callbacks
	if (!_webApi->addJsApi("RemoteExtension", boost::bind(&Extension::onRequestJS,this,_1))) {
		LERROR("Extension", "Cannot add web handler" );
		return false;
	}

	//	Start thread
	_thread = boost::thread(boost::bind(&Extension::threadLoop, this));

	return true;
}

void Extension::fin() {
	LINFO("Extension", "Finalizing htmlshell remote extension");

	_io.stop();
	_thread.join();
	_socket->close();

	_browserApi->rmListener(_listener);
	DEL(_listener);
	_browserApi = NULL;

	_webApi->removeJsApi("RemoteExtension");
	_webApi = NULL;

	_systemApi = NULL;
}

void Extension::threadLoop() {
	LINFO("Extension", "Begin htmlshell remote extension thread!");

	//	Connect to server
	_socket->async_connect(
		getEndpoint(),
		boost::bind(&Extension::onConnect,this,boost::asio::placeholders::error)
	);

	try {
		_io.run();
	}
	catch (std::exception& e) {
		LWARN("Extension", "Exception in thread io loop: %s", e.what());
	}

	LINFO("Extension", "End htmlshell remote extension thread!");
}

void Extension::shutdown() {
	LINFO("Extension", "Exit htmlshell!");
	_systemApi->shutdown(0);
	_io.stop();
}

//	Web API callbacks
bool Extension::onRequestJS( const tvd::web::Request &req ) {
	LTRACE( "Extension", "On JS API request: name=%s, params=%s", req.name.c_str(), req.params.c_str() );
	shell::JsRequestCommand *evt = new shell::JsRequestCommand();
	evt->set_browserid( req.browserId );
	evt->set_queryid( req.queryId );
	evt->set_params( req.params );

	shell::Response *res = new shell::Response();
	res->set_allocated_jsrequest( evt );
	enqueueMsg( res );
	return true;
}

//	Browser API callbacks
bool Extension::onKey( int browserID, int keycode, bool isUp ) {
	//	Fill key event response
	shell::BrowserKeyEvent *keyEvt = new shell::BrowserKeyEvent();
	keyEvt->set_browserid(browserID);
	keyEvt->set_code(keycode);
	keyEvt->set_isup(isUp);

	shell::Response *res = new shell::Response();
	res->set_allocated_bkey(keyEvt);
	enqueueMsg( res );

	return false;
}

void Extension::onWebLog( int browserID, const tvd::browser::WebLog &logData ) {
	//	Fill browser weblog data
	shell::BrowserWebLog *webLog = new shell::BrowserWebLog();
	webLog->set_level( logData.level );
	webLog->set_line( logData.line );
	webLog->set_source( logData.source );
	webLog->set_message( logData.message );

	shell::BrowserWebLogEvent *evt = new shell::BrowserWebLogEvent();
	evt->set_browserid(browserID);
	evt->set_allocated_logdata(webLog);

	shell::Response *res = new shell::Response();
	res->set_allocated_bweblog( evt );
	enqueueMsg( res );
}

void Extension::onClosed( int browserID, const std::string &error ) {
	shell::BrowserClosedEvent *evt = new shell::BrowserClosedEvent();
	evt->set_browserid(browserID);
	evt->set_error(error);

	shell::Response *res = new shell::Response();
	res->set_allocated_bclosed( evt );
	enqueueMsg( res );
}

void Extension::onLaunched( int browserID ) {
	shell::BrowserLaunchedEvent *evt = new shell::BrowserLaunchedEvent();
	evt->set_browserid(browserID);

	shell::Response *res = new shell::Response();
	res->set_allocated_blaunched( evt );
	enqueueMsg( res );
}

void Extension::onLoadProgress( int browserID ) {
	shell::BrowserLoadedEvent *evt = new shell::BrowserLoadedEvent();
	evt->set_browserid(browserID);

	shell::Response *res = new shell::Response();
	res->set_allocated_bloaded( evt );
	enqueueMsg( res );
}

//	Socket aux
Endpoint Extension::getEndpoint() {
	std::string remotePort = _systemApi->getCmdSwitch(URL_PARAMETER);
#if USE_TCP_SOCKETS
	return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::loopback(), _port);
#else
	return boost::asio::local::stream_protocol::endpoint(remotePort);
#endif
}

void Extension::onConnect( const boost::system::error_code &error ) {
	if (error) {
		_maxConnectRetry--;
		if (_maxConnectRetry < 0) {
			LERROR("Extension", "Couldn't connect. Timeout expired!");
			_io.stop();
		}
		else {
			LERROR("Extension", "Couldn't connect. Retrying in %dms", _retryConnectTimeout);
			boost::this_thread::sleep( boost::posix_time::milliseconds( _retryConnectTimeout ) );
			_socket->async_connect(
				getEndpoint(),
				boost::bind(&Extension::onConnect,this,boost::asio::placeholders::error)
			);
		}
	}
	else {
		tryRead();
	}
}

void Extension::send( const util::Buffer *msg ) {
	try {
		boost::asio::write(
			*_socket,
			boost::asio::buffer(msg->data(),msg->length()),
			boost::asio::transfer_all()
		);
	} catch( ... ) {
		LWARN("Extension", "Error sending data: len=%d", msg->length());
		shutdown();
	}
}

void Extension::enqueueMsg( shell::Response *res ) {
	_io.post(boost::bind(&Extension::sendMsg, this, res));
}

bool Extension::sendMsg( shell::Response *res ) {
	bool result;
	util::DWORD s = res->ByteSize()+MESSAGE_HEADER;
	_dataWrite->resize( s );
	PUT_DWORD(s,_dataWrite->bytes());

	//	Serialize
	if (res->SerializeToArray( _dataWrite->bytes()+MESSAGE_HEADER, _dataWrite->capacity()-MESSAGE_HEADER )) {
		if (res->data_case() != shell::Response::kPingAck) {
			LTRACE("Extension", "Send response: type=%d, data=%d", res->data_case(), _dataWrite->length() );
		}
		send( _dataWrite );
		result=true;
	}
	else {
		LWARN("Extension", "Cannot serialize response" );
		result=false;
	}

	delete res;
	return result;
}

void Extension::tryRead() {
	LTRACE("Extension", "Try read: buffer=%d", _dataRead->capacity() );
	_socket->async_read_some(
		boost::asio::buffer(_dataRead->bytes(), _dataRead->capacity()),
		boost::bind(
			&Extension::onRead,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void Extension::onRead( const boost::system::error_code& error, size_t transferred ) {
	if (error) {
		//	Error receiving data
		LWARN("Extension", "Read received error from socket: msg=%s", error.message().c_str());
		_io.stop();
	} else {
		if (transferred > 0) {
			//LTRACE("Extension", "Bytes recieved: %d", transferred);
			_dataRead->resize(transferred);
			assembleMessage( _dataRead );
		}

		tryRead();
	}
}

void Extension::assembleMessage( util::Buffer *b ) {
	size_t offset  = 0;
	size_t msgSize;
	size_t need;
	size_t rcvLen=b->length();

	while (offset < rcvLen) {
		size_t sLen=_msg->length();

		//	How many bytes need for complete message?
		if (sLen > MESSAGE_HEADER) {
			//	Stored bytes already have header
			msgSize = MSG_SIZE(_msg->bytes());
			need    = msgSize - sLen;
		}
		else if (MESSAGE_HEADER-sLen <= (rcvLen-offset)) {
			//	Stored bytes don't have the header, but with bytes received complete header!
			_msg->append( b->bytes()+offset, MESSAGE_HEADER-sLen );
			offset += MESSAGE_HEADER-sLen;
			sLen    = MESSAGE_HEADER;
			msgSize = MSG_SIZE(_msg->bytes());
			need    = msgSize-MESSAGE_HEADER;
		}
		else {
			//	Can't complete the header
			_msg->append( b->bytes()+offset, rcvLen-offset );
			offset = rcvLen;
			continue;
		}

		//	Copy bytes
		size_t rest = (rcvLen-offset);
		size_t copy = (rest > need) ? need : rest;

		// LINFO("Extension", "rcvLen=%d, sLen=%d, msgSize=%d, need=%d, rest=%d, copy=%d",
		// 	rcvLen, sLen, msgSize, need, rest, copy);

		_msg->append( b->bytes()+offset, copy );
		offset += copy;

		//	Is message complete?
		if (msgSize == _msg->length()) {
			//	Parse message
			shell::Request req;
			if (req.ParseFromArray( _msg->bytes()+MESSAGE_HEADER, _msg->length()-MESSAGE_HEADER)) {
				processRequest( &req );
			}
			_msg->resize(0);
		}
	}
}

#define VALIDATE_FIELD(f)	  \
	if (!req->has_##f()) {	LERROR("Extension", "Invalid parameters: type=%d", req->data_case() ); return; }

void Extension::processRequest( shell::Request *req ) {
	if (!req->has_ping()) {
		LTRACE( "Extension", "Process message: type=%d", req->data_case() );
	}

	switch (req->data_case()) {
		case shell::Request::kPing: {
			VALIDATE_FIELD(ping);

			//	Fill ping response
			shell::PingCommand *ping = new shell::PingCommand();
			ping->set_id(req->ping().id());

			//	Fill response
			shell::Response *res = new shell::Response();
			res->set_allocated_pingack( ping );
			sendMsg( res );

			break;
		}
		case shell::Request::kExit: {
			VALIDATE_FIELD(exit);
			shutdown();
			break;
		}
		case shell::Request::kLaunchBrowser: {
			VALIDATE_FIELD(launchbrowser);
			tvd::browser::Options opts;
			const shell::LaunchBrowserOptions &reqOpts = req->launchbrowser().options();
			opts.initJS = reqOpts.js();
			opts.transparentBg = reqOpts.bgtransparent();
			opts.needFocus = reqOpts.focus();
			opts.visible = reqOpts.visible();
			opts.zIndex = reqOpts.zindex();
			opts.enableMouse = reqOpts.enablemouse();
			opts.enableGeolocation = reqOpts.enablegeolocation();
			const shell::Rect &bounds = reqOpts.bounds();
			opts.bounds.x = bounds.x();
			opts.bounds.y = bounds.y();
			opts.bounds.w = bounds.w();
			opts.bounds.h = bounds.h();
			const google::protobuf::RepeatedPtrField<std::string> &plugins = reqOpts.plugins();
			opts.plugins.insert(opts.plugins.end(), plugins.begin(), plugins.end());
			opts.extraUA = reqOpts.extraua();

			LDEBUG("Extension", "launchBrowser: url=%s, opts=(js=%s, bg_transparent=%d, focus=%d, zIndex=%d)",
				req->launchbrowser().url().c_str(),
				opts.initJS.c_str(),
				opts.transparentBg,
				opts.needFocus,
				opts.zIndex
			);

			if (!_browserApi->launchBrowser( req->launchbrowser().browserid(), req->launchbrowser().url(), opts )) {
				LERROR( "Extension", "Cannot launch browser: url=%s", req->launchbrowser().url().c_str() );
				onClosed( req->launchbrowser().browserid(), "Cannot launch browser" );
			}
			break;
		}
		case shell::Request::kCloseBrowser: {
			VALIDATE_FIELD(closebrowser);
			LDEBUG("Extension", "closeBrowser: browserID=%d", req->closebrowser().browserid() );
			_browserApi->closeBrowser(req->closebrowser().browserid());
			break;
		}
		case shell::Request::kShowBrowser: {
			VALIDATE_FIELD(showbrowser);
			LDEBUG("Extension", "showBrowser: browserID=%d show=%d", req->showbrowser().browserid(), req->showbrowser().show() );
			_browserApi->showBrowser(req->showbrowser().browserid(), req->showbrowser().show(), req->showbrowser().focus());
			break;
		}
		case shell::Request::kJsResponse: {
			VALIDATE_FIELD(jsresponse);
			tvd::web::Response jsResponse;
			const shell::JsResponseCommand &res = req->jsresponse();
			jsResponse.browserId = res.browserid();
			jsResponse.queryId = res.queryid();
			jsResponse.isSignal = res.issignal();
			jsResponse.error = res.error();
			jsResponse.params = res.params();
			if (!_webApi->executeCallback( jsResponse )) {
				tvd::browser::WebLog err;
				err.level = 2; // error
				err.line = __LINE__;
				err.source = __FILE__;
				err.message = "Cannot execute callback";
				onWebLog( jsResponse.browserId, err );
			}
			break;
		}
		case shell::Request::kVersion: {
			VALIDATE_FIELD(version);

			{	//	Send version
				//	Fill version response
				shell::VersionCommand *ver = new shell::VersionCommand();
				ver->set_major(MAJOR_VERSION);
				ver->set_minor(MINOR_VERSION);

				//	Fill response
				shell::Response *res = new shell::Response();
				res->set_allocated_version( ver );
				sendMsg( res );
			}

			//	Handle response
			if (req->version().major() != MAJOR_VERSION) {
				LERROR( "Extension", "Invalid version!" );
				shutdown();
			}
			break;
		}
		default: {
			LERROR( "Extension", "Cannot process request: invalid request" );
			return;
		};
	};
}

}

