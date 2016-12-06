/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "settings.h"
#include "../../../src/settings/settings.h"
#include "../../../src/assert.h"
#include "../../../src/mcr.h"
#include "generated/config.h"
#include <boost/filesystem.hpp>
#include <limits>

namespace bfs = boost::filesystem;

SettingsTest::SettingsTest() {
	_settings = NULL;
}

bool SettingsTest::init() {
	std::string impl = GetParam();
	std::string name("settings_test_");
	_settings = util::Settings::create( name+impl, impl );
	DTV_ASSERT( _settings != NULL );
	return _settings->initialize();
}

void SettingsTest::fin() {
	_settings->finalize();
	DEL(_settings);
}

void SettingsTest::SetUp() {
	ASSERT_TRUE( init() );
}

void SettingsTest::TearDown() {
	if (_settings) {
		_settings->clear();
		fin();
	}
	const char *impl = GetParam();
	if (strcmp(impl,"memory")!=0) {
		std::string filename("settings_test.");
		bfs::remove( filename + impl);
	}
}

void SettingsTest::reload() {
	fin();
	init();
}

util::Settings *SettingsTest::settings() {
	DTV_ASSERT(_settings);
	return _settings;
}

TEST_P( SettingsTest, check_instance ) {
	std::string name("settings_test_");
	ASSERT_STREQ( settings()->name().c_str(), (name + GetParam()).c_str() );
}

TEST_P( SettingsTest, save_get_bool ) {
	bool res = false;

	settings()->put("aBool", true);
	settings()->get("aBool", res);

	ASSERT_TRUE( res );

	settings()->put("aBool", false);
	settings()->get("aBool", res);

	ASSERT_FALSE( res );
}

TEST_P( SettingsTest, save_get_int ) {
	int res = 0;

	settings()->put("anInt", 1);
	settings()->get("anInt", res);

	ASSERT_EQ( 1, res );

	settings()->put("anInt", -1);
	settings()->get("anInt", res);

	ASSERT_EQ( -1, res );
}

TEST_P( SettingsTest, save_get_uint ) {
	unsigned int res = 0u;

	settings()->put("anUInt", 1u);
	settings()->get("anUInt", res);

	ASSERT_EQ( 1u, res );

	settings()->put("anUInt", -1u);
	settings()->get("anUInt", res);

	ASSERT_EQ( std::numeric_limits<unsigned int>::max(), res );
}

TEST_P( SettingsTest, save_get_l_int ) {
	long int res = 0;

	settings()->put("aLongInt", 1L);
	settings()->get("aLongInt", res);

	ASSERT_EQ( 1L, res );

	settings()->put("aLongInt", -1L);
	settings()->get("aLongInt", res);

	ASSERT_EQ( -1L, res );
}

TEST_P( SettingsTest, save_get_ll_int ) {
	long long int res = 0;

	settings()->put("aLongLongInt", 1LL);
	settings()->get("aLongLongInt", res);

	ASSERT_EQ( 1LL, res );

	settings()->put("aLongLongInt", -1LL);
	settings()->get("aLongLongInt", res);

	ASSERT_EQ( -1LL, res );
}

TEST_P( SettingsTest, save_get_ul_int ) {
	unsigned long int res = 0;

	settings()->put("anULongInt", 1UL);
	settings()->get("anULongInt", res);

	ASSERT_EQ( 1UL, res );

	settings()->put("anULongInt", -1UL);
	settings()->get("anULongInt", res);

	ASSERT_EQ( std::numeric_limits<unsigned long int>::max(), res );
}

TEST_P( SettingsTest, save_get_ull_int ) {
	unsigned long long int res = 0;

	settings()->put("anULongLongInt", 1ULL);
	settings()->get("anULongLongInt", res);

	ASSERT_EQ( 1ULL, res );

	settings()->put("anULongLongInt", -1ULL);
	settings()->get("anULongLongInt", res);

	ASSERT_EQ( std::numeric_limits<unsigned long long int>::max(), res );
}

TEST_P( SettingsTest, save_get_float ) {
	float res = 0.0f;

	settings()->put("aFloat", 1.0f);
	settings()->get("aFloat", res);

	ASSERT_EQ( 1.0f, res );

	settings()->put("aFloat", -1.0f);
	settings()->get("aFloat", res);

	ASSERT_EQ( -1.0f, res );
}

TEST_P( SettingsTest, save_get_double ) {
	double res = 0.0;

	settings()->put("aDouble", 1.0);
	settings()->get("aDouble", res);

	ASSERT_EQ( 1.0, res );

	settings()->put("aDouble", -1.0);
	settings()->get("aDouble", res);

	ASSERT_EQ( -1.0, res );
}

TEST_P( SettingsTest, save_get_string ) {
	std::string res;

	settings()->put("aString", "Hello");
	settings()->get("aString", res);

	ASSERT_EQ( "Hello", res );

	settings()->put("aString", "World");
	settings()->get("aString", res);

	ASSERT_EQ( "World", res );
}

TEST_P( SettingsTest, save_get_void_ptr ) {
	const char abc[] = "abc\0ABC\t.-,\nḉ[]\\':;";
	util::Buffer write( abc, sizeof(abc)/sizeof(const char *), false );
	util::Buffer read;

	settings()->put("voidPtr", write );
	settings()->get("voidPtr", read );

	ASSERT_EQ( write, read );
}

TEST_P( SettingsTest, save_get_destroyed_buffer) {
	const char abc[] = "abc\0ABC\t.-,\nḉ[]\\':;";
	util::Buffer *write = new util::Buffer( abc, sizeof(abc)/sizeof(const char *), true );
	util::Buffer original(*write);
	util::Buffer read;

	settings()->put("voidPtr", *write );
	delete write;
	settings()->get("voidPtr", read );

	ASSERT_EQ( original, read );
}

TEST_P( SettingsTest, save_get_nested ) {
	std::string res;

	settings()->put("level1.level2.node", "value");
	settings()->get("level1.level2.node", res);

	ASSERT_EQ( "value", res );

	res = "";
	settings()->get("level1.level2.inexistent.node", res);
	ASSERT_EQ( "", res );
}

TEST_P( SettingsTest, save_and_check ) {
	const char abc[] = "abc\0ABC\t.-,\nḉ[]\\':;";
	util::Buffer write( abc, sizeof(abc)/sizeof(const char *), false );
	util::Buffer read;

	settings()->put("str", "Hello");
	settings()->put("d", 1.5);
	settings()->put("f", 2.6f);
	settings()->put("b", true);
	settings()->put("i", 8);
	settings()->put("ui", 1u);
	settings()->put("l", 15L);
	settings()->put("ul", 567UL);
	settings()->put("ll", -17LL);
	settings()->put("ull", 8754ULL);
	settings()->put("voidptr", write);
	settings()->put("level1.level2.node", "value");

	if (strcmp(GetParam(),"memory")) {
		reload();
	}

	std::string str;
	settings()->get("str", str);
	ASSERT_EQ( "Hello", str );

	double d;
	settings()->get("d", d);
	ASSERT_EQ( 1.5, d );

	float f;
	settings()->get("f", f);
	ASSERT_EQ( 2.6f, f );

	bool b=false;
	settings()->get("b", b);
	ASSERT_TRUE( b );

	int i;
	settings()->get("i", i);
	ASSERT_EQ( 8, i );

	unsigned int ui;
	settings()->get("ui", ui);
	ASSERT_EQ( 1u, ui );

	long int l;
	settings()->get("l", l);
	ASSERT_EQ( 15L, l );

	unsigned long int ul;
	settings()->get("ul", ul);
	ASSERT_EQ( 567UL, ul );

	long long int ll;
	settings()->get("ll", ll);
	ASSERT_EQ( -17LL, ll );

	unsigned long long int ull;
	settings()->get("ull", ull);
	ASSERT_EQ( 8754ULL, ull );

	settings()->get("voidptr", read);
	ASSERT_EQ( read, write );

	settings()->get("level1.level2.node", str);
	ASSERT_EQ( "value", str );
}

TEST_P( SettingsTest, save_and_clear ) {
	const char abc[] = "abc\0ABC\t.-,\nḉ[]\\':;";
	util::Buffer write( abc, sizeof(abc)/sizeof(const char *), false );
	util::Buffer read;

	settings()->put("str", "Hello");
	settings()->put("d", 1.5);
	settings()->put("f", 2.6f);
	settings()->put("b", true);
	settings()->put("i", 8);
	settings()->put("ui", 1u);
	settings()->put("l", 15L);
	settings()->put("ul", 567UL);
	settings()->put("ll", -17LL);
	settings()->put("ull", 8754ULL);
	settings()->put("voidptr", write);

	reload();
	settings()->clear();

	std::string str("");
	settings()->get("str", str);
	ASSERT_EQ( "", str );

	double d=0.0;
	settings()->get("d", d);
	ASSERT_EQ( 0.0, d );

	float f=0.0f;
	settings()->get("f", f);
	ASSERT_EQ( 0.0f, f );

	bool b=false;
	settings()->get("b", b);
	ASSERT_FALSE( b );

	int i=0;
	settings()->get("i", i);
	ASSERT_EQ( 0, i );

	unsigned int ui=0;
	settings()->get("ui", ui);
	ASSERT_EQ( 0U, ui );

	long int l=0L;
	settings()->get("l", l);
	ASSERT_EQ( 0L, l );

	unsigned long int ul=0UL;
	settings()->get("ul", ul);
	ASSERT_EQ( 0UL, ul );

	long long int ll=0LL;
	settings()->get("ll", ll);
	ASSERT_EQ( 0LL, ll );

	unsigned long long int ull=0ULL;
	settings()->get("ull", ull);
	ASSERT_EQ( 0ULL, ull );

	settings()->get("voidptr", read);
	ASSERT_EQ( 0, read.length() );

	settings()->get("level1.level2.node", str);
	ASSERT_EQ( "", str );
}

TEST_P( SettingsTest, clear_key_str ) {
	std::string str("");
	settings()->put("str", "Hello");
	settings()->clear("str");
	settings()->get("str", str);
	ASSERT_EQ( "", str );
}

TEST_P( SettingsTest, clear_key_double ) {
	double d=0.0;
	settings()->put("d", 1.5);
	settings()->clear("d");
	settings()->get("d", d);
	ASSERT_EQ( 0.0, d );
}

TEST_P( SettingsTest, clear_key_float ) {
	float f=0.0f;
	settings()->put("f", 2.6f);
	settings()->clear("f");
	settings()->get("f", f);
	ASSERT_EQ( 0.0f, f );
}

TEST_P( SettingsTest, clear_key_bool ) {
	bool b=false;
	settings()->put("b", true);
	settings()->clear("b");
	settings()->get("b", b);
	ASSERT_FALSE( b );
}

TEST_P( SettingsTest, clear_key_int ) {
	int i=0;
	settings()->put("i", 8);
	settings()->clear("i");
	settings()->get("i", i);
	ASSERT_EQ( 0, i );
}

TEST_P( SettingsTest, clear_key_uint ) {
	unsigned int ui=0;
	settings()->put("ui", 1u);
	settings()->clear("ui");
	settings()->get("ui", ui);
	ASSERT_EQ( 0U, ui );
}

TEST_P( SettingsTest, clear_key_long ) {
	long int l=0L;
	settings()->put("l", 15L);
	settings()->clear("l");
	settings()->get("l", l);
	ASSERT_EQ( 0L, l );
}

TEST_P( SettingsTest, clear_key_ulong ) {
	unsigned long int ul=0UL;
	settings()->put("ul", 567UL);
	settings()->clear("ul");
	settings()->get("ul", ul);
	ASSERT_EQ( 0UL, ul );
}

TEST_P( SettingsTest, clear_key_longlong ) {
	long long int ll=0LL;
	settings()->put("ll", -17LL);
	settings()->clear("ll");
	settings()->get("ll", ll);
	ASSERT_EQ( 0LL, ll );
}

TEST_P( SettingsTest, clear_key_ulonglong ) {
	unsigned long long int ull=0ULL;
	settings()->put("ull", 8754ULL);
	settings()->clear("ull");
	settings()->get("ull", ull);
	ASSERT_EQ( 0ULL, ull );
}

TEST_P( SettingsTest, clear_key_voidptr ) {
	const char abc[] = "abc\0ABC\t.-,\nḉ[]\\':;";
	util::Buffer write( abc, sizeof(abc)/sizeof(const char *), false );
	util::Buffer read;

	settings()->put("voidptr", write);
	settings()->clear("voidptr");
	settings()->get("voidptr", read );
	ASSERT_EQ( 0, read.length() );
}

TEST_P( SettingsTest, clear_nested_key ) {
	std::string str("");
	settings()->put("level1.level2.node", "nested");
	settings()->clear("level1.level2.node");
	settings()->get("level1.level2.node", str);
	ASSERT_EQ( "", str );
}

static const char* impls[] = {
#if SETTINGS_USE_BDB
	"bdb",
#endif
#if SETTINGS_USE_XML
	"xml",
#endif
#if SETTINGS_USE_MEMORY
	"memory",
#endif
};
INSTANTIATE_TEST_CASE_P( SettingsTest, SettingsTest, ::testing::ValuesIn(impls) );
