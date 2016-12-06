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

#include "system.h"
#include "../../src/canvas.h"
#include "../../src/system.h"
#include "../../src/factory.h"
#include "generated/config.h"
#include <util/io/dispatcher.h>
#include <util/log.h>
#include <util/types.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace impl {

	typedef std::pair<util::key::type, bool> KeyStored;
	std::vector<KeyStored> _keys;
	boost::condition_variable wakeup;
	boost::mutex mutex;

	bool init( const std::string &name ) {
		canvas::System *sys = canvas::System::create( name );
		if (!sys) {
			return false;
		}

		if (!sys->initialize( new canvas::Factory() )) {
			return false;
		}

		System::setSystem(sys);
		return true;
	}

	void fin() {
		System::getSystem()->finalize();
		delete System::getSystem();
		System::setSystem(NULL);
	}

}

canvas::System* System::_system;

void System::SetUp() {
	util::log::flush();
}

void System::TearDown() {
	util::log::flush();
}

canvas::System *System::getSystem() {
	return _system;
}

void System::setSystem( canvas::System *sys ) {
	_system = sys;
}

TEST_F( System, canvas_exist ) {
	ASSERT_TRUE( getSystem()->canvas() != NULL );
}

TEST_F( System, DISABLED_keys ) {
	printf( "Press F1\n" );
	{	//	Wait for key!
		boost::unique_lock<boost::mutex> lock( impl::mutex );
		impl::wakeup.wait(lock);
	}

	ASSERT_TRUE( impl::_keys.size() > 0 );
	bool pressed=false;
	for (size_t i=0; i<impl::_keys.size(); i++) {
		if (impl::_keys[i].first == util::key::f1) {
			pressed=true;
		}
	}
	ASSERT_TRUE( pressed );
}

static void timerCallback( int *count, util::id::Ident &/*id*/ ) {
	(*count)++;
	impl::wakeup.notify_all();
}

TEST_F( System, DISABLED_timer_attach ) {
	int count=0;
	util::id::Ident id = getSystem()->io()->addTimer( 10, boost::bind(&timerCallback,&count,_1) );
	ASSERT_TRUE( util::id::isValid(id) );

	{	//	Wait for timer!
		boost::unique_lock<boost::mutex> lock( impl::mutex );
		impl::wakeup.wait(lock);
	}

	ASSERT_TRUE( count == 1 );
}

TEST_F( System, DISABLED_timer_queue_attach ) {
	//TODO: getSystem()->setDispatcher( &dispatch );

	int count=0;
	util::id::Ident id = getSystem()->io()->addTimer( 10, boost::bind(&timerCallback,&count,_1) );
	ASSERT_TRUE(util::id::isValid(id));

	{	//	Wait for timer!
		boost::unique_lock<boost::mutex> lock( impl::mutex );
		impl::wakeup.wait(lock);
	}

	ASSERT_TRUE( count == 1 );
}


TEST_F( System, DISABLED_timer_cancel ) {
	int count=0;
	util::id::Ident id = getSystem()->io()->addTimer( 10000, boost::bind(&timerCallback,&count,_1) );
	ASSERT_TRUE( util::id::isValid(id) );

	getSystem()->io()->stopTimer( id );
	ASSERT_TRUE( count == 0 );
}

TEST_F( System, DISABLED_timer_queue_cancel ) {
	int count=0;
	util::id::Ident id = getSystem()->io()->addTimer( 10000, boost::bind(&timerCallback,&count,_1) );
	ASSERT_TRUE( util::id::isValid(id) );

	getSystem()->io()->stopTimer( id );
	ASSERT_TRUE( count == 0 );
}
