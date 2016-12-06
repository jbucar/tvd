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

#include "../../src/service/service.h"
#include "../../src/service/provider.h"
#include <util/settings/settings.h>
#include <gtest/gtest.h>

class TestProvider : public pvr::service::Provider {
public:
	TestProvider( util::Settings *s ) : pvr::service::Provider("test", "1.0"), _s(s) {}
	virtual ~TestProvider() {}

protected:
	virtual util::Settings *settings() const { return _s; }

private:
	util::Settings *_s;
};

class TestProviderEnabled : public TestProvider {
public:
	TestProviderEnabled( util::Settings *s ) : TestProvider(s) {}

protected:
	virtual bool defaultState() const { return true; }
};

class TestProviderDisabled : public TestProvider {
public:
	TestProviderDisabled( util::Settings *s ) : TestProvider(s) {}

protected:
	virtual bool defaultState() const { return false; }
};

class TestServiceFail : public pvr::service::Service<TestProvider> {
public:
	TestServiceFail() {}
	virtual ~TestServiceFail() {}

protected:
	virtual bool init() { return false; }
	virtual void fin() {}
};

class ServiceTest : public testing::Test {
public:
	ServiceTest() { _settings = NULL; }
	virtual ~ServiceTest() {}

	virtual void SetUp() {
		_settings = util::Settings::create("test", "memory");
		ASSERT_TRUE( _settings->initialize() );
		_settings->clear();
	}

	virtual void TearDown() {
		_settings->finalize();
		delete _settings;
	}

	util::Settings *settings() { return _settings; }

private:
	util::Settings *_settings;
};

TEST_F( ServiceTest, constructor ) {
	pvr::service::Service<TestProvider> srv;
}

TEST_F( ServiceTest, provider_constructor ) {
	TestProvider *p = new TestProvider(settings());
	ASSERT_EQ( p->name(), "test");
	ASSERT_EQ( p->version(), "1.0");
	ASSERT_EQ( p->isEnabled(), false);
	delete p;
}

TEST_F( ServiceTest, add_get ) {
	pvr::service::Service<TestProvider> srv;
	TestProvider *p = new TestProvider(settings());
	srv.add( p );
	ASSERT_EQ( srv.get("test"), p );
}

TEST_F( ServiceTest, start_with_init_ok ) {
	pvr::service::Service<TestProvider> srv;
	ASSERT_TRUE( srv.start() );
}

TEST_F( ServiceTest, start_with_init_fail ) {
	TestServiceFail srv;
	ASSERT_FALSE( srv.start() );
}

TEST_F( ServiceTest, start_with_provider_fail ) {
	pvr::service::Service<TestProvider> srv;
	srv.add( new TestProvider(settings()) );
	ASSERT_TRUE( srv.start() );
}

TEST_F( ServiceTest, enable_default_state_enabled ) {
	pvr::service::Service<TestProvider> srv;
	srv.add( new TestProviderEnabled(settings()) );
	ASSERT_TRUE( srv.start() );
	ASSERT_TRUE( srv.get( "test" )->isEnabled() );
}

TEST_F( ServiceTest, enable_default_state_disabled ) {
	pvr::service::Service<TestProvider> srv;
	srv.add( new TestProviderDisabled(settings()) );
	ASSERT_TRUE( srv.start() );
	ASSERT_FALSE( srv.get( "test" )->isEnabled() );
}

TEST_F( ServiceTest, enable_disable ) {
	pvr::service::Service<TestProvider> srv;
	srv.add( new TestProvider(settings()) );
	ASSERT_TRUE( srv.start() );

	srv.get( "test" )->enable(true);
	ASSERT_TRUE( srv.get( "test" )->isEnabled() );

	srv.get( "test" )->enable(false);
	ASSERT_FALSE( srv.get( "test" )->isEnabled() );
}

TEST_F( ServiceTest, enable_persist ) {
	{
		pvr::service::Service<TestProvider> *srv = new pvr::service::Service<TestProvider>();
		srv->add( new TestProviderEnabled(settings()) );
		ASSERT_TRUE( srv->start() );
		ASSERT_TRUE( srv->get( "test" )->isEnabled() );
		srv->get( "test" )->enable(false);
		delete srv;
	}

	{
		pvr::service::Service<TestProvider> *srv = new pvr::service::Service<TestProvider>();
		srv->add( new TestProviderEnabled(settings()) );
		ASSERT_TRUE( srv->start() );
		ASSERT_FALSE( srv->get( "test" )->isEnabled() );
		delete srv;
	}
}

TEST_F( ServiceTest, reset_config ) {
	{
		pvr::service::Service<TestProvider> *srv = new pvr::service::Service<TestProvider>();
		srv->add( new TestProviderEnabled(settings()) );
		ASSERT_TRUE( srv->start() );
		ASSERT_TRUE( srv->get( "test" )->isEnabled() );
		srv->get( "test" )->enable(false);
		delete srv;
	}

	{
		pvr::service::Service<TestProvider> *srv = new pvr::service::Service<TestProvider>();
		srv->add( new TestProviderEnabled(settings()) );
		ASSERT_TRUE( srv->start() );
		ASSERT_FALSE( srv->get( "test" )->isEnabled() );
		srv->resetConfig();
		ASSERT_TRUE( srv->get( "test" )->isEnabled() );
		delete srv;
	}
}

