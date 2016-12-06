#pragma once

#include <util/keydefs.h>
#include <gtest/gtest.h>

namespace canvas {
class Canvas;
}

namespace player {
class Device;
class System;
}

namespace ginga {

void init();
void fin();
void run();
void stop();

class Ginga: public testing::Test
{
public:
	Ginga();
	virtual ~Ginga();

protected:
	virtual void SetUp( void );
	virtual void TearDown( void );

	bool play( const std::string &file );

	bool pressKey( ::util::key::type key );
	void onKeyPressed( ::util::key::type key );

	bool compareTo( const std::string &file );
	void compare( void );

	canvas::Canvas *canvas( void );

private:
	bool _compare;
	bool _compareReady;
	bool _keyPressed;
	std::string _file;
};

}
