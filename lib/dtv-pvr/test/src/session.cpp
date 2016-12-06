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
#include "../../src/parental/control.h"
#include <util/settings/settings.h>
#include <gtest/gtest.h>

class SessionTest : public testing::Test {
public:
	SessionTest() { _settings = NULL; _session = NULL; _enabled=0; _blocked=0; }
	virtual ~SessionTest() {}

	virtual void SetUp() {
		_settings = util::Settings::create("test", "memory");
		ASSERT_TRUE( _settings );
		ASSERT_TRUE( _settings->initialize() );
		_settings->clear();

		createSession();
	}

	virtual void TearDown() {
		delete _session;

		_settings->finalize();
		delete _settings;
	}

	void createSession() {
		delete _session;
		_session = new pvr::parental::Session(_settings);
		_session->onEnabledChanged().connect( boost::bind(&SessionTest::onCallback,this,_1,&_enabled) );
		_session->onActiveChanged().connect( boost::bind(&SessionTest::onCallback,this,_1,&_blocked) );
	}

	void onCallback( bool state, int *value ) {
		(*value) += state ? 1 : -1;
	}

	util::Settings *settings() const { return _settings; }
	pvr::parental::Session *session()	const { return _session; }
	int _enabled;
	int _blocked;

private:
	pvr::parental::Session *_session;
	util::Settings *_settings;
};

TEST_F( SessionTest, constructor ) {
	ASSERT_FALSE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );
	ASSERT_FALSE( session()->isSexRestricted() );
	ASSERT_FALSE( session()->isViolenceRestricted() );
	ASSERT_FALSE( session()->isDrugsRestricted() );
	ASSERT_EQ( session()->ageRestricted(), 0 );
}

TEST_F( SessionTest, pass_disabled_enabled ) {
	ASSERT_EQ( _enabled, 0 );
	ASSERT_EQ( _blocked, 0 );
	session()->pass( "012345" );
	ASSERT_EQ( _enabled, 1 );
	ASSERT_EQ( _blocked, 0 );	//TODO: reveer
	ASSERT_TRUE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );
}

TEST_F( SessionTest, pass_disabled_enabled_disabled ) {
	ASSERT_EQ( _enabled, 0 );
	session()->pass( "012345" );
	ASSERT_EQ( _enabled, 1 );
	ASSERT_TRUE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );
	session()->pass( "" );
	ASSERT_FALSE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );
	ASSERT_EQ( _enabled, 0 );
}

TEST_F( SessionTest, pass_storage ) {
	session()->pass( "012345" );
	ASSERT_TRUE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );

	//	Destroy/create again (expire pass)
	createSession();

	ASSERT_TRUE( session()->isEnabled() );
	ASSERT_TRUE( session()->isBlocked() );

	//	Check for pass
	ASSERT_TRUE( session()->check( "012345" ) );

	//	Reset config
	session()->resetConfig();
	ASSERT_FALSE( session()->check( "012345" ) );
	ASSERT_FALSE( session()->isEnabled() );
	ASSERT_FALSE( session()->isBlocked() );
}

TEST_F( SessionTest, expire_pass_disabled ) {
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( session()->isBlocked() );
	session()->expire();
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( session()->isBlocked() );
}

TEST_F( SessionTest, expire_pass_enabled_not_blocked ) {
	ASSERT_EQ( _blocked, 0 );
	session()->pass( "012345" );
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( session()->isBlocked() );
	session()->expire();
	ASSERT_EQ( _blocked, -1 );
	ASSERT_TRUE( session()->isBlocked() );
}

TEST_F( SessionTest, check_pass_disabled ) {
	ASSERT_FALSE( session()->check("1111") );
	ASSERT_FALSE( session()->isBlocked() );
}

TEST_F( SessionTest, check_pass_enabled_error ) {
	ASSERT_EQ( _blocked, 0 );
	session()->pass( "012345" );
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( session()->check("1111") );
	ASSERT_EQ( _blocked, -1 );
	ASSERT_TRUE( session()->isBlocked() );
}

TEST_F( SessionTest, check_pass_enabled_ok ) {
	ASSERT_EQ( _blocked, 0 );
	session()->pass( "012345" );
	ASSERT_EQ( _blocked, 0 );
	ASSERT_TRUE( session()->check("012345") );
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( session()->isBlocked() );
	ASSERT_EQ( _blocked, 0 );
}

TEST_F( SessionTest, time_expiration ) {
	ASSERT_EQ( session()->timeExpiration(), -1 );
	session()->timeExpiration( 10 );
	ASSERT_EQ( session()->timeExpiration(), 10 );
}

TEST_F( SessionTest, change_pass_and_time_expiration_not_block ) {
	session()->pass("012345");
	session()->timeExpiration( 10000 );
	ASSERT_FALSE( session()->isBlocked() );
}

TEST_F( SessionTest, time_expiration_storage ) {
	ASSERT_EQ( session()->timeExpiration(), -1 );
	session()->timeExpiration( 10 );

	//	Destroy/Create
	createSession();

	ASSERT_EQ( session()->timeExpiration(), 10 );

	//	Reset config
	session()->resetConfig();
	ASSERT_EQ( session()->timeExpiration(), -1 );
}

TEST_F( SessionTest, is_allowed_when_pass_disabled ) {
	BasicControl *control = allControl();

	ASSERT_TRUE(session()->isAllowed(control));
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_nothing_restricted ) {
	session()->pass( "012345" );
	BasicControl *control = allControl();

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_sex_restricted_and_content_has_sex ) {
	session()->pass( "012345" );
	session()->restrictSex(true);
	session()->expire();
	BasicControl *control = sexControl();

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_sex_restricted_and_content_has_no_sex ) {
	session()->pass( "012345" );
	session()->restrictSex(true);
	session()->expire();
	BasicControl *control = nothingControl();

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_violence_restricted_and_content_has_violence ) {
	session()->pass( "012345" );
	session()->restrictViolence(true);
	session()->expire();
	BasicControl *control = violenceControl();

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_violence_restricted_and_content_has_no_violence ) {
	session()->pass( "012345" );
	session()->restrictViolence(true);
	session()->expire();
	BasicControl *control = nothingControl();

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_drugs_restricted_and_content_has_drugs ) {
	session()->pass( "012345" );
	session()->restrictDrugs(true);
	session()->expire();
	BasicControl *control = drugsControl();

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_drugs_restricted_and_content_have_no_drugs ) {
	session()->pass( "012345" );
	session()->restrictDrugs(true);
	session()->expire();
	BasicControl *control = nothingControl();

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_age_restricted_and_content_has_more_age ) {
	session()->pass( "012345" );
	session()->restrictAge(16);
	session()->expire();
	BasicControl *control = ageControl(18);

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_age_restricted_and_content_has_the_same_age ) {
	session()->pass( "012345" );
	session()->restrictAge(16);
	session()->expire();
	BasicControl *control = ageControl(16);

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_age_restricted_and_content_have_no_age ) {
	session()->pass( "012345" );
	session()->restrictAge(16);
	session()->expire();
	BasicControl *control = nothingControl();

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_allowed_when_pass_enabled_and_age_restricted_and_content_have_less_age ) {
	session()->pass( "012345" );
	session()->restrictAge(16);
	session()->expire();
	BasicControl *control = ageControl(14);

	ASSERT_TRUE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, is_not_allowed_when_pass_enabled_and_all_is_restricted_and_content_has_all ) {
	session()->pass( "012345" );
	BasicControl *control = allControl();
	session()->restrictAge(16);
	session()->restrictSex(true);
	session()->restrictViolence(true);
	session()->restrictDrugs(true);
	session()->expire();

	ASSERT_FALSE( session()->isAllowed(control) );
	delete control;
}

TEST_F( SessionTest, restrictions_storage ) {
	session()->pass( "012345" );
	session()->restrictViolence(true);
	session()->restrictDrugs(true);
	session()->restrictAge(16);

	//	Destroy/create
	createSession();

	ASSERT_TRUE( session()->isDrugsRestricted() );
	ASSERT_TRUE( session()->isViolenceRestricted() );
	ASSERT_FALSE( session()->isSexRestricted() );
	ASSERT_EQ( session()->ageRestricted(), 16 );
}

TEST_F( SessionTest, restrictions_reset_when_disable ) {
	session()->pass( "012345" );
	session()->restrictViolence(true);
	session()->restrictDrugs(true);
	session()->restrictAge(16);

	//	Disable
	session()->pass( "" );

	ASSERT_FALSE( session()->isDrugsRestricted() );
	ASSERT_FALSE( session()->isViolenceRestricted() );
	ASSERT_FALSE( session()->isSexRestricted() );
	ASSERT_EQ( session()->ageRestricted(), 0 );
}

TEST_F( SessionTest, restrictions_reset_config ) {
	session()->pass( "012345" );
	session()->restrictViolence(true);
	session()->restrictDrugs(true);
	session()->restrictAge(16);

	//	Reset config
	session()->resetConfig();

	ASSERT_FALSE( session()->isDrugsRestricted() );
	ASSERT_FALSE( session()->isViolenceRestricted() );
	ASSERT_FALSE( session()->isSexRestricted() );
	ASSERT_EQ( session()->ageRestricted(), 0 );
}
