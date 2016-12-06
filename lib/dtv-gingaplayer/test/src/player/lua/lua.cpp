/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../../util.h"
#include "luaplayer.h"
#include "../../../../src/player/luaplayer.h"
#include "../../../../src/device.h"
#include "../../../../src/system.h"
#include "../../../../src/player/settings.h"
#include "generated/test.h"
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fs = boost::filesystem;

namespace lua {

void scanFiles( const std::string &path, std::vector<std::string> &files, int maxDepth ) {
	if (fs::exists( path )) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr( path ); itr != end_itr; ++itr ) {
			if (fs::is_directory( itr->status() ) && 0 < maxDepth) {
				scanFiles( itr->path().string(), files, maxDepth - 1 );
			}
			else if (fs::is_regular_file( itr->status() )) {
				fs::path file = itr->path();
				if (file.extension() == ".lua") {
					files.push_back( file.string() );
				}
			}
		}
	}
}

std::vector<std::string> scanFiles( const std::string &dir ) {
	std::vector<std::string> files;
	fs::path rootPath( util::getTestRoot() );
	rootPath /= "lua";
	rootPath /= dir;
	scanFiles( rootPath.string().c_str(), files, 0 );
	std::sort( files.begin(), files.end() );
	return files;
}

class LuaTest : public LuaPlayer {
public:
	LuaTest() {}
	virtual ~LuaTest() {}

protected:
	virtual void SetUp( void ) {
		LuaPlayer::SetUp();
		//	Get File
		_file = GetParam();
		ASSERT_FALSE( _file.empty() );
	}

	std::string getImageName( const std::string &file, canvas::Canvas *c ) {
		//	Remove extension to file
		fs::path image(file);
		std::string basicFileName = file.substr( 0, file.find( image.extension().string() ) );
		std::string returnStr = basicFileName + "_" + c->name() + ".png";
  		if (!fs::exists( returnStr )){
  			returnStr = basicFileName + ".png";
  		}
		return returnStr;
	}

	std::string _file;
};

class LuaCanvasTest : public LuaTest {};

TEST_P( LuaCanvasTest, execute_canvas_script ) {
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( _player->setProperty( "src", _file ) );

	play(_player);
	wait();
	ASSERT_TRUE( compareImagesFromPath( canvas(), getImageName(_file, canvas()) ) );
}

INSTANTIATE_TEST_CASE_P(
    LuaCanvasTest,
    LuaCanvasTest,
    testing::ValuesIn( scanFiles("canvas") ));

class LuaCanvasFailTest : public LuaTest {};

TEST_P( LuaCanvasFailTest, execute_canvas_with_assert_script ) {
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( _player->setProperty( "src", _file ) );
	if (play(_player)) {
		ASSERT_TRUE( compareImagesFromPath( canvas(), getImageName(_file, canvas()) ) );
	}
}

INSTANTIATE_TEST_CASE_P(
    LuaCanvasFailTest,
    LuaCanvasFailTest,
    testing::ValuesIn( scanFiles("canvas.fail") ));


class LuaEventTest : public LuaTest {};

namespace event {

static void callback( Player *test, player::event::EventImpl *event ) {
	if ((*event)["class"] == "user") {
		if ((*event)["action"] == "wait") {
			test->ref();
		}
		else if ((*event)["action"] == "signal") {
			test->unref();
		}
	}
}

}

TEST_P( LuaEventTest, execute_event_script ) {
	_player->setInputEventCallback( boost::bind(&event::callback, this,_1) );

	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE( setProperty( _player, "src", _file ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), "green" ) );
}

INSTANTIATE_TEST_CASE_P(
    LuaEventTest,
    LuaEventTest,
    testing::ValuesIn( scanFiles("event") ));

#ifdef TEST_GINGAPLAYER_TCP
INSTANTIATE_TEST_CASE_P(
    LuaTCPEventTest,
    LuaEventTest,
    testing::ValuesIn( scanFiles("event/tcp") ));
#endif

class LuaSettingsReadOnlyTest: public LuaTest {};

class LuaPersistentTestCase: public LuaTest {};

TEST_P( LuaSettingsReadOnlyTest, execute_setting_script ) {
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE( setProperty( _player, "src", _file ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), "green" ) );
}

INSTANTIATE_TEST_CASE_P(
	LuaSettingsTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsUserTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/user") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsDefaultTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/default") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsSiTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/si") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsChannelTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/channel") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsSystemTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/system") ));

INSTANTIATE_TEST_CASE_P(
	LuaSettingsServiceTest,
	LuaSettingsReadOnlyTest,
	testing::ValuesIn( scanFiles("settings/service") ));


TEST_P( LuaPersistentTestCase, execute_persistent_script ) {
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( _player->setProperty( "src", _file ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), "green" ) );
}

INSTANTIATE_TEST_CASE_P(
	LuaPersistentTest,
	LuaPersistentTestCase,
	testing::ValuesIn( scanFiles("persistent") ));

INSTANTIATE_TEST_CASE_P(
	LuaPersistentSharedTest,
	LuaPersistentTestCase,
	testing::ValuesIn( scanFiles("persistent/shared") ));

INSTANTIATE_TEST_CASE_P(
	LuaPersistentServiceTest,
	LuaPersistentTestCase,
	testing::ValuesIn( scanFiles("persistent/service") ));

INSTANTIATE_TEST_CASE_P(
	LuaPersistentChannelTest,
	LuaPersistentTestCase,
	testing::ValuesIn( scanFiles("persistent/channel") ));

}
