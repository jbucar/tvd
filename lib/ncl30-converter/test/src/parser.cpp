#include "parser.h"
#include "../../include/ncl/NclDocumentConverter.h"
#include "../../src/PrivateBaseContext.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fs = boost::filesystem;

std::string getDepot() {
	const char *root = getenv( "DEPOT" );
	return root ? root : "";
}

void scanFiles( const std::string &path, std::vector<std::string> &files, int maxDepth ) {
	if (fs::exists( path )) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr( path ); itr != end_itr; ++itr) {
			if (fs::is_directory( itr->status() ) && 0 < maxDepth) {
				scanFiles( itr->path().string(), files, maxDepth - 1 );
			} else if (fs::is_regular_file( itr->status() )) {
				fs::path file = itr->path();
				if (file.extension() == ".ncl") {
					files.push_back( file.string() );
				}
			}
		}
	}
}

std::vector<std::string> scanFiles( const std::string &path ) {
	std::vector<std::string> files;
	fs::path rootPath( getDepot() );
	rootPath /= "tool";
	rootPath /= "ginga";
	rootPath /= "test";
	rootPath /= "src";
	rootPath /= path;
	scanFiles( rootPath.string().c_str(), files, 100 );
	std::sort( files.begin(), files.end() );
	return files;
}

namespace bptcn = ::br::pucrio::telemidia::converter::ncl;
namespace bptn = ::br::pucrio::telemidia::ncl;
namespace bptgn = ::br::pucrio::telemidia::ginga::ncl;

Parser::Parser() {
}

Parser::~Parser() {
}

void Parser::SetUp( void ) {
	_pbc = new bptgn::PrivateBaseContext();
	_converter = new bptcn::NclDocumentConverter( _pbc );
}

void Parser::TearDown( void ) {
	delete _converter;
	_converter = NULL;
	if (_pbc != NULL) {
		delete _pbc;
	}
}

bptn::NclDocument* Parser::parse( const std::string location ) {
	std::string doc = location;
	_converter->parse( doc );
	return (bptn::NclDocument*) (_converter->getObject( "return", "document" ));
}

class NclParserTest: public Parser
{
};

TEST_P( NclParserTest, execute_ncl ) {
	bptn::NclDocument* ncld = parse( GetParam() );
	ASSERT_TRUE( ncld != NULL );
	delete ncld;

}

INSTANTIATE_TEST_CASE_P( NclParserTest, NclParserTest, testing::ValuesIn( scanFiles("ncl") ) );

class NclFailParserTest: public Parser
{
};

TEST_P( NclFailParserTest, execute_ncl ) {
	bptn::NclDocument* ncld = parse( GetParam() );
	ASSERT_TRUE( ncld == NULL );
	delete ncld;

}

INSTANTIATE_TEST_CASE_P( NclFailParserTest, NclFailParserTest, testing::ValuesIn( scanFiles("ncl.fail") ) );

class NclFailParserAppsTest: public Parser
{
};

TEST_F( NclFailParserAppsTest, importFail ) {
  	std::string pathToCocineros = getDepot();
	pathToCocineros += "lib/ncl30-converter/test/ncl/TestCase_FallaImport/falloImport.ncl";
	bptn::NclDocument* ncld = parse(pathToCocineros);
	ASSERT_TRUE( ncld == NULL );
	delete ncld;

}

