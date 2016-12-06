#pragma once

#include <gtest/gtest.h>
#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
class NclDocumentConverter;
}
}
namespace ginga {
namespace ncl {
class PrivateBaseContext;
}
}
namespace ncl {
class NclDocument;
}
}
}
}
;

namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptcn = ::br::pucrio::telemidia::converter::ncl;
namespace bptgn = ::br::pucrio::telemidia::ginga::ncl;

class Parser: public testing::TestWithParam<std::string>
{
public:
	Parser();
	virtual ~Parser();

	bptn::NclDocument* parse( const std::string location );

protected:
	virtual void SetUp( void );
	virtual void TearDown( void );

private:
	bptcn::NclDocumentConverter * _converter;
	bptgn::PrivateBaseContext *_pbc;

};
