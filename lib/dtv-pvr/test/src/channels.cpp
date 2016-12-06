/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "types.h"
#include "../../src/parental/session.h"
#include "../../src/channels/settingspersister.h"
#include "../../src/channels/xmlpersister.h"
#include <util/buffer.h>
#include <util/serializer/binary.h>
#include <util/serializer/xml.h>
#include <util/settings/settings.h>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace bfs = boost::filesystem;

namespace impl {
	static BasicTuner tuner;
}

pvr::Tuner *getTuner( const std::string & /*name*/ ) {
	return &impl::tuner;
}

class ChannelsTest : public testing::Test {
public:
	ChannelsTest() {
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();

		_added = 0;
		_removed = 0;
	}
	virtual ~ChannelsTest() {}

	virtual void SetUp() {
		_session = new pvr::parental::Session(_settings);
		_chs = new pvr::Channels( createPersister(), _session );

		ASSERT_TRUE( _chs != NULL );

		_chs->onNewChannel().connect( boost::bind(&ChannelsTest::onNew,this,_1) );
		_chs->onChannelRemoved().connect( boost::bind(&ChannelsTest::onRemove,this,_1) );
	}

	virtual void TearDown() {
		_settings->finalize();

		delete _chs;
		delete _session;
		delete _settings;
	}

	void onNew( pvr::Channel * /*ch*/ ) {
		_added++;
	}

	void onRemove( pvr::Channel * /*ch*/ ) {
		_removed++;
	}

	virtual pvr::channels::Persister *createPersister() {
		return new pvr::channels::Persister();
	}

	pvr::Channels *_chs;
	int _added;
	int _removed;
	pvr::parental::Session *_session;
	util::Settings *_settings;
};

class ChannelsTestSessionBlocked : public ChannelsTest {
public:
	virtual void SetUp() {
		ChannelsTest::SetUp();
		_session->pass("pepe");
		_session->expire();
	}

	int addChannel() {
		_session->check("pepe");
		int id = _chs->add(new pvr::Channel("name1", "10.01", "net1"));
		_session->expire();
		return id;
	}
};

TEST_F( ChannelsTest, default_constructor ) {
	ASSERT_TRUE( _chs->getAll().empty() );
}

TEST_F( ChannelsTest, add_channel ) {
	int id = _chs->add( new pvr::Channel("name","01.10", "net1") );
	ASSERT_EQ( id, 0 );
	ASSERT_EQ( _chs->getAll().size(), 1 );
	ASSERT_TRUE( _chs->get( id ) != NULL );
	ASSERT_EQ( _added, 1 );
}

TEST_F( ChannelsTest, add_two_channels ) {
	int id1 = _chs->add( new pvr::Channel("name","01.10", "net1") );
	ASSERT_TRUE( id1 >= 0 );
	ASSERT_TRUE( _chs->get( id1 ) != NULL );

	int id2 = _chs->add( new pvr::Channel("name","01.11", "net1") );
	ASSERT_TRUE( id2 >= 0 );
	ASSERT_TRUE( _chs->get( id2 ) != NULL );

	ASSERT_NE( id1, id2 );
	ASSERT_EQ( _added, 2 );
}

TEST_F( ChannelsTest, remove_channel ) {
	int id = _chs->add( new pvr::Channel("name","01.10", "net1") );
	ASSERT_EQ( _chs->getAll().size(), 1 );
	_chs->remove( id );
	ASSERT_EQ( _chs->getAll().size(), 0 );
	ASSERT_EQ( _removed, 1 );
}

TEST_F( ChannelsTest, remove_all ) {
	ASSERT_TRUE( _chs->add( new pvr::Channel("name","01.10", "net1") ) >= 0 );
	ASSERT_TRUE( _chs->add( new pvr::Channel("name","01.11", "net1") ) >= 0 );
	ASSERT_EQ( _chs->getAll().size(), 2 );

	_chs->removeAll();
	ASSERT_EQ( _chs->getAll().size(), 0 );
	ASSERT_EQ( _removed, 2 );

	int id = _chs->add( new pvr::Channel("name","01.10", "net1") );
	ASSERT_EQ( id, 0 );
}

TEST_F( ChannelsTest, clear_all ) {
	ASSERT_TRUE( _chs->add( new pvr::Channel("name","01.10", "net1") ) >= 0 );
	ASSERT_TRUE( _chs->add( new pvr::Channel("name","01.11", "net1") ) >= 0 );
	ASSERT_EQ( _chs->getAll().size(), 2 );
	_chs->reset();
	ASSERT_EQ( _chs->getAll().size(), 0 );
	ASSERT_EQ( _removed, 0 );
	int newID = _chs->add( new pvr::Channel("name","01.10", "net1") );
	ASSERT_EQ( newID, 0 );
}

TEST_F( ChannelsTest, unblock_all_when_session_disabled ) {
	pvr::Channel *ch1 = new pvr::Channel("name", "10.01", "net1");
	pvr::Channel *ch2 = new pvr::Channel("name2", "10.02", "net1");
	pvr::Channel *ch3 = new pvr::Channel("name3", "10.03", "net1");

	_session->pass("pepe");

	{
		ch1->toggleBlocked();
		ch3->toggleBlocked();
		_chs->add(ch1);
		_chs->add(ch2);
		_chs->add(ch3);
	}

	_session->pass("");

	ASSERT_FALSE( ch1->isBlocked() );
	ASSERT_FALSE( ch2->isBlocked() );
	ASSERT_FALSE( ch3->isBlocked() );
}

TEST_F(ChannelsTestSessionBlocked, check_session_when_add) {
	pvr::Channel *ch = new pvr::Channel("name", "10.01", "net1");
	ASSERT_EQ( _chs->add(ch), -1 );
}

TEST_F(ChannelsTestSessionBlocked, check_session_when_remove) {
	int id = addChannel();
	ASSERT_FALSE( _chs->remove(id) );
}

TEST_F(ChannelsTestSessionBlocked, check_session_when_removeAll) {
	pvr::Channel *ch = new pvr::Channel("name", "10.01", "net1");
	pvr::Channel *ch2 = new pvr::Channel("name2", "11.01", "net2");
	_session->check("pepe");
	_chs->add( ch );
	_chs->add( ch2 );
	_session->expire();
	pvr::ChannelList chs = _chs->getAll();
	ASSERT_EQ( chs.size(), 2 );
	_chs->removeAll();
	ASSERT_EQ( chs.size(), 2 );
}

class TestPersister : public pvr::channels::Persister {
public:
	virtual bool save( const pvr::ChannelList & /*l*/ ) {
		return true;
	}

	virtual bool load( pvr::ChannelList &list ) {
		{
			pvr::Channel *c = new pvr::Channel("name","01.10", "net1");
			c->channelID( 1 );
			list.push_back( c );
		}

		{
			pvr::Channel *c = new pvr::Channel("name","01.11", "net1");
			c->channelID( 2 );
			list.push_back( c );
		}

		return true;
	}
};

class ChannelsTestPersister : public ChannelsTest {
public:
	virtual pvr::channels::Persister *createPersister() {
		return new TestPersister();
	}
};

TEST_F( ChannelsTestPersister, load_id ) {
	ASSERT_TRUE( _chs->load(&getTuner) );
	const pvr::ChannelList &l = _chs->getAll();
	ASSERT_EQ( l.size(), 2 );
	ASSERT_TRUE( l[0]->channelID() > 0 );
	ASSERT_TRUE( l[1]->channelID() > 0 );
	int id = _chs->add( new pvr::Channel("name","01.12", "net1") );
	ASSERT_EQ( id, l[1]->channelID()+1 );
}

class ChannelsDB : public testing::Test {
public:
	ChannelsDB() { _session = NULL; _settings = NULL; }

	virtual void SetUp() {
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();

		_session = new pvr::parental::Session(_settings);
	}

	virtual void TearDown() {
		_settings->finalize();
		delete _settings;
		delete _session;
	}

	pvr::parental::Session *_session;
	util::Settings *_settings;
};

TEST_F( ChannelsDB, load_save_bin ) {
	{
		pvr::Channels *chs = new pvr::Channels( new pvr::channels::settings::Persister(_settings), _session );
		ASSERT_TRUE( chs->add( new pvr::Channel("name","01.10", "net1") ) >= 0 );
		ASSERT_TRUE( chs->add( new pvr::Channel("name","01.11", "net1") ) >= 0 );
		ASSERT_EQ( chs->getAll().size(), 2 );
		delete chs;
	}

	{
		pvr::Channels *chs = new pvr::Channels( new pvr::channels::settings::Persister(_settings), _session );
		ASSERT_TRUE( chs->load(&getTuner) );
		ASSERT_EQ( chs->getAll().size(), 2 );
		delete chs;
	}
}

#define XML_FILE "/tmp/channels.xml"

TEST_F( ChannelsDB, load_save_xml ) {
	if (bfs::exists( XML_FILE )) {
		bfs::remove( XML_FILE );
	}

	{
		pvr::Channels *chs = new pvr::Channels( new pvr::channels::xml::Persister(XML_FILE), _session );
		ASSERT_TRUE( chs->add( new pvr::Channel("name","01.10", "net1") ) >= 0 );
		ASSERT_TRUE( chs->add( new pvr::Channel("name","01.11", "net1") ) >= 0 );
		ASSERT_EQ( chs->getAll().size(), 2 );
		delete chs;
	}

	{
		pvr::Channels *chs = new pvr::Channels( new pvr::channels::xml::Persister(XML_FILE), _session );
		ASSERT_TRUE( chs->load(&getTuner) );
		ASSERT_EQ( chs->getAll().size(), 2 );
		delete chs;
	}

	bfs::remove( XML_FILE );
}
