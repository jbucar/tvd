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
#include "../../../util.h"
#include "../../../../../src/service/extension/application/controller/spawner/spawner.h"
#include "../../../../../src/service/extension/application/controller/spawner/taskitem.h"

#ifdef _WIN32
#define CMD_SLEEP "sleep.exe"
#else
#define CMD_SLEEP "/bin/sleep"
#endif

TEST( Spawner, constructor ) {
	tuner::app::spawner::Spawner spawner( 1 );

	ASSERT_TRUE( spawner.maxParallel() == 1 );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, basic_run_stop ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item = new tuner::app::spawner::TaskItem( CMD_SLEEP );
	item->process()->addParam("1000");

	int onStart, onQueue;
	onStart = onQueue = 0;
	item->onStart( boost::bind( &test::counter, &onStart ) );
	item->onQueue( boost::bind( &test::counter, &onQueue ) );
	ASSERT_TRUE( spawner.run( item ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 0 );
	spawner.stop( item );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, stop_finished_process ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item = new tuner::app::spawner::TaskItem( CMD_SLEEP );
	item->process()->addParam("0");
	int onStart, onQueue;
	onStart = onQueue = 0;
	item->onStart( boost::bind( &test::counter, &onStart ) );
	item->onQueue( boost::bind( &test::counter, &onQueue ) );
	ASSERT_TRUE( spawner.run( item ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 0 );
	spawner.stop( item, 1000 );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, stop_invalid ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item->process()->addParam("1000");
	int onStart, onQueue;
	onStart = onQueue = 0;
	item->onStart( boost::bind( &test::counter, &onStart ) );
	item->onQueue( boost::bind( &test::counter, &onQueue ) );
	ASSERT_TRUE( spawner.run( item ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 0 );

	tuner::app::spawner::TaskItem *item2 = NULL;
	item2++;
	spawner.stop( item2 );
	ASSERT_TRUE( spawner.running() == 1 );

	spawner.stop( item );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, stop_with_wait ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item->process()->addParam("1000");

	int onStart, onQueue;
	onStart = onQueue = 0;
	item->onStart( boost::bind( &test::counter, &onStart ) );
	item->onQueue( boost::bind( &test::counter, &onQueue ) );
	ASSERT_TRUE( spawner.run( item ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 0 );

	spawner.stop( item, 10 );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, run_two_process ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item1 = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item1->process()->addParam("1000");

	int onStart1, onQueue1;
	onStart1 = onQueue1 = 0;
	item1->onStart( boost::bind( &test::counter, &onStart1 ) );
	item1->onQueue( boost::bind( &test::counter, &onQueue1 ) );
	item1->priority( 2 );
	ASSERT_TRUE( spawner.run( item1 ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart1 == 1 );
	ASSERT_TRUE( onQueue1 == 0 );

	tuner::app::spawner::TaskItem *item2 = new tuner::app::spawner::TaskItem( CMD_SLEEP );
	item2->process()->addParam("1000");

	int onStart2, onQueue2;
	onStart2 = onQueue2 = 0;
	item2->onStart( boost::bind( &test::counter, &onStart2 ) );
	item2->onQueue( boost::bind( &test::counter, &onQueue2 ) );
	item2->priority( 2 );
	ASSERT_TRUE( spawner.run( item2 ) == tuner::app::spawner::status::queued );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart2 == 0 );
	ASSERT_TRUE( onQueue2 == 1 );

	spawner.stop( item1 );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart1 == 1 );
	ASSERT_TRUE( onQueue1 == 0 );
	ASSERT_TRUE( onStart2 == 1 );
	ASSERT_TRUE( onQueue2 == 1 );

	spawner.stop( item2 );
	ASSERT_TRUE( spawner.running() == 0 );
}

TEST( Spawner, priority ) {
	tuner::app::spawner::Spawner spawner( 1 );

	tuner::app::spawner::TaskItem *item1 = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item1->process()->addParam("1000");
	item1->priority( 2 );
	ASSERT_TRUE( spawner.run( item1 ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );

	tuner::app::spawner::TaskItem *item2 = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item2->process()->addParam("1000");
	item2->priority( 4 );
	ASSERT_TRUE( spawner.run( item2 ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );

	spawner.stop( item1 );
	ASSERT_TRUE( spawner.running() == 1 );

	spawner.stop( item2 );
	ASSERT_TRUE( spawner.running() == 0 );
}

static void checkQueue( int *count, tuner::app::spawner::Spawner *spawner, int *running ) {
	(*count)++;
	*running = spawner->running();
}

TEST( Spawner, enable ) {
	tuner::app::spawner::Spawner spawner( 1 );
	ASSERT_TRUE( spawner.tasks() == 0 );	

	tuner::app::spawner::TaskItem *item = new tuner::app::spawner::TaskItem(  CMD_SLEEP );
	item->process()->addParam("1000");

	int onStart, onQueue, running;
	onStart = onQueue = running = 0;
	item->onStart( boost::bind( &test::counter, &onStart ) );
	item->onQueue( boost::bind( &checkQueue, &onQueue, &spawner, &running ) );
	ASSERT_TRUE( spawner.run( item ) == tuner::app::spawner::status::started );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 0 );
	ASSERT_TRUE( running == 0 );
	ASSERT_TRUE( spawner.tasks() == 1 );

	running=0;
	spawner.enable( false );
	ASSERT_TRUE( spawner.running() == 0 );
	ASSERT_TRUE( onStart == 1 );
	ASSERT_TRUE( onQueue == 1 );
	ASSERT_TRUE( running == 0 );
	ASSERT_TRUE( spawner.tasks() == 1 );

	running=0;
	spawner.enable( true );
	ASSERT_TRUE( spawner.running() == 1 );
	ASSERT_TRUE( onStart == 2 );
	ASSERT_TRUE( onQueue == 1 );
	ASSERT_TRUE( running == 0 );
	ASSERT_TRUE( spawner.tasks() == 1 );	

	spawner.stop( item );
	ASSERT_TRUE( spawner.tasks() == 0 );	
	ASSERT_TRUE( spawner.running() == 0 );
}

