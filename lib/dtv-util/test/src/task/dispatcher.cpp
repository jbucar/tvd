#include "generated/config.h"
#include "../../../src/task/dispatcher.h"
#include <gtest/gtest.h>
#include <boost/bind.hpp>

class DispatcherImpl : util::task::Dispatcher {
public:
	DispatcherImpl() : util::task::Dispatcher() {}
	virtual ~DispatcherImpl() {}

	virtual void post( util::task::Target target, const util::task::Type &task ) {
		run(target, task);
	}
};

class DispatcherTest : public testing::Test {
public:
	DispatcherTest() : _disp(NULL), _wasRun(false) {}
	virtual ~DispatcherTest() {}

	void run() { _wasRun = true; }

protected:
	virtual void SetUp() {
		_disp = new DispatcherImpl();
	}

	virtual void TearDown() {
		_wasRun = false;
		delete _disp;
	}

	util::task::Dispatcher * disp() { return (util::task::Dispatcher *) _disp; }
	bool wasRun() { return _wasRun; }

private:
	DispatcherImpl *_disp;
	bool _wasRun;

};

TEST_F(DispatcherTest, registerTarget) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->post(task, boost::bind(&DispatcherTest::run, this));
	ASSERT_TRUE(wasRun());
}

TEST_F(DispatcherTest, unregisterTarget) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->unregisterTarget(task);
	disp()->post(task, boost::bind(&DispatcherTest::run, this));
	ASSERT_FALSE(wasRun());
}

TEST_F(DispatcherTest, enableTarget) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->unregisterTarget(task);
	disp()->registerTarget(task, "task");
	disp()->post(task, boost::bind(&DispatcherTest::run, this));
	ASSERT_TRUE(wasRun());
}

TEST_F(DispatcherTest, enableTargetWithDifferentName) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->unregisterTarget(task);
	disp()->registerTarget(task, "other_task");
	ASSERT_EQ(disp()->name(task), "other_task");
}

TEST_F(DispatcherTest, enableTargetWithDifferentNameWithoutRegister) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->registerTarget(task, "other_task");
	ASSERT_EQ(disp()->name(task), "other_task");
}

TEST_F(DispatcherTest, postUnregisteredTask) {
	int *task = (int *) 0x10;
	disp()->post(task, boost::bind(&DispatcherTest::run, this));
	ASSERT_FALSE(wasRun());
}

TEST_F(DispatcherTest, clear) {
	int *task = (int *) 0x10;
	disp()->registerTarget(task, "task");
	disp()->clear();
	disp()->post(task, boost::bind(&DispatcherTest::run, this));
	ASSERT_FALSE(wasRun());
}
