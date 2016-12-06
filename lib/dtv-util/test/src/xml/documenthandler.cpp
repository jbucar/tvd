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

#include "documenthandler.h"
#include "../util.h"
#include "../../../src/assert.h"
#include "../../../src/buffer.h"
#include "../../../src/mcr.h"
#include "../../../src/xml/nodehandler.h"
#include "../../../src/xml/documenthandler.h"
#include "../../../src/xml/documentserializer.h"
#include "generated/config.h"
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <iterator>

namespace fs = boost::filesystem;

DocumentHandlerTest::DocumentHandlerTest() {
	_docHand = NULL;
}

void DocumentHandlerTest::SetUp() {
	_docHand = util::xml::dom::DocumentHandler::create( GetParam() );
	ASSERT_TRUE( _docHand != NULL );
}

void DocumentHandlerTest::TearDown() {
	if (_docHand) {
		_docHand->finalize();
		DEL(_docHand);
	}
}

util::xml::dom::DocumentHandler *DocumentHandlerTest::docHandler() {
	DTV_ASSERT(_docHand);
	return _docHand;
}

TEST_P( DocumentHandlerTest, get_handler_without_init ) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	ASSERT_DEATH( docHandler()->handler(), "" );
}

TEST_P( DocumentHandlerTest, get_root_without_init ) {
	ASSERT_FALSE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_init ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );
	ASSERT_FALSE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_load ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	ASSERT_TRUE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_load_from_string ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	const char *xml= "<root><hello>world</hello></root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	ASSERT_TRUE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_load_from_buffer ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	const char *xml= "<root><hello>world</hello></root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	ASSERT_TRUE( docHandler()->root() );

	DEL(buf);
}

TEST_P( DocumentHandlerTest, api_load_from_string_fail ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	const char *xml = "<root><hello>world</root>";
	ASSERT_FALSE( docHandler()->loadXML( xml ) );
	ASSERT_FALSE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_load_from_buffer_fail ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	const char *xml = "<root><hello>world</root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_FALSE( docHandler()->loadXML( buf ) );
	ASSERT_FALSE( docHandler()->root() );

	DEL(buf);
}

TEST_P( DocumentHandlerTest, api_create ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument("root_name") );
	ASSERT_TRUE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_save ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument("root_name") );
	ASSERT_TRUE( docHandler()->root() );
	ASSERT_TRUE( docHandler()->save("tmp.xml") );
	ASSERT_TRUE( fs::exists( "tmp.xml" ));
	fs::remove("tmp.xml");
}

TEST_P( DocumentHandlerTest, api_save_to_buffer ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	util::Buffer *buf = new util::Buffer();
	ASSERT_TRUE( docHandler()->createDocument("root_name") );
	ASSERT_TRUE( docHandler()->root() );
	ASSERT_TRUE( docHandler()->save(buf) );

	const char *expected = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n<root_name />\n";
	ASSERT_FALSE( strcmp(expected, (const char *) buf->data()) );
	ASSERT_EQ( buf->length(), strlen(expected)+1 );
	ASSERT_EQ( 0, memcmp(buf->data(), expected, buf->length()) );

	DEL(buf);
}

TEST_P( DocumentHandlerTest, api_reload ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	ASSERT_TRUE( docHandler()->root() );
	docHandler()->close();
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	ASSERT_TRUE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_reload_without_close ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	ASSERT_TRUE( docHandler()->root() );
	ASSERT_FALSE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
}

TEST_P( DocumentHandlerTest, api_recreate ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument( "root_name" ) );
	ASSERT_TRUE( docHandler()->root() );
	docHandler()->close();
	ASSERT_TRUE( docHandler()->createDocument( "root_name" ) );
	ASSERT_TRUE( docHandler()->root() );
}

TEST_P( DocumentHandlerTest, api_recreate_without_close ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument( "root_name" ) );
	ASSERT_TRUE( docHandler()->root() );
	ASSERT_FALSE( docHandler()->createDocument( "root_name" ) );
}

TEST_P( DocumentHandlerTest, api_create_and_check ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument( "prueba" ) );

	ASSERT_TRUE( docHandler()->save( "tmp.xml" ) );
	ASSERT_TRUE( docHandler()->close() );
	ASSERT_TRUE( docHandler()->loadDocument( "tmp.xml" ) );

	ASSERT_EQ( "prueba", docHandler()->handler()->tagName( docHandler()->root() ) );
	fs::remove("tmp.xml");
}

TEST_P( DocumentHandlerTest, api_create_save_and_load_without_close ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->createDocument( "prueba" ) );

	ASSERT_TRUE( docHandler()->save( "tmp.xml" ) );
	ASSERT_FALSE( docHandler()->loadDocument( "tmp.xml" ) );
	fs::remove("tmp.xml");
}

TEST_P( DocumentHandlerTest, load_fail ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_FALSE( docHandler()->loadDocument( util::getTestFile("xml/test2.xml") ) );
}

TEST_P( DocumentHandlerTest, load_fail2 ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_FALSE( docHandler()->loadDocument( util::getTestFile("xml/test3.xml") ) );
}

TEST_P( DocumentHandlerTest, load_fail3 ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_FALSE( docHandler()->loadDocument( util::getTestFile("xml/test4.xml") ) );
}

TEST_P( DocumentHandlerTest, load_with_children ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
}

TEST_P( DocumentHandlerTest, load_with_text ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test6.xml") ) );
}

TEST_P( DocumentHandlerTest, load_with_attr ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
}

TEST_P( DocumentHandlerTest, load_complex ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
}

TEST_P( DocumentHandlerTest, serialize_to_stream_preorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test9.xml") ) );

	std::stringbuf buffer;
	std::ostream output(&buffer);
	std::string expected(
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n\
<root id=\"0\" >\n\
  <empty></empty>\n\
  <text>text</text>\n\
  <number>text</number>\n\
  <nested>\n\
    <nested1 attr1=\"1\" >n1</nested1>\n\
    <nested2 attr2=\"2\" attr3=\"2\" >n2</nested2>\n\
  </nested>\n\
</root>\n");

	{
		util::xml::dom::StreamDocumentSerializer serializer(output);
		docHandler()->traverse( serializer );
	}
	ASSERT_EQ( expected, buffer.str() );
}

TEST_P( DocumentHandlerTest, serialize_to_stream_postorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test9.xml") ) );

	std::stringbuf buffer;
	std::ostream output(&buffer);
	std::string expected(
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n\
  <empty></empty>\n\
  <text>text</text>\n\
  <number>text</number>\n\
    <nested1 attr1=\"1\" >n1</nested1>\n\
    <nested2 attr2=\"2\" attr3=\"2\" >n2</nested2>\n\
  <nested>\n\
  </nested>\n\
<root id=\"0\" >\n\
</root>\n");

	{
		util::xml::dom::StreamDocumentSerializer serializer(output);
		docHandler()->traverse( serializer, util::xml::dom::postorder );
	}
	ASSERT_EQ( expected, buffer.str() );
}

TEST_P( DocumentHandlerTest, serialize_to_stream_inorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test9.xml") ) );

	std::stringbuf buffer;
	std::ostream output(&buffer);
	std::string expected(
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n\
  <empty></empty>\n\
<root id=\"0\" >\n\
  <text>text</text>\n\
  <number>text</number>\n\
    <nested1 attr1=\"1\" >n1</nested1>\n\
  <nested>\n\
    <nested2 attr2=\"2\" attr3=\"2\" >n2</nested2>\n\
  </nested>\n\
</root>\n");

	{
		util::xml::dom::StreamDocumentSerializer serializer(output);
		docHandler()->traverse( serializer, util::xml::dom::inorder );
	}
	ASSERT_EQ( expected, buffer.str() );
}

TEST_P( DocumentHandlerTest, serialize_to_file_preorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	std::string filename = util::getTestFile("xml/test9.xml");
	ASSERT_TRUE( docHandler()->loadDocument( filename ) );

	{
		util::xml::dom::FileDocumentSerializer serializer("tmp.xml");
		docHandler()->traverse( serializer );
	}

	std::ifstream ifs1(filename.c_str());
	std::ifstream ifs2("tmp.xml");
	std::istream_iterator<char> it1(ifs1), it2(ifs2), eos;

	while (it1!=eos && it2!=eos) {
		ASSERT_EQ(*it1++, *it2++);
	}
	ASSERT_EQ(it1, eos);
	ASSERT_EQ(it2, eos);

	fs::remove("tmp.xml");
}

TEST_P( DocumentHandlerTest, serialize_to_file_postorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	std::string filename = util::getTestFile("xml/test9.xml");
	ASSERT_TRUE( docHandler()->loadDocument( filename ) );

	{
		util::xml::dom::FileDocumentSerializer serializer("tmppostorder.xml");
		docHandler()->traverse( serializer, util::xml::dom::postorder );
	}

	std::ifstream ifs1( util::getTestFile("xml/test9postorder.xml").c_str() );
	std::ifstream ifs2("tmppostorder.xml");
	std::istream_iterator<char> it1(ifs1), it2(ifs2), eos;

	while (it1!=eos && it2!=eos) {
		ASSERT_EQ(*it1++, *it2++);
	}
	ASSERT_EQ(it1, eos);
	ASSERT_EQ(it2, eos);

	fs::remove("tmppostorder.xml");
}

TEST_P( DocumentHandlerTest, serialize_to_file_inorder ) {
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );

	std::string filename = util::getTestFile("xml/test9.xml");
	ASSERT_TRUE( docHandler()->loadDocument( filename ) );

	{
		util::xml::dom::FileDocumentSerializer serializer("tmpinorder.xml");
		docHandler()->traverse( serializer, util::xml::dom::inorder );
	}

	std::ifstream ifs1( util::getTestFile("xml/test9inorder.xml").c_str() );
	std::ifstream ifs2( "tmpinorder.xml" );
	std::istream_iterator<char> it1(ifs1), it2(ifs2), eos;

	while (it1!=eos && it2!=eos) {
		ASSERT_EQ(*it1++, *it2++);
	}
	ASSERT_EQ(it1, eos);
	ASSERT_EQ(it2, eos);

	fs::remove("tmpinorder.xml");
}

#if DOM_USE_TINYXML || DOM_USE_XERCES
static const char* impls[] = {
#if DOM_USE_TINYXML
	"tinyxml",
#endif
#if DOM_USE_XERCES
	"xerces",
#endif
};
INSTANTIATE_TEST_CASE_P( DocumentHandlerTest, DocumentHandlerTest, ::testing::ValuesIn(impls) );
#endif
