/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include <map>
#include <string>
#include <boost/thread.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <util/buffer.h>
#include "handler/types.h"

#if defined(_WIN32) || defined(__APPLE__)
	#define USE_TCP_SOCKETS 1
#else
	#define USE_TCP_SOCKETS 0
#endif

#define DEFAULT_CONNECTOR_FILE "/tmp/gingaconnector.socket"

namespace connector {

class Connector;
class MessageHandler;
class KeepAliveHandler;
class KeepAliveData;

class Connector {
public:
	Connector( const std::string &file, bool isServer );
	virtual ~Connector();

	void send( const util::Buffer *msg );
	void addHandler( messages::type type, MessageHandler *handler );
	void addHandler( MessageHandler *handler );

	//	Signals
	typedef boost::function<void ( void)> Callback;
	typedef boost::function<void (const Callback &)> DispatcherCallback;
	void setDispatcher( const DispatcherCallback &callback );

	void onConnect( const Callback &callback );
	void onDisconnect( const Callback &callback );
	void onConnectTimeout( const Callback &callback );
	void onTimeout( const Callback &callback );

	//	Client connect timeout
	void setRetryConnectTimeout( int retryTimeout );
	void setMaxConnectRetry( int maxRetry );

	//	Server connect timeout
	void setConnectTimeout( int timeout );

	//	Keep alive timeout
	void setKeepAliveInterval( int timeout );
	void setMaxKeeyAliveRetry( int timeout );

protected:
#if USE_TCP_SOCKETS
	typedef boost::asio::ip::tcp::socket   Socket;
	typedef boost::asio::ip::tcp::acceptor Acceptor;
	typedef boost::asio::ip::tcp::endpoint Endpoint;
#else
	typedef boost::asio::local::stream_protocol::socket   Socket;
	typedef boost::asio::local::stream_protocol::acceptor Acceptor;
	typedef boost::asio::local::stream_protocol::endpoint Endpoint;
#endif
	typedef boost::shared_ptr<Socket> SocketPtr;
	typedef boost::shared_ptr<Acceptor> AcceptorPtr;
	typedef std::map<enum messages::type, MessageHandler *> Handlers;
	typedef std::pair<enum messages::type, MessageHandler *> HandlerElement;

	void connectorThread( void );

	void handle_accept( AcceptorPtr acceptor, const boost::system::error_code& error );
	void onAcceptExpired( const boost::system::error_code& error );

	void handle_connect( const boost::system::error_code& err );

	void onKeepAliveEvent( KeepAliveData *ka );
	void onKeepAliveIntervalExpired( const boost::system::error_code& error );

	void handle_read( const boost::system::error_code& error, size_t transferred );
	void tryRead( void );
	void assembleMessage( const util::Buffer &b );
	void messageReceived( util::Buffer *msj );

	typedef void (Connector::*TimerCallback)( const boost::system::error_code& error );
	void launchTimer( boost::asio::deadline_timer &timer, int ms, TimerCallback );
	void addHandlerImpl( messages::type type, MessageHandler *handler );
	void connected();
	void disconnected();
	void connectTimeout();
	void timeout();
	void dispatch( const Callback &callback );

	Endpoint getEndpoint();

private:
	boost::thread _thread;
	boost::mutex _mutex;

	boost::asio::io_service _io;
	std::string _endPointName;
#if USE_TCP_SOCKETS
	unsigned short _port;
#endif
	SocketPtr _socket;
	Handlers _handlers;
	util::Buffer _dataIn;
	util::Buffer _msg;

	bool _isServer;
	bool _isConnected;

	//	Keep alvive
	KeepAliveHandler *_keepAliveHandler;
	int  _keepAliveInterval;
	int  _keepAlive;
	int  _maxKeepAliveRetry;
	boost::asio::deadline_timer _keepAliveTimer;

	//	Client
	int _retryConnectTimeout;
	int _maxConnectRetry;

	//	Server
	int _connectTimeout;
	boost::asio::deadline_timer _acceptTimer;

	//	Signals
	DispatcherCallback _dispCallback;
	Callback _onConnect, _onConnectTimeout, _onDisconnect, _onTimeout;
};

}

