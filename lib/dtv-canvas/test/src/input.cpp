/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/


#include "input.h"
#include "../../src/inputmanager.h"

InputManagerTest::InputManagerTest() {
	_mgr = new canvas::InputManager(getSystem());
}

InputManagerTest::~InputManagerTest() {
	delete _mgr;
	_mgr = NULL;
}

void InputManagerTest::keyCallback( util::key::type key, bool isUp, canvas::input::ListenerID id ) {
	KeyEvent ev;
	ev.key = key;
	ev.isUp = isUp;
	ev.id = id;
	_events.push_back( ev );
}

void InputManagerTest::pressKey( util::key::type key ) {
	_mgr->dispatchKey( key, false );
	_mgr->dispatchKey( key, true );
}

void InputManagerTest::SetUp() {
	System::SetUp();
	_events.clear();
}

void InputManagerTest::TearDown() {
	System::TearDown();
}

#define TEST_EVENT(ev,k,e,l)	\
	ASSERT_EQ( ev.key, k ); \
	ASSERT_EQ( ev.isUp, e  ); \
	ASSERT_EQ( ev.id, l );

#define TEST_PRESS(ev,i,k,l)	  \
	TEST_EVENT(ev[i],k,false,l); \
	TEST_EVENT(ev[i+1],k,true,l);

static void checkReservation( const util::key::Keys &keys, util::key::Keys *reservated ) {
	ASSERT_EQ( keys.size(), reservated->size() );
	ASSERT_EQ( keys, *reservated );
}

static void checkKeyPress( util::key::type key, util::key::type keySend ) {
	ASSERT_EQ( key, keySend );
}

static void checkButtonPress( canvas::input::ButtonEvent *evt1, canvas::input::ButtonEvent *evt2 ) {
	ASSERT_EQ( evt1->button, evt2->button );
	ASSERT_EQ( evt1->x, evt2->x );
	ASSERT_EQ( evt1->y, evt2->y );
	ASSERT_EQ( evt1->isPress, evt2->isPress );
}


TEST_F( InputManagerTest, default_constructor ) {
}

TEST_F( InputManagerTest, dispatch_key_without_listeners ) {
	_mgr->onKey().connect( boost::bind(&checkKeyPress,_1,util::key::capital_a) );
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 0 );
}

TEST_F( InputManagerTest, dispatch_null_key ) {
	ASSERT_FALSE( _mgr->dispatchKey( util::key::null, false ) );
	ASSERT_FALSE( _mgr->dispatchKey( util::key::null, true ) );
}

static util::key::type translateKeySimple( util::key::type key ) {
	if (key == util::key::capital_a) {
		return util::key::capital_b;
	}
	return key;
}

TEST_F( InputManagerTest, translate_key_simple ) {
	_mgr->onKeyTranslator().connect( &translateKeySimple );
	_mgr->onKey().connect( boost::bind(&checkKeyPress,_1,util::key::capital_b) );
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 0 );
}

TEST_F( InputManagerTest, dispatch_button_one_client ) {
	canvas::input::ButtonEvent evt;
	_mgr->onButton().connect( boost::bind(&checkButtonPress,_1,&evt) );
	evt.button = 0;
	evt.x = 10;
	evt.y = 10;
	evt.isPress = true;
	_mgr->dispatchButton( &evt );
}

TEST_F( InputManagerTest, dispatch_button_multiple_clients ) {
	canvas::input::ButtonEvent evt;
	_mgr->onButton().connect( boost::bind(&checkButtonPress,_1,&evt) );
	_mgr->onButton().connect( boost::bind(&checkButtonPress,_1,&evt) );
	evt.button = 0;
	evt.x = 10;
	evt.y = 10;
	evt.isPress = true;
	_mgr->dispatchButton( &evt );
}

//reserveKeys
TEST_F( InputManagerTest, reserve_keys_basic ) {
	util::key::Keys keys;
	canvas::input::ListenerID id1( (void *)1 );
	keys.insert( util::key::capital_a );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );
	pressKey( util::key::capital_a );
	pressKey( util::key::capital_b );
	ASSERT_EQ( _events.size(), 2 );
	TEST_PRESS( _events, 0, util::key::capital_a, id1 )
}

static void dispatchKeyOnCallback( util::key::type key, bool isUp, canvas::input::ListenerID id, InputManagerTest *test ) {
	test->keyCallback( key, isUp, id );
	test->pressKey( util::key::capital_b );
}

TEST_F( InputManagerTest, dispatch_key_recursive ) {
	canvas::input::ListenerID id1( (void *)1 );

	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	_mgr->reserveKeys( id1, boost::bind(&dispatchKeyOnCallback,_1,_2,id1,this), keys );

	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 2 );
	TEST_PRESS( _events, 0, util::key::capital_a, id1 )
}

static void dispatchKeyAndRemoveOnCallback( util::key::type key, bool isUp, canvas::input::ListenerID id, InputManagerTest *test ) {
	test->keyCallback( key, isUp, id );
	test->mgr()->releaseKeys(id);
	canvas::input::ListenerID id3( (void *)3 );
	test->mgr()->releaseKeys(id3);
	test->pressKey( util::key::capital_b );
}

TEST_F( InputManagerTest, dispatch_key_recursive_with_remove ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );

	canvas::input::ListenerID id1( (void *)1 );
	_mgr->reserveKeys( id1, boost::bind(&dispatchKeyAndRemoveOnCallback,_1,_2,id1,this), keys );

	canvas::input::ListenerID id2( (void *)2 );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys );

	canvas::input::ListenerID id3( (void *)3 );
	_mgr->reserveKeys( id3, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id3), keys );

	pressKey( util::key::capital_a );

	ASSERT_EQ( _events.size(), 3 );
	TEST_EVENT( _events[0], util::key::capital_a, false, id1 );
	TEST_PRESS( _events, 1, util::key::capital_a, id2 );
}

TEST_F( InputManagerTest, releaseAll ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	keys.insert( util::key::capital_b );

	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );

	canvas::input::ListenerID id1( (void *)1 );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	canvas::input::ListenerID id2( (void *)2 );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys );

	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 4 );
	_events.clear();

	keys.clear();
	_mgr->releaseAll();

	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 0 );
}

TEST_F( InputManagerTest, reserve_key_same_priority ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys );
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 4 );
	TEST_EVENT( _events[0], util::key::capital_a, false, id1 );
	TEST_EVENT( _events[1], util::key::capital_a, false, id2 );
	TEST_EVENT( _events[2], util::key::capital_a, true, id1 );
	TEST_EVENT( _events[3], util::key::capital_a, true, id2 );
}

TEST_F( InputManagerTest, reserve_key_other_priority ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys, HI_INPUT_PRIORITY );
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 2 );
	TEST_PRESS( _events, 0, util::key::capital_a, id2 );
}

TEST_F( InputManagerTest, change_reserved_keys ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	canvas::input::ListenerID id1( (void *)1 );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );
	keys.insert( util::key::capital_b );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	pressKey( util::key::capital_a );
	pressKey( util::key::capital_b );

	ASSERT_EQ( _events.size(), 4 );
	TEST_PRESS( _events, 0, util::key::capital_a, id1 );
	TEST_PRESS( _events, 2, util::key::capital_b, id1 );
}

TEST_F( InputManagerTest, change_reserved_to_empty ) {
	canvas::input::ListenerID id1( (void *)1 );

	//	reserve A
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	keys.clear();
	_mgr->releaseKeys( id1 );
	pressKey( util::key::capital_a );

	ASSERT_EQ( _events.size(), 0 );
}

TEST_F( InputManagerTest, change_reserved_to_empty_between_press_release ) {
	canvas::input::ListenerID id1( (void *)1 );

	//	reserve A
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	keys.clear();
	_mgr->dispatchKey( util::key::capital_a, false );
	_mgr->releaseKeys( id1 );
	_mgr->dispatchKey( util::key::capital_a, true );

	ASSERT_EQ( _events.size(), 1 );
}

static void releaseOnkeyCallback( util::key::type key, bool isUp, canvas::input::ListenerID id, InputManagerTest *test ) {
	test->keyCallback( key, isUp, id );
	test->mgr()->releaseKeys( id );
}

TEST_F( InputManagerTest, change_reserved_to_empty_on_press_callback ) {
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );
	util::key::Keys keys;
	_mgr->onReservationChanged().connect( boost::bind(&checkReservation,_1,&keys) );

	//	reserve A
	keys.insert( util::key::capital_a );
	_mgr->reserveKeys( id1, boost::bind(&releaseOnkeyCallback,_1,_2,id1,this), keys );

	//	reserve B
	keys.insert( util::key::capital_b );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys );

	_mgr->dispatchKey( util::key::capital_a, false );
	_mgr->dispatchKey( util::key::capital_a, true );

	ASSERT_EQ( _events.size(), 3 );
	TEST_EVENT( _events[0], util::key::capital_a, false, id1 );
	TEST_PRESS( _events, 1, util::key::capital_a, id2 );
}

static void releaseAll( util::key::type key, bool isUp, canvas::input::ListenerID id, InputManagerTest *test ) {
	test->keyCallback( key, isUp, id );
	test->mgr()->releaseAll();
}

TEST_F( InputManagerTest, release_all_on_press_callback ) {
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );

	{	//	reserve A
		util::key::Keys keys;
		keys.insert( util::key::capital_a );
		_mgr->reserveKeys( id1, boost::bind(&releaseAll,_1,_2,id1,this), keys );
	}

	{	//	reserve B
		util::key::Keys keys;
		keys.insert( util::key::capital_a );
		_mgr->reserveKeys( id2, boost::bind(&releaseAll,_1,_2,id2,this), keys );
	}

	pressKey( util::key::capital_a );

	ASSERT_EQ( _events.size(), 1 );
	TEST_EVENT( _events[0], util::key::capital_a, false, id1 );
}

static void release2( util::key::type key, bool isUp, canvas::input::ListenerID id, InputManagerTest *test ) {
	test->keyCallback( key, isUp, id );
	if (!isUp) {
		test->mgr()->releaseAll();
		util::key::Keys keys;
		keys.insert( util::key::capital_a );
		test->mgr()->reserveKeys( id, boost::bind(&releaseAll,_1,_2,id,test), keys );
	}
}

TEST_F( InputManagerTest, reserve_free_reserve ) {
	canvas::input::ListenerID id1( (void *)1 );

	//	reserve A
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	_mgr->reserveKeys( id1, boost::bind(&release2,_1,_2,id1,this), keys );

	pressKey( util::key::capital_a );
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 4 );
	TEST_PRESS( _events, 0, util::key::capital_a, id1 );
	TEST_PRESS( _events, 2, util::key::capital_a, id1 );
}

TEST_F( InputManagerTest, dispatch_key_same_listener ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );
	_mgr->dispatchKey( util::key::capital_a, false );
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys, HI_INPUT_PRIORITY );
	_mgr->dispatchKey( util::key::capital_a, true );

	ASSERT_EQ( _events.size(), 2 );
	TEST_PRESS( _events, 0, util::key::capital_a, id1 );
}

TEST_F( InputManagerTest, dispatch_key_same_listener_multiple ) {
	util::key::Keys keys;
	keys.insert( util::key::capital_a );
	keys.insert( util::key::capital_b );
	keys.insert( util::key::capital_c );
	canvas::input::ListenerID id1( (void *)1 );
	canvas::input::ListenerID id2( (void *)2 );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	//	Press
	_mgr->dispatchKey( util::key::capital_a, false );	//id1
	_mgr->dispatchKey( util::key::capital_b, false ); //id1
	_mgr->reserveKeys( id2, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id2), keys, HI_INPUT_PRIORITY );
	_mgr->dispatchKey( util::key::capital_c, false ); //id2

	//	Release all
	_mgr->dispatchKey( util::key::capital_a, true );	//id1
	_mgr->dispatchKey( util::key::capital_c, true );	//id2
	_mgr->dispatchKey( util::key::capital_b, true );	//id1

	ASSERT_EQ( _events.size(), 6 );
	TEST_EVENT( _events[0], util::key::capital_a, false, id1 )
	TEST_EVENT( _events[1], util::key::capital_b, false, id1 )
	TEST_EVENT( _events[2], util::key::capital_c, false, id2 )
	TEST_EVENT( _events[3], util::key::capital_a, true, id1 )
	TEST_EVENT( _events[4], util::key::capital_c, true, id2 )
	TEST_EVENT( _events[5], util::key::capital_b, true, id1 )
}

TEST_F( InputManagerTest, standby_basic ) {
	util::key::Keys keys;
	canvas::input::ListenerID id1( (void *)1 );
	keys.insert( util::key::capital_a );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys );

	_mgr->standby();
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 0 );

	_mgr->wakeup();
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 2 );
}

TEST_F( InputManagerTest, standby_priority ) {
	util::key::Keys keys;
	canvas::input::ListenerID id1( (void *)1 );
	int prio = MAX_INPUT_PRIORITY;
	keys.insert( util::key::capital_a );
	_mgr->reserveKeys( id1, boost::bind(&InputManagerTest::keyCallback,this,_1,_2,id1), keys, prio );

	_mgr->standby();
	pressKey( util::key::capital_a );
	ASSERT_EQ( _events.size(), 2 );
}

