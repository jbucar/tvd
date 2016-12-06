#pragma once

#include <htmlshelllib/library.h>
#include <util/types.h>
#include <boost/thread.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/signals2.hpp>
#include <string>

namespace ar {
namespace edu {
namespace unlp {
namespace info {
namespace lifia {
namespace tvd {
namespace htmlshell {
	class Request;
	class Response;
}}}}}}}

namespace tvd {
	class Var;
	namespace web {
		typedef struct RequestS Request;
		class ApiInterface_1_0;
	}
	namespace browser {
		typedef struct WebLogS WebLog;
		class ApiInterface_1_0;
		class Listener;
	}
	namespace system {
		class ApiInterface_1_0;
	}
	template<typename T> class NotificationHandlerImpl;
}

namespace util {
	class Buffer;
}

namespace remote {

namespace shell = ar::edu::unlp::info::lifia::tvd::htmlshell;

#if defined(_WIN32) || defined(__APPLE__)
	#define USE_TCP_SOCKETS 1
	typedef boost::asio::ip::tcp::socket   Socket;
	typedef boost::asio::ip::tcp::acceptor Acceptor;
	typedef boost::asio::ip::tcp::endpoint Endpoint;
#else
	#define USE_TCP_SOCKETS 0
	typedef boost::asio::local::stream_protocol::socket   Socket;
	typedef boost::asio::local::stream_protocol::acceptor Acceptor;
	typedef boost::asio::local::stream_protocol::endpoint Endpoint;
#endif

typedef boost::shared_ptr<Socket> SocketPtr;
typedef boost::shared_ptr<Acceptor> AcceptorPtr;

class Extension : public tvd::HtmlShellLibrary {
public:
	Extension();
	virtual ~Extension();

	//	Browser API
	bool onKey( int browserID, int keycode, bool isUp );
	void onWebLog( int browserID, const tvd::browser::WebLog &logData );
	void onClosed( int browserID, const std::string &error );
	void onLaunched( int browserID );
	void onLoadProgress( int browserID );

protected:
	// tvd::HtmlShellLibrary methods:
	virtual bool init();
	virtual void fin();

	//	Web API
	bool onRequestJS( const tvd::web::Request &req );

	void shutdown();

	//	Socket aux
	bool sendMsg( shell::Response *res );
	void enqueueMsg( shell::Response *res );
	Endpoint getEndpoint();
	void tryRead();
	void send( const util::Buffer *msg );
	void onConnect( const boost::system::error_code& err );
	void assembleMessage( util::Buffer *b );
	void onRead( const boost::system::error_code& error, size_t transferred );
	void processRequest( shell::Request *req );

	//	Thread
	void threadLoop();

private:
	tvd::web::ApiInterface_1_0 *_webApi;
	tvd::system::ApiInterface_1_0 *_systemApi;

	tvd::browser::ApiInterface_1_0 *_browserApi;
	tvd::browser::Listener *_listener;

	//	Socket implementation
	int _retryConnectTimeout;
	int _maxConnectRetry;
	util::Buffer *_dataRead;
	util::Buffer *_msg;
	util::Buffer *_dataWrite;
	boost::asio::io_service _io;
	SocketPtr _socket;

	//	Thread
	boost::thread _thread;
};

}

