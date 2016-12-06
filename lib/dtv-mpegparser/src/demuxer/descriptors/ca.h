#pragma once

#include "../../types.h"
#include <vector>

namespace tuner {
namespace desc {
namespace ca {

//	Types
struct StructType {
	ID systemID;
	ID pid;
	std::vector<util::BYTE> data;
};
typedef struct StructType Descriptor;
typedef std::vector<Descriptor> Systems;

//	Get system name from ID
const char *getSystemName( tuner::ID caSystemID );

//	Parse/show descriptor
size_t parse( Descriptor &desc, util::BYTE *data, size_t len );
void show( const Descriptor &desc );

}
}
}

