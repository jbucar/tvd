#include <gtest/gtest.h>
#include <string>

namespace canvas {
	class Canvas;
}

namespace util  {

std::string getDepot();
std::string getTestRoot();
std::string getImageName( const std::string &name );
bool compareImagesFromPath( canvas::Canvas *c, const std::string &file );
bool compareImages( canvas::Canvas *c, const std::string &file );

}

