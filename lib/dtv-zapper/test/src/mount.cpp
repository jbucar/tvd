#include "generated/config.h"
#include "util.h"
#include "../../src/mount/mount.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

class MountTest : public testing::Test {
public:
	MountTest() : _isMount(false), _mountPoint(""), _mount(NULL) {}
	virtual ~MountTest() {}

	virtual void SetUp() {
		_mount = new zapper::mount::Mount();
		ASSERT_TRUE(_mount->initialize());
		_mountPoint = "";
		_isMount = false;
	}

	virtual void TearDown() {
		_mount->finalize();
		delete _mount;
	}

	void onMount( const std::string &mountPoint, bool isMount ) {
		_mountPoint = mountPoint;
		_isMount = isMount;
	}

	void getFiles( std::vector<std::string> *files, const std::string & /*mountPoint*/, bool /*isMount*/ ) {
		_mount->getFiles(".txt", *files);
	}

	bool isMounted() { return _isMount; }
	const std::string &mountPoint() { return _mountPoint; }
	zapper::mount::Mount *mount() { return _mount; }

private:
	bool _isMount;
	std::string _mountPoint;
	zapper::mount::Mount *_mount;
};

TEST_F(MountTest, initialize) {
	ASSERT_EQ(mount()->mount().size(), 0);
	ASSERT_EQ(mount()->maxDepth(), 5);
}

TEST_F(MountTest, maxDepth) {
	int depth = 2;
	mount()->maxDepth(depth);
	ASSERT_EQ(mount()->maxDepth(), depth);
}

TEST_F(MountTest, addPath) {
	mount()->addPath("some_path", true);
	ASSERT_EQ(mount()->mount().size(), 1);

	mount()->addPath("other_path", true);
	ASSERT_EQ(mount()->mount().size(), 2);
}

TEST_F(MountTest, addPath_repeated) {
	mount()->addPath("some_path", true);
	mount()->addPath("some_path", true);
	ASSERT_EQ(mount()->mount().size(), 1);
}

TEST_F(MountTest, addPath_signal) {
	std::string path("some_path");

	mount()->onMount().connect(boost::bind(&MountTest::onMount, this, _1, _2));
	mount()->addPath(path, true);

	ASSERT_EQ(mountPoint(), path);
	ASSERT_TRUE(isMounted());
}

TEST_F(MountTest, addPath_signal_already_added) {
	mount()->addPath("some_path", true);

	mount()->onMount().connect(boost::bind(&MountTest::onMount, this, _1, _2));
	mount()->addPath("some_path", true);

	ASSERT_TRUE(mountPoint().empty());
}

TEST_F(MountTest, removePath_signal) {
	std::string path("some_path");

	mount()->addPath(path, true);
	mount()->onMount().connect(boost::bind(&MountTest::onMount, this, _1, _2));
	mount()->addPath(path, false);

	ASSERT_EQ(mountPoint(), path);
	ASSERT_FALSE(isMounted());
}

TEST_F(MountTest, removePath_not_added) {
	mount()->onMount().connect(boost::bind(&MountTest::onMount, this, _1, _2));
	mount()->addPath("some_path", false);

	ASSERT_TRUE(mountPoint().empty());
}

TEST_F(MountTest, removePath) {
	mount()->addPath("some_path", true);
	mount()->addPath("some_path", false);
	ASSERT_EQ(mount()->mount().size(), 0);
}

TEST_F(MountTest, getFiles_without_register_ext) {
	mount()->addPath(util::getTestResource("files"), true);
	std::vector<std::string> files;
	mount()->getFiles(".txt", files);
	ASSERT_EQ(files.size(), 0);
}

TEST_F(MountTest, getFiles_empty) {
	mount()->addPath(util::getTestResource("files"), true);
	std::vector<std::string> files;
	mount()->registerExtension(".mp3");
	mount()->getFiles(".mp3", files);
	ASSERT_EQ(files.size(), 0);	
}

TEST_F(MountTest, getFiles) {
	mount()->addPath(util::getTestResource("files"), true);
	std::vector<std::string> files;
	mount()->registerExtension(".txt");
	mount()->getFiles(".txt", files);
	ASSERT_EQ(files.size(), 2);
}

TEST_F(MountTest, getFiles_with_maxDepth_0_before_register) {
	mount()->addPath(util::getTestResource(), true);
	std::vector<std::string> files;
	// Get files of current directory
	mount()->maxDepth(0);
	mount()->registerExtension(".txt");
	mount()->getFiles(".txt", files);
	ASSERT_EQ(files.size(), 0);
}

TEST_F(MountTest, getFiles_with_maxDepth_0_after_register) {
	mount()->addPath(util::getTestResource(), true);
	std::vector<std::string> files;
	mount()->registerExtension(".txt");
	// Get files of current directory
	mount()->maxDepth(0);
	mount()->getFiles(".txt", files);
	ASSERT_EQ(files.size(), 0);
}

TEST_F(MountTest, registerExtensions) {
	mount()->addPath(util::getTestResource("files"), true);
	std::vector<std::string> files;
	std::set<std::string> exts;
	exts.insert(".txt");
	exts.insert(".conf");
	mount()->registerExtensions(exts);

	mount()->getFiles(".txt", files);
	ASSERT_EQ(files.size(), 2);

	files.clear();
	mount()->getFiles(".conf", files);
	ASSERT_EQ(files.size(), 3);
}

TEST_F(MountTest, getFiles_onMount) {
	std::vector<std::string> files;
	mount()->registerExtension(".txt");
	mount()->onMount().connect(boost::bind(&MountTest::getFiles, this, &files, _1, _2));
	mount()->addPath(util::getTestResource("files"), true);
	ASSERT_EQ(files.size(), 2);
}
