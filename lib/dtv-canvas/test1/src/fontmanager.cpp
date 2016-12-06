#include <gtest/gtest.h>
#include "../../src/impl/dummy/fontmanager.h"
#include "generated/config.h"
#if CANVAS_FONTMANAGER_USE_FONTCONFIG
#	include "../../src/impl/fontconfig/fontmanager.h"
#endif
#include <util/mcr.h>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

class FontManagerTest : public testing::Test {
public:
	FontManagerTest()  { _mgr = NULL; }
	virtual ~FontManagerTest() {}

	canvas::FontManager *mgr() { return _mgr; }

protected:
	virtual void SetUp() {
		_mgr = createMgr();
		ASSERT_TRUE( _mgr );
		ASSERT_TRUE( _mgr->initialize() );
	}

	virtual void TearDown() {
		_mgr->finalize();
		DEL(_mgr);
	}

	virtual canvas::FontManager *createMgr() {
		return new canvas::dummy::FontManager();
	}

private:
	canvas::FontManager *_mgr;
};

class MyFontManager : public canvas::FontManager {
public:
	MyFontManager() { find = 0; }
	virtual ~MyFontManager() {}

	int find;

protected:
	virtual std::string findFontImpl( const canvas::FontInfo & /*font*/ ) {
		find++;
		return "test.ttf";
	}
};

class FontManagerTestCache : public FontManagerTest {
protected:
	virtual canvas::FontManager *createMgr() {
		return new MyFontManager();
	}
};

TEST_F( FontManagerTest, basic ) {
}

TEST_F( FontManagerTest, find ) {
	canvas::FontInfo font( "Tiresias, Verdana", 20 );
	const std::string &val = mgr()->findFont( font );
	ASSERT_TRUE( val.empty() );
}

TEST_F( FontManagerTestCache, basic ) {
	canvas::FontInfo font( "Tiresias", 20 );

	MyFontManager *m = (MyFontManager *)mgr();

	{	//	Cache font
		const std::string &val = mgr()->findFont( font );
		ASSERT_TRUE( val == "test.ttf" );
		ASSERT_TRUE( m->find == 1 );
	}

	{	//	Find again
		const std::string &val = mgr()->findFont( font );
		ASSERT_TRUE( val == "test.ttf" );
		ASSERT_TRUE( m->find == 1 );
	}
}

#if CANVAS_FONTMANAGER_USE_FONTCONFIG

class FontManager_fontconfig : public FontManagerTest {
protected:
	virtual canvas::FontManager *createMgr() {
		return new canvas::fontconfig::FontManager();
	}
};

static std::string getDepot() {
	const char *root=getenv( "DEPOT" );
	return root ? root : "/";
}

static std::string getFontRoot() {
	bfs::path rootPath(getDepot());
	rootPath /= "lib";
	rootPath /= "dtv-canvas";
	rootPath /= "test1";
	rootPath /= "fonts";
	return rootPath.string();
}

TEST_F( FontManager_fontconfig, basic ) {
}

TEST_F( FontManager_fontconfig, find_next_family ) {
	canvas::FontInfo font( "alguna", 20 );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "Tiresias.ttf" );
}

TEST_F( FontManager_fontconfig, find_regular ) {
	canvas::FontInfo font( "Tiresias", 20 );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "Tiresias.ttf" );
}

TEST_F( FontManager_fontconfig, find_bold ) {
	canvas::FontInfo font( "Tiresias", 20, true );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "Tiresias-Bold.ttf" );
}

TEST_F( FontManager_fontconfig, find_italic ) {
	canvas::FontInfo font( "Tiresias", 20, false, true );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "Tiresias-Italic.ttf" );
}

TEST_F( FontManager_fontconfig, find_bold_italic ) {
	canvas::FontInfo font( "Tiresias", 20, true, true );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "Tiresias-Italic.ttf" );
}

TEST_F( FontManager_fontconfig, add_dir ) {
	mgr()->addFontDirectory( getFontRoot() );
	canvas::FontInfo font( "rako", 20 );
	const std::string &val = mgr()->findFont( font );
	ASSERT_FALSE( val.empty() );

	bfs::path p(val);
	ASSERT_TRUE( p.filename() == "rako.ttf" );
}

TEST_F( FontManager_fontconfig, clear_font_dirs ) {
	canvas::FontInfo font( "rako", 20 );

	{	//	Add font directory and find for custom font
		mgr()->addFontDirectory( getFontRoot() );
		const std::string &val = mgr()->findFont( font );
		ASSERT_FALSE( val.empty() );
		bfs::path p(val);
		ASSERT_TRUE( p.filename() == "rako.ttf" );
	}

	{	//	Clear font directories and search again
		mgr()->clearFontDirectories();
		const std::string &val = mgr()->findFont( font );
		ASSERT_FALSE( val.empty() );
		bfs::path p(val);
		ASSERT_TRUE( p.filename() == "Tiresias.ttf" );
	}
}

#endif
