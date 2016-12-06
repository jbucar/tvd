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
#include "../../src/connector.h"
#include "../../src/handler/keepalivehandler.h"
#include "../../src/handler/keyeventhandler.h"
#include "../../src/handler/keyregisterhandler.h"
#include "../../src/handler/editingcommandhandler.h"
#include "../../src/handler/videoresizehandler.h"
#include "../../src/handler/playvideohandler.h"
#include "../../src/handler/exitcommandhandler.h"
#include "../../src/handler/startcommandhandler.h"
#include "../../src/handler/canvashandler.h"
#include "util.h"
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <ctime>

namespace fs = boost::filesystem;
//	TODO: Test consistence

class DISABLED_ConnectorTest : public testing::Test {
public:
	DISABLED_ConnectorTest( void ) {
		_client = NULL;
		_server = NULL;
	}
	virtual ~DISABLED_ConnectorTest( void ) {}

	void onConnect( int event ) {
		_connect++;
		test::notify( (test::flags::type)event );
	}

	void onDisconnect( int event ) {
		_disconnect++;
		LTRACE("ConnectorTest", "onDisconnect=%d", _disconnect );
		test::notify( (test::flags::type)event );
	}

	void onConnectTimeout( int event ) {
		_connectTimeout++;
		LTRACE("ConnectorTest", "onConnectTimeout=%d", _connectTimeout );
		test::notify( (test::flags::type)event );
	}

	void onTimeout( int event ) {
		_timeout++;
		LTRACE("ConnectorTest", "onTimeout=%d", _timeout );
		test::notify( (test::flags::type)event );
	}

	connector::Connector *startServer( const std::string &pipe, int msConnectTimeout=5000 ) {
		connector::Connector *srv=new connector::Connector( pipe, true );
		srv->onConnect( boost::bind( &DISABLED_ConnectorTest::onConnect, this, test::flags::connector_ev_srv ) );
		srv->onConnectTimeout( boost::bind( &DISABLED_ConnectorTest::onConnectTimeout, this, test::flags::connector_ev_srv ) );
		srv->onDisconnect( boost::bind( &DISABLED_ConnectorTest::onDisconnect, this, test::flags::connector_ev_srv ) );
		srv->onTimeout( boost::bind( &DISABLED_ConnectorTest::onTimeout, this, test::flags::connector_ev_srv ) );
		srv->setConnectTimeout( msConnectTimeout );
		return srv;
	}

	connector::Connector *startClient( const std::string &pipe, int maxRetry=10, int timeout=50 ) {
		connector::Connector *cli = new connector::Connector( pipe, false );
		cli->setMaxConnectRetry( maxRetry );
		cli->setRetryConnectTimeout( timeout );
		cli->onConnect( boost::bind( &DISABLED_ConnectorTest::onConnect, this, test::flags::connector_ev_rcv ) );
		cli->onConnectTimeout( boost::bind( &DISABLED_ConnectorTest::onConnectTimeout, this, test::flags::connector_ev_rcv ) );
		cli->onDisconnect( boost::bind( &DISABLED_ConnectorTest::onDisconnect, this, test::flags::connector_ev_rcv ) );
		return cli;
	}

	void start( void ) {
		//	Setup server
		_server = startServer( _pipe );
		//	Setup client
		_client = startClient( _pipe );

		test::wait( test::flags::connector_ev_srv );
		test::wait( test::flags::connector_ev_rcv );
	}

	virtual void SetUp() {
		test::clean();
		fs::path tmp( fs::temp_directory_path() );
		tmp /= "connector_%%%%%%";
		_pipe = fs::unique_path(tmp).string();
		_connect=_disconnect=_connectTimeout=_timeout=0;
	}

	virtual void TearDown() {
		stop();
	}

	void stop() {
		stopClient();
		stopServer();
	}

	void stopServer() {
		if (_server) {
			//printf( "stop server\n" );
			delete _server;
			_server = NULL;
		}
	}

	void stopClient() {
		if (_client) {
			delete _client;
			_client = NULL;
			test::wait( test::flags::connector_ev_rcv );
			test::wait( test::flags::connector_ev_srv );
		}
	}

	std::string _pipe;
	int _connect, _disconnect, _connectTimeout, _timeout;
	connector::Connector *_server;
	connector::Connector *_client;
};

TEST_F( DISABLED_ConnectorTest, basic_connect ) {
	start();
	stop();
	ASSERT_TRUE( _connect == 2 );
	ASSERT_TRUE( _disconnect == 2 );
	ASSERT_TRUE( _connectTimeout == 0 );
	ASSERT_TRUE( _timeout == 0 );
}

TEST_F( DISABLED_ConnectorTest, client_connect_connectTimeout ) {
	int msTimeout = 50;
	int retry = 2;
	_server = startClient( _pipe, retry, msTimeout );
	boost::this_thread::sleep( boost::posix_time::milliseconds( (retry+50)*(msTimeout*2) ) );
	ASSERT_TRUE( _connect == 0 );
	ASSERT_TRUE( _disconnect == 0 );
	ASSERT_TRUE( _connectTimeout == 1 );
	ASSERT_TRUE( _timeout == 0 );
}

TEST_F( DISABLED_ConnectorTest, server_connect_connectTimeout ) {
	int msTimeout = 100;
	_server = startServer( _pipe, msTimeout );
	boost::this_thread::sleep( boost::posix_time::milliseconds( msTimeout*2 ) );
	ASSERT_TRUE( _connect == 0 );
	ASSERT_TRUE( _disconnect == 0 );
	ASSERT_TRUE( _connectTimeout == 1 );
	ASSERT_TRUE( _timeout == 0 );
}

TEST_F( DISABLED_ConnectorTest, send_without_connect ) {
	int msTimeout = 1000000;
	_server = startServer( _pipe, msTimeout );

	connector::ExitCommandHandler exit;
	exit.send( _server );
}

TEST_F( DISABLED_ConnectorTest, check_keep_alive ) {
	int keepInterval=500;
	start();

	//	Wait keep alive interval
	boost::this_thread::sleep( boost::posix_time::milliseconds( keepInterval*3 ) );

	stop();

	ASSERT_TRUE( _connect == 2 );
	ASSERT_TRUE( _disconnect == 2 );
	ASSERT_TRUE( _connectTimeout == 0 );
	ASSERT_TRUE( _timeout == 0 );
}

TEST_F( DISABLED_ConnectorTest, check_keep_alive_timeout ) {
	int keepInterval=500;
	start();

	//	Wait keep alive interval
	boost::this_thread::sleep( boost::posix_time::milliseconds( keepInterval*2 ) );

	_client->addHandler( connector::messages::keepAlive, NULL );

	//	Wait keep alive interval
	boost::this_thread::sleep( boost::posix_time::milliseconds( keepInterval*10 ) );

	stop();

	ASSERT_TRUE( _connect == 2 );
	ASSERT_TRUE( _disconnect == 2 );
	ASSERT_TRUE( _connectTimeout == 0 );
	ASSERT_TRUE( _timeout == 1 );
}

TEST_F( DISABLED_ConnectorTest, check_handler ) {
	start();
	_client->addHandler( NULL );
}

class DummyHandler : public connector::MessageHandler {
public:
	DummyHandler()
	: received (NULL)
	{
	}

	virtual ~DummyHandler() {
		if (received) {
			delete received;
		}
	}

	virtual void process (util::Buffer* message) {
		size_t msgSize = MSG_PAYLOAD_SIZE(message->bytes());
		util::BYTE *payload = MSG_PAYLOAD(message->bytes());

		if (received != NULL)
			delete received;
		received = new util::Buffer((const char*)payload, msgSize);
		test::notify( test::flags::connector_ev_data );
	}

	virtual connector::messages::type type() const {
		return connector::messages::keyRegister;
	}

	virtual void send (connector::Connector* conn, util::Buffer* msg) {
		util::Buffer buf;
		buf.copy(3, msg->bytes(), msg->length());
		MAKE_MSG(buf.bytes(), connector::messages::keyRegister, msg->length()+3);
		conn->send(&buf);
	}

	util::Buffer* received;
};

TEST_F( DISABLED_ConnectorTest, dummyMessageEmptyMessage ) {
	start();
	DummyHandler *dummy = new DummyHandler();

	_server->addHandler( dummy );

	util::Buffer message;
	dummy->send(_client, &message);
	test::wait(test::flags::connector_ev_data);

	EXPECT_EQ(dummy->received->length(), 0);
}

TEST_F( DISABLED_ConnectorTest, dummyOneCharMessage) {
	start();
	DummyHandler *dummy = new DummyHandler();

	_server->addHandler(dummy);

	util::Buffer message;
	message.append("\xA5", 1);
	dummy->send(_client, &message);
	test::wait(test::flags::connector_ev_data);

	EXPECT_EQ(dummy->received->length(), 1);
	EXPECT_EQ(dummy->received->bytes()[0], 0xA5 );

	message.copy("\x5A", 1);
	dummy->send(_client, &message);
	test::wait(test::flags::connector_ev_data);

	EXPECT_EQ(dummy->received->length(), 1);
	EXPECT_EQ(dummy->received->bytes()[0], 0x5A);
}

TEST_F( DISABLED_ConnectorTest, dummyOneByteLengthLimit) {
	start();
	DummyHandler *dummy = new DummyHandler();
	char text[258];
	memset(text, 0xA5, 258);

	_server->addHandler(dummy);
	for (int i = 253; i <= 258; ++i) {
		util::Buffer message(text, i);
		dummy->send(_client, &message);
		test::wait(test::flags::connector_ev_data);
		EXPECT_EQ(dummy->received->length(), i);
		EXPECT_TRUE(memcmp(dummy->received->bytes(), text, i) == 0);
	}
}

/*TEST_F( DISABLED_ConnectorTest, dummyTwoBytesLengthLimit) {
	start();
	DummyHandler *dummy = new DummyHandler();
	char *text;
	text = new char[0xFFFF];
	memset(text, 0xA5, 0xFFFF);

	_server->addHandler(dummy);
	for (int i = 0xFFF0; i < 0xFFFF-MESSAGE_HEADER; ++i) {
		printf("Testing with length %d\n", i);
		util::Buffer message(text, i);
		dummy->send(_client, &message);
		test::wait(test::flags::connector_ev_data);
		EXPECT_EQ(dummy->received->length(), i);
		EXPECT_TRUE(memcmp(dummy->received->buffer(), text, i) == 0);
	}
	delete [] text;
}*/

static void keyRegisterBasic( std::set< util::key::type > *keys, const connector::KeyRegisterDataPtr &m ) {
	*keys = *m.get();
	test::notify( test::flags::connector_ev_data );
}

TEST_F( DISABLED_ConnectorTest, key_register_basic ) {
	start();

	std::set< util::key::type > rcv;
	connector::KeyRegisterHandler *keyHandler = new connector::KeyRegisterHandler();
	keyHandler->onKeyEvent( boost::bind( &keyRegisterBasic, &rcv, _1 ) );
	_server->addHandler( keyHandler );

	connector::KeyRegisterHandler key;
	std::set< util::key::type > keys;
	keys.insert( util::key::number_1 );
	keys.insert( util::key::number_5 );
	keys.insert( util::key::asterisk );
	key.send( _client, keys );
	test::wait( test::flags::connector_ev_data );

	ASSERT_TRUE( rcv.size() == 3 );
	ASSERT_TRUE( rcv.find( util::key::number_1 ) != rcv.end() );
	ASSERT_TRUE( rcv.find( util::key::number_5 ) != rcv.end() );
	ASSERT_TRUE( rcv.find( util::key::asterisk ) != rcv.end() );
}

TEST_F( DISABLED_ConnectorTest, key_register_all ) {
	start();

	std::set< util::key::type > rcv;
	connector::KeyRegisterHandler *keyHandler = new connector::KeyRegisterHandler();
	keyHandler->onKeyEvent( boost::bind( &keyRegisterBasic, &rcv, _1 ) );
	_server->addHandler( keyHandler );

	connector::KeyRegisterHandler key;
	std::set< util::key::type > keys;
#define SAVE_KEY( c, n, u ) keys.insert( static_cast<util::key::type>(util::key::c) );
	UTIL_KEY_LIST(SAVE_KEY);
#undef SAVE_KEY
	key.send( _client, keys );
	test::wait( test::flags::connector_ev_data );

	ASSERT_TRUE( rcv.size() == util::key::LAST-1 );
}

static void keyEventBasic( util::key::type *key, bool *isUp, util::key::type dataKey, bool dataState ) {
	*key = dataKey;
	*isUp = dataState;
	test::notify( test::flags::connector_ev_data );
}

TEST_F( DISABLED_ConnectorTest, key_event_basic ) {
	start();

	util::key::type rcv = util::key::null;
	bool isUp = false;
	connector::KeyEventHandler *keyHandler = new connector::KeyEventHandler();
	keyHandler->onKeyEvent( boost::bind( &keyEventBasic, &rcv, &isUp, _1, _2 ) );
	_server->addHandler( keyHandler );
	connector::KeyEventHandler key;

	key.send( _client, util::key::number_1, true );
	test::wait( test::flags::connector_ev_data );
	ASSERT_TRUE( rcv == util::key::number_1 );
	ASSERT_TRUE( isUp == true );

	key.send( _client, util::key::number_5, false );
	test::wait( test::flags::connector_ev_data );
	ASSERT_TRUE( rcv == util::key::number_5 );
	ASSERT_TRUE( isUp == false );

	key.send( _client, util::key::small_a, true );
	test::wait( test::flags::connector_ev_data );
	ASSERT_TRUE( rcv == util::key::small_a );
	ASSERT_TRUE( isUp == true );
}


static void onStartPresentation( bool *start, bool data ) {
	*start = data;
	test::notify( test::flags::connector_ev_data );
}

TEST_F( DISABLED_ConnectorTest, start_event_basic ) {
	start();

	bool start;

	{	//	Setup server
		connector::StartCommandHandler *handler = new connector::StartCommandHandler();
		handler->onStartPresentationEvent( boost::bind( &onStartPresentation, &start, _1 ) );
		_server->addHandler( handler );
	}

	connector::StartCommandHandler handler;

	start=false;
	handler.send( _client, true );
	test::wait( test::flags::connector_ev_data );
	ASSERT_TRUE( start == true );

	start=true;
	handler.send( _client, false );
	test::wait( test::flags::connector_ev_data );
	ASSERT_TRUE( start == false );
}

struct VideoEvent {
	size_t id;
	float x, y, w, h;
};

void videoResize_handler( struct VideoEvent *ev, size_t id, const boost::shared_ptr<connector::AVDescriptor<float> > &descriptor ) {
	ev->id = id;
	ev->x = descriptor->getX();
	ev->y = descriptor->getY();
	ev->w = descriptor->getWidth();
	ev->h = descriptor->getHeight();
	test::notify( test::flags::connector_ev_data );
}

TEST_F( DISABLED_ConnectorTest, video_basic ) {
	start();

	struct VideoEvent ev;
	connector::VideoResizeHandler<float>* videoResizeHandler = new connector::VideoResizeHandler<float>();
	videoResizeHandler->onVideoResizeEvent( boost::bind( &videoResize_handler, &ev, _1, _2 ) );
	_server->addHandler( videoResizeHandler);

	connector::VideoResizeHandler<float> videoResize;
	connector::AVDescriptor<float> *descriptor = new connector::AVDescriptor<float>(0.25,0.25,0.50,0.50);
	size_t id=5;
	videoResize.send( _client, id, descriptor);
	test::wait( test::flags::connector_ev_data );

	ASSERT_TRUE( ev.id == 5 );
	ASSERT_TRUE( ev.x == 0.25 );
	ASSERT_TRUE( ev.y == 0.25 );
	ASSERT_TRUE( ev.w == 0.50 );
	ASSERT_TRUE( ev.h == 0.50 );
}

void exit_handler( int *counter ) {
	(*counter)++;
	test::notify( test::flags::connector_ev_data );
}
/*
TEST_F( DISABLED_ConnectorTest, exit_basic ) {
	start();

	int counter=0;
	connector::ExitCommandHandler* exitHandler = new connector::ExitCommandHandler();
	exitHandler->onExitEvent( boost::bind( &exit_handler, &counter ) );
	_server->addHandler( exitHandler);

	connector::ExitCommandHandler exit;
	exit.send( _client );
	test::wait( test::flags::connector_ev_data );

	ASSERT_TRUE( counter == 1 );
}*/

void editingCommandHandler_callback (connector::EditingCommandData *where, const connector::EditingCommandDataPtr received) {
	where->copyFrom( received.get() );
	test::notify( test::flags::connector_ev_data);
}

TEST_F( DISABLED_ConnectorTest, editingcommand_basic ) {
	const char privateData[] = "LIFIA_TVD,datos privados";
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;
	connector::EditingCommandData    ec_sent(0x05); // Add document
	ec_sent.setEventID(1);
	ec_sent.setPrivateDataPayload((const unsigned char*)privateData, sizeof(privateData)/sizeof(char));
	ecSender.send( _client, &ec_sent );
	test::wait ( test::flags::connector_ev_data );

	ASSERT_TRUE( ec_received.getEventID() == 1 );
	ASSERT_TRUE( ec_received.getCommandTag() == 0x05 );
	ASSERT_TRUE( ec_received.getNPT() == 0);
	ASSERT_TRUE( ec_received.getPayloadSize() == sizeof(privateData)/sizeof(char) );
	ASSERT_TRUE( std::string((const char*)ec_received.getPayload()) == std::string(privateData) );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_fullOneSection ) {
	const char privateData[241] = { 'a' }; // 241 bytes maximum in one section...
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;
	connector::EditingCommandData    ec_sent(0x05); // Add document
	ec_sent.setEventID(1);
	ec_sent.setPrivateDataPayload((const unsigned char*)privateData, sizeof(privateData)/sizeof(char));
	ecSender.send( _client, &ec_sent );
	test::wait ( test::flags::connector_ev_data );

	ASSERT_TRUE( ec_received.getEventID() == 1 );
	ASSERT_TRUE( ec_received.getCommandTag() == 0x05 );
	ASSERT_TRUE( ec_received.getNPT() == 0);
	ASSERT_TRUE( ec_received.getPayloadSize() == sizeof(privateData)/sizeof(char) );
	ASSERT_TRUE( std::string((const char*)ec_received.getPayload()) == std::string(privateData) );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_fullManySections ) {
	const char privateData[241*128] = { 'a' }; // 241 bytes maximum in one section...
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;
	connector::EditingCommandData    ec_sent(0x05); // Add document
	ec_sent.setEventID(1);
	ec_sent.setPrivateDataPayload((const unsigned char*)privateData, sizeof(privateData)/sizeof(char));
	ecSender.send( _client, &ec_sent );
	test::wait ( test::flags::connector_ev_data );

	ASSERT_TRUE( ec_received.getEventID() == 1 );
	ASSERT_TRUE( ec_received.getCommandTag() == 0x05 );
	ASSERT_TRUE( ec_received.getNPT() == 0);
	ASSERT_TRUE( ec_received.getPayloadSize() == sizeof(privateData)/sizeof(char) );
	ASSERT_TRUE( memcmp((const void*)ec_received.getPayload(), (const void*)privateData, ec_received.getPayloadSize()) == 0 );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_one_full_section ) {
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;
	const unsigned char array[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x00, 0x00
	};
	util::Buffer buffer( (const char*) array, sizeof(array)/sizeof(char), false );
	ecSender.send( _client, buffer );
	test::wait ( test::flags::connector_ev_data );

	ASSERT_TRUE( ec_received.getEventID() == 1 );
	ASSERT_TRUE( ec_received.getCommandTag() == 0x2D );
	ASSERT_TRUE( ec_received.getNPT() == 0 );
	ASSERT_TRUE( ec_received.getPayloadSize() == 0 );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_npt ) {
	const unsigned char privateData[] = "";
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	unsigned long long nptValues[] = {
		0x000000000ULL,
		0x000000001ULL,
		0x000000010ULL,
		0x0000000FEULL,
		0x0000000FFULL,
		0x000000100ULL,
		0x000000101ULL,
		0x00000FFFFULL,
		0x000010000ULL,
		0x000FFFFFFULL,
		0x001000000ULL,
		0x005AA55AAULL,
		0x0AA55AA55ULL,
		0x0FFFFFFFFULL,
		0x100000000ULL,
		0x1AA55AA55ULL,
		0x1FFFFFFFFULL
	};

	for (unsigned long long i = 0; i < sizeof(nptValues)/sizeof(unsigned long long); ++i)
	{
		connector::EditingCommandData ec_sent(0x05); // Add document
		ec_sent.setEventID(1);
		ec_sent.setNPT(nptValues[i]);
		ec_sent.setFinal(false);
		ec_sent.setPrivateDataPayload(privateData, sizeof(privateData)/sizeof(unsigned char));
		ecSender.send( _client, &ec_sent );
		test::wait ( test::flags::connector_ev_data );
		ASSERT_TRUE( ec_received.getEventID() == 1 );
		ASSERT_TRUE( ec_received.getNPT() == nptValues[i]);
	}
}

TEST_F( DISABLED_ConnectorTest, editingcommand_eventID ) {
	const unsigned char privateData[] = "";
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	unsigned long eventid_values[] = {
		0x0000UL,
		0x0001UL,
		0x00FFUL,
		0x0100UL,
		0xFFFFUL
	};

	for (unsigned long long i = 0; i < sizeof(eventid_values)/sizeof(unsigned long long); ++i)
	{
		connector::EditingCommandData ec_sent(0x05); // Add document
		ec_sent.setEventID(eventid_values[i]);
		ec_sent.setNPT(0);
		ec_sent.setFinal(false);
		ec_sent.setPrivateDataPayload(privateData, sizeof(privateData)/sizeof(unsigned char));
		ecSender.send( _client, &ec_sent );
		test::wait ( test::flags::connector_ev_data );
		ASSERT_TRUE( ec_received.getEventID() == eventid_values[i] );
	}
}

TEST_F( DISABLED_ConnectorTest, editingcommand_commandTag ) {
	const unsigned char privateData[] = "";
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	for (unsigned i = 0; i < 256; ++i)
	{
		connector::EditingCommandData ec_sent( (unsigned char)i);
		ec_sent.setEventID(1);
		ec_sent.setNPT(0);
		ec_sent.setFinal(false);
		ec_sent.setPrivateDataPayload(privateData, sizeof(privateData)/sizeof(unsigned char));
		ecSender.send( _client, &ec_sent );
		test::wait ( test::flags::connector_ev_data );
		ASSERT_TRUE( ec_received.getCommandTag() == (unsigned char)i );
	}
}

TEST_F( DISABLED_ConnectorTest, editingcommand_invalidSeqNum01 ) {
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	const unsigned char array1[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x01 /* incomplete */, 0x00
	};
	util::Buffer buffer1( (const char*) array1, sizeof(array1)/sizeof(char), false );
	ecSender.send( _client, buffer1 );

	const unsigned char array2[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x00, 0x00
	};
	util::Buffer buffer2( (const char*) array2, sizeof(array2)/sizeof(char), false );
	ecSender.send( _client, buffer2 );

	test::wait ( test::flags::connector_ev_data );
	ASSERT_TRUE( (ec_received.getPayloadSize() == 0) );
	// Assert previous EC was deleted properly...
	ASSERT_TRUE( ecHandler->getIncompleteEditingCommandsCount() == 0 );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_invalidSeqNum02 ) {
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	const unsigned char array1[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x02 /* complete, but wrong seqNum */, 0x00
	};
	util::Buffer buffer2( (const char*) array1, sizeof(array1)/sizeof(char), false );
	ecSender.send( _client, buffer2 );

	const unsigned char array2[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x00 /* complete */, 0x00
	};
	util::Buffer buffer1( (const char*) array2, sizeof(array2)/sizeof(char), false );
	ecSender.send( _client, buffer1 );

	test::wait ( test::flags::connector_ev_data );
	ASSERT_TRUE( (ec_received.getPayloadSize() == 0) );
	// Assert previous EC was deleted properly...
	ASSERT_TRUE( ecHandler->getIncompleteEditingCommandsCount() == 0 );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_invalidSeqNum03 ) {
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	const unsigned char array1[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x01 /* incomplete addDocument */, 0x00
	};
	util::Buffer buffer1( (const char*) array1, sizeof(array1)/sizeof(char), false );
	ecSender.send( _client, buffer1 );

	const unsigned char array2[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x00 /* complete, but other command tag*/, 0x00
	};
	util::Buffer buffer2( (const char*) array2, sizeof(array2)/sizeof(char), false );
	ecSender.send( _client, buffer2 );

	test::wait ( test::flags::connector_ev_data );
	// Assert previous EC was queued waiting for its end...
	ASSERT_TRUE( ecHandler->getIncompleteEditingCommandsCount() == 1 );

	const unsigned char array3[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x02 /* complete previos addDocument */, 0x00
	};
	util::Buffer buffer3( (const char*) array3, sizeof(array3)/sizeof(char), false );
	ecSender.send( _client, buffer3 );

	test::wait ( test::flags::connector_ev_data );
	ASSERT_TRUE( (ec_received.getPayloadSize() == 0) );
	// Assert previous EC was completed and deleted...
	ASSERT_TRUE( ecHandler->getIncompleteEditingCommandsCount() == 0 );
}

TEST_F( DISABLED_ConnectorTest, editingcommand_invalidEditingCommand ) {
	start();
	connector::EditingCommandHandler* ecHandler = new connector::EditingCommandHandler();
	connector::EditingCommandData ec_received; // Where I keep received data...

	ecHandler->onEditingCommandHandler( boost::bind( &editingCommandHandler_callback, &ec_received, _1 ) );
	_server->addHandler( ecHandler );

	connector::EditingCommandHandler ecSender;

	const unsigned char array1[] =
	{
		0x1a,
		0x00, // Invalid length, EC is not complete
		0x00, 0x01, // Event ID
		0x00, 0x00, 0x00, 0x00, // RESERVED + 1 bit NPT
		0x00, 0x00, 0x00, 0x00, // 32 bits NPT
		0x03, // Private data length
		0x05, // Command tag
		0x00, // Sequence number + 1 bit finalFlag.
	};
	util::Buffer buffer1( (const char*) array1, sizeof(array1)/sizeof(char), false );
	ecSender.send( _client, buffer1 );

	const unsigned char array2[] =
	{
		0x1a, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x2d, 0x00 /* complete, but other command tag*/, 0x00
	};
	util::Buffer buffer2( (const char*) array2, sizeof(array2)/sizeof(char), false );
	ecSender.send( _client, buffer2 );

	test::wait ( test::flags::connector_ev_data );
	// Assert previous EC was not queued
	ASSERT_TRUE( ecHandler->getIncompleteEditingCommandsCount() == 0 );
}

void canvasHandlerCallback( int* received, util::Buffer *msg ) {
	//printf("[Connector] canvasHandlerCallback msg: '%s'\n", msg->asHexa().c_str());
	(*received) = msg->length() - 3; //Substract header size
	test::notify( test::flags::connector_ev_data );
}

TEST_F( DISABLED_ConnectorTest, canvasHandler_basic ) {
	int received = 0;
	start();

	connector::CanvasHandler *canvasHandler = new connector::CanvasHandler();
	canvasHandler->onCanvasEvent( boost::bind(&canvasHandlerCallback, &received, _1) );
	_server->addHandler(canvasHandler);

	const char data[] =
	{
		"Hello, World!!!"
	};
	canvasHandler->send(_client, data, 15);

	test::wait ( test::flags::connector_ev_data );
	ASSERT_TRUE( received == 15 );

	stop();
}

#define NLOOPS 10000
void canvasHandlerCallback2( int* received, util::Buffer * /*msg*/ ) {
	(*received)++;
	if ((*received)==NLOOPS) {
		test::notify( test::flags::connector_ev_data );
	}
}

TEST_F( DISABLED_ConnectorTest, kill_all ) {
	int received = 0;
	start();

	srand( (int)time(0));
	connector::CanvasHandler *canvasHandler = new connector::CanvasHandler();
	canvasHandler->onCanvasEvent( boost::bind(&canvasHandlerCallback2, &received, _1) );
	_server->addHandler(canvasHandler);

	const util::BYTE data[] = {
		0x02, 0x00, 0x16, 0xAB, 0xBA, 0xCD, 0xDE, 0x02, 0x00, 0x16, 0xAB, 0xBA, 0xCD, 0xDE,
		0x02, 0x00, 0x16, 0xAB, 0xBA, 0xCD, 0xDE, 0xDE, 0x00, 0x16, 0xAB, 0xBA, 0xCD, 0xDE
	};

	for( int i=0; i<NLOOPS; i++) {
		int size = (rand() % 28) + 1;
		canvasHandler->send(_client, (const char *)data, size);
	}
	test::wait ( test::flags::connector_ev_data );
	ASSERT_TRUE( received == NLOOPS );

	stop();
}
