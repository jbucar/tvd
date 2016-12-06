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
#include "connector.h"
#include "generated/config.h"
#include "handler/messagehandler.h"
#include "handler/keepalivehandler.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>

#define TCP_PORT 55555

namespace connector {

Connector::Connector( const std::string &file, bool isServer/*=false*/ )
	: _endPointName(file),
	  _socket(new Socket(_io)),
	  _dataIn( 1024 ),
	  _keepAliveTimer(_io),
	  _acceptTimer(_io)
{
#if USE_TCP_SOCKETS
	_port = TCP_PORT;
#endif
	_isServer            = isServer;
	_keepAliveHandler    = NULL;
	_isConnected         = false;

	//	Connect timeout
	_connectTimeout      = CONNECT_TIMEOUT;
	_retryConnectTimeout = CONNECT_TIMEOUT_RETRY;
	_maxConnectRetry     = CONNECT_TIMEOUT_MAX_RETRY;

	//	Keep alive
	_keepAliveInterval   = KEEP_ALIVE_INTERVAL;
	_maxKeepAliveRetry   = KEEP_ALIVE_MAX_RETRY;
	_keepAlive           = 0;

	LINFO("Connector", "this(%p) Constructor: file=%s, mode=%d", this, file.c_str(), isServer);
	_thread = boost::thread( boost::bind( &Connector::connectorThread, this ) );
}

Connector::~Connector()
{
	LINFO("Connector", "this(%p) Destructor begin", this );
	_io.stop();
	_thread.join();

	BOOST_FOREACH( const HandlerElement &elem, _handlers ) {
		delete elem.second;
	}

	//	Remove endpoint in case we are the server
	if (_isServer) {
		std::remove(_endPointName.c_str());
	}

	LINFO("Connector", "this(%p) Destructor end", this );
}

void Connector::handle_accept(AcceptorPtr /*acceptor*/,
	const boost::system::error_code& error)
{
	if (!error) {
		_acceptTimer.cancel();
		connected();
		tryRead();
	}
	else {
		_io.stop();
	}
}

void Connector::onAcceptExpired( const boost::system::error_code& error ) {
	if (!error) {
		LWARN("Connector", "this(%p) accept timer expired: timer=%d", this, _connectTimeout);
		connectTimeout();
	}
}

void Connector::setRetryConnectTimeout( int timeout ) {
	_retryConnectTimeout = timeout;
}

void Connector::setMaxConnectRetry( int maxRetry ) {
	_maxConnectRetry = maxRetry;
}

void Connector::setConnectTimeout( int timeout ) {
	_connectTimeout = timeout;
}

Connector::Endpoint Connector::getEndpoint() {
#if USE_TCP_SOCKETS
	return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::loopback(), _port);
#else
	return boost::asio::local::stream_protocol::endpoint(_endPointName);
#endif
}

void Connector::handle_connect(const boost::system::error_code& error) {
	if (error) {
		_maxConnectRetry--;
		if (_maxConnectRetry < 0) {
			LERROR("Connector", "this(%p) couldn't connect. Timeout expired!", this);
			connectTimeout();
		}
		else {
			LERROR("Connector", "this(%p) couldn't connect. Retrying in %dms", this, _retryConnectTimeout);
			boost::this_thread::sleep( boost::posix_time::milliseconds( _retryConnectTimeout ) );
			_socket->async_connect(getEndpoint(),
				boost::bind(&Connector::handle_connect, this,
					boost::asio::placeholders::error));
		}
	}
	else {
		connected();
		tryRead();
	}
}

void Connector::tryRead( void ) {
	_socket->async_read_some(
		boost::asio::buffer(_dataIn.bytes(), _dataIn.capacity()),
		boost::bind(
			&Connector::handle_read,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void Connector::handle_read( const boost::system::error_code& error, size_t transferred ) {
	if (error) {
		//	Error receiving data
		LINFO("Connector", "this(%p) Thread received error from socket: msg=%s", this, error.message().c_str());
		_io.stop();
	} else {
		LTRACE("Connector", "this(%p) Bytes transferred: %d", this, transferred);

		_dataIn.resize(transferred);
		assembleMessage(_dataIn);  // Assemble the received message

		tryRead();
	}
}

void Connector::send( const util::Buffer *msg ) {
	try {
		boost::asio::write(
			*_socket,
			boost::asio::buffer(msg->data(),msg->length()),
			boost::asio::transfer_all()
		);
	} catch( ... ) {
		LWARN("Connector", "this(%p) error sending data", this);
	}
}

void Connector::assembleMessage( const util::Buffer &b ) {
	int offset  = 0;
	size_t msgSize;
	int need;
	int rcvLen=b.length();

	while (offset < rcvLen) {
		int sLen=_msg.length();

		//	How many bytes need for complete message?
		if (sLen > MESSAGE_HEADER) {
			//	Stored bytes already have header
			msgSize = MSG_SIZE(_msg.bytes());
			need    = msgSize - sLen;
		}
		else if (MESSAGE_HEADER-sLen <= (rcvLen-offset)) {
			//	Stored bytes don't have the header, but with bytes received complete header!
			_msg.append( b.bytes()+offset, MESSAGE_HEADER-sLen );
			offset += MESSAGE_HEADER-sLen;
			sLen    = MESSAGE_HEADER;
			msgSize = MSG_SIZE(_msg.bytes());
			need    = msgSize-MESSAGE_HEADER;
		}
		else {
			//	Can't complete the header
			_msg.append( b.bytes()+offset, rcvLen-offset );
			offset = rcvLen;
			continue;
		}

		DTV_ASSERT( need >= 0 );

		//	Copy bytes
		int rest = (rcvLen-offset);
		int copy = (rest > need) ? need : rest;

		LTRACE("Connector", "this(%p) rcvLen=%d, sLen=%d, msgSize=%d, need=%d, rest=%d, copy=%d",
			this, rcvLen, sLen, msgSize, need, rest, copy);

		_msg.append( b.bytes()+offset, copy );
		offset += copy;

		//	Is message complete?
		if (msgSize == _msg.length()) {
			messageReceived(&_msg);
			_msg.resize(0);
		}
	}
}

void Connector::messageReceived( util::Buffer *m ) {
	util::BYTE type = MSG_TYPE(m->bytes());

	LTRACE("Connector", "this(%p) Received a message: type=%d", this, type);

	Handlers::iterator cb = _handlers.find( (messages::type)type );
	if (cb != _handlers.end()) {
		(*cb).second->process( m );
	} else {
		LWARN("Connector", "this(%p) Received unhandled message type: type=%02x", this, type);
	}
}

void Connector::addHandler( messages::type type, MessageHandler *handler ) {
	_io.post( boost::bind( &Connector::addHandlerImpl, this, type, handler) );
}

void Connector::addHandler( MessageHandler *handler ) {
	if (handler) {
		_io.post( boost::bind( &Connector::addHandlerImpl, this, handler->type(), handler) );
	}
}

void Connector::addHandlerImpl( messages::type type, MessageHandler *handler ) {
	Handlers::iterator it=_handlers.find(type);
	if (it != _handlers.end()) {
		delete (*it).second;
		if (!handler) {
			_handlers.erase(it);
		}
	}

	if (handler) {
		if (!_dispCallback.empty()) {
			handler->setDispatcher( _dispCallback );
		}
		_handlers[type] = handler;
	}
}

void Connector::connectorThread( void ) {
	LINFO("Connector", "this(%p) Connector thread started: mode=%d", this, _isServer);

	if (_isServer) {
		AcceptorPtr acceptor(new Acceptor(_io, getEndpoint()));
		acceptor->async_accept(*_socket,
		    boost::bind(&Connector::handle_accept, this, acceptor,
				    boost::asio::placeholders::error));

		    //	Launch timer ....
		    launchTimer( _acceptTimer, _connectTimeout, &Connector::onAcceptExpired );
	} else {
		_socket->async_connect(getEndpoint(),
			boost::bind(&Connector::handle_connect, this,
				boost::asio::placeholders::error));
	}

	try {
		_io.run();
	}
	catch (std::exception& e) {
		LWARN("Connector", "this(%p) Exception: %s", this, e.what());
	}

	disconnected();
	LINFO("Connector", "this(%p) Connector Thread Stopped", this);
}

void Connector::setDispatcher( const DispatcherCallback &callback ) {
	_dispCallback = callback;
}

void Connector::onConnect( const Callback &callback ) {
	_onConnect = callback;
}

void Connector::connected( void ) {
	//	--Connector thread
	if (!_isConnected) {
		LINFO("Connector", "this(%p) Connected", this );
		dispatch( _onConnect );
		_isConnected = true;

		//	Add Keep Alive handler
		_keepAliveHandler = new KeepAliveHandler();
		_keepAliveHandler->onKeepAliveEvent( boost::bind(&Connector::onKeepAliveEvent, this, _1) );
		addHandler( _keepAliveHandler );

		if (_isServer) {
			LDEBUG("Connector", "this(%p) Launching keep alive timer: interval=%d, retry=%d",
				this, _keepAliveInterval, _maxKeepAliveRetry);

			//	Launch keep alive timer ....
			_keepAlive=0;
			launchTimer( _keepAliveTimer, _keepAliveInterval, &Connector::onKeepAliveIntervalExpired );
		}
	}
}

void Connector::launchTimer( boost::asio::deadline_timer &timer, int ms, TimerCallback fnc ) {
	timer.expires_from_now( boost::posix_time::milliseconds(ms) );
	timer.async_wait( boost::bind( fnc, this, boost::asio::placeholders::error ) );
}

void Connector::onKeepAliveEvent( connector::KeepAliveData * /*ka*/ ) {
	//	--Connector thread
	if (_isServer) {
		_keepAlive=0;
	}
	else {
		LTRACE("Connector", "this(%p) Received keep alive", this);

		//	Return data;
		_keepAliveHandler->send(this);
	}
}

void Connector::onKeepAliveIntervalExpired( const boost::system::error_code& error ) {
	//	--Connector thread
	if (!error) {
		LTRACE("Connector", "this(%p) Sending keep alive", this);

		//	Check keep alive data
		if (_keepAlive >= _maxKeepAliveRetry) {
			timeout();
		}
		else {
			_keepAlive++;
			_keepAliveHandler->send(this);
		}

		//	Re-launch timer
		launchTimer( _keepAliveTimer, _keepAliveInterval, &Connector::onKeepAliveIntervalExpired );
	}
}

void Connector::onDisconnect( const Callback &callback ) {
	_onDisconnect = callback;
}

void Connector::disconnected( void ) {
	//	--Connector thread
	if (_isConnected) {
		LINFO("Connector", "this(%p) Disconnected", this);

		//	Send exit to all handlers
		BOOST_FOREACH( const HandlerElement &elem, _handlers ) {
			elem.second->notifyDisconnect();
		}

		//	Send global exit
		dispatch( _onDisconnect );
		_isConnected = false;
	}
}

void Connector::connectTimeout() {
	//	--Connector thread
	LINFO("Connector", "this(%p) Connect timeout expired", this);
	dispatch( _onConnectTimeout );
	_io.stop();
}

void Connector::onConnectTimeout( const Callback &callback ) {
	_onConnectTimeout = callback;
}

void Connector::timeout() {
	//	--Connector thread
	LINFO("Connector", "this(%p) Keep alive Timeout!!!", this);
	dispatch( _onTimeout );
	_io.stop();
}

void Connector::onTimeout( const Callback &callback ) {
	_onTimeout = callback;
}

//	Keep alive timeout
void Connector::setKeepAliveInterval( int timeout ) {
	_keepAliveInterval = timeout;
}

void Connector::setMaxKeeyAliveRetry( int timeout ) {
	_maxKeepAliveRetry = timeout;
}

void Connector::dispatch( const Callback &callback ) {
	if (!callback.empty()) {
		if (_dispCallback.empty()) {
			callback();
		}
		else {
			_dispCallback( callback );
		}
	}
	else {
		LINFO( "Connector", "this(%p) callback empty" );
	}
}

}
