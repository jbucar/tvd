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

#include "nodehandler.h"
#include "documenthandler.h"
#include "../util.h"
#include "../../../src/mcr.h"
#include "../../../src/buffer.h"
#include "../../../src/xml/nodehandler.h"
#include "../../../src/xml/documenthandler.h"
#include "generated/config.h"
#include <boost/foreach.hpp>
#include <gtest/gtest.h>

void NodeHandlerTest::SetUp() {
	DocumentHandlerTest::SetUp();
	ASSERT_TRUE( docHandler()->initialize() );
	ASSERT_TRUE( docHandler()->handler() );
}

TEST_P( NodeHandlerTest, basic_tag_name ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "hola", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_tag_name_from_string ) {
	const char *xml= "<root></root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "root", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_tag_name_from_buffer ) {
	const char *xml= "<root></root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "root", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, basic_tag_name2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "ncl", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_tag_name2_from_string ) {
	const char *xml= "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<ncl id=\"clima\" xmlns=\"http://www.ncl.org.br/NCL3.0/EDTVProfile\"></ncl>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "ncl", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_tag_name2_from_buffer ) {
	const char *xml= "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<ncl id=\"clima\" xmlns=\"http://www.ncl.org.br/NCL3.0/EDTVProfile\"></ncl>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( "ncl", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, basic_tag_name_fail ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_NE( "ncl", docHandler()->handler()->tagName( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count_from_string ) {
	const char *xml= "<root></root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count_from_buffer ) {
	const char *xml= "<root></root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, basic_children_count2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 1, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count2_from_string ) {
	const char *xml= "<root><prueba></prueba></root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 1, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count2_from_buffer ) {
	const char *xml= "<root><prueba></prueba></root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 1, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, basic_children_count3 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 2, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count3_from_string ) {
	const char *xml= "<root><prueba></prueba><prueba2></prueba2></root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 2, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_count3_from_buffer ) {
	const char *xml= "<root><prueba></prueba><prueba2></prueba2></root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_EQ( 2, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, basic_children_count_fail ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_NE( 4, docHandler()->handler()->childrenCount( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_children ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_FALSE( docHandler()->handler()->hasChildren( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_children2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_TRUE( docHandler()->handler()->hasChildren( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_child_empty ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_FALSE( docHandler()->handler()->child( node, 0 ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, child_named ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_TRUE( docHandler()->handler()->child( node, "pepe" ) == NULL );
	ASSERT_TRUE( docHandler()->handler()->child( node, "prueba" ) != NULL );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_one_child ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_TRUE( docHandler()->handler()->child( node, 0 ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_one_child_failed ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );
	ASSERT_FALSE( docHandler()->handler()->child( node, 1 ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_empty ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	std::vector< util::xml::dom::Node > children;
	docHandler()->handler()->children( node, children );
	ASSERT_EQ( 0, children.size() );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_one_child ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	std::vector< util::xml::dom::Node > children;
	docHandler()->handler()->children( node, children );

	ASSERT_EQ( 1, children.size() );
	ASSERT_EQ( "prueba", docHandler()->handler()->tagName( children.at( 0 ) ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, basic_children_two_children ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	std::vector< util::xml::dom::Node > children;
	docHandler()->handler()->children( node, children );

	ASSERT_EQ( 2, children.size() );
	ASSERT_EQ( "head", docHandler()->handler()->tagName( children.at( 0 ) ) );
	ASSERT_EQ( "body", docHandler()->handler()->tagName( children.at( 1 ) ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_empty ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "", docHandler()->handler()->attribute( node, "attr1" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_basic ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "pepe", docHandler()->handler()->attribute( node, "prop1" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_fail ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "", docHandler()->handler()->attribute( node, "prop2" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_basic2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "clima", docHandler()->handler()->attribute( node, "id" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, textcontent_basic ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test6.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "Hola", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, textcontent_basic2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "Hola", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, textcontent_empty ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, set_textcontent ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test6.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( "Hola", docHandler()->handler()->textContent( node ) );
	docHandler()->handler()->textContent( node, "Chau" );
	ASSERT_EQ( "Chau", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attribute ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "prop1" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attribute2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test8.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "xmlns" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attribute_fail ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasAttribute( node, "xmlns" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attribute_fail2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasAttribute( node, "xmlns" ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, set_attribute ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	docHandler()->handler()->setAttribute( node, "id", "prueba" );
	ASSERT_EQ( "prueba", docHandler()->handler()->attribute( node, "id" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, set_attribute2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	docHandler()->handler()->setAttribute( node, "id", "prueba" );
	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "id" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, set_attribute3 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	docHandler()->handler()->setAttribute( node, "id", "" );
	ASSERT_EQ( "", docHandler()->handler()->attribute( node, "id" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, set_attribute4 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	docHandler()->handler()->setAttribute( node, "prop1", "prueba" );
	ASSERT_EQ( "prueba", docHandler()->handler()->attribute( node, "prop1" ) );
	ASSERT_NE( "clima", docHandler()->handler()->attribute( node, "id" ) );
	ASSERT_NE( "http://www.ncl.org.br/NCL3.0/EDTVProfile", docHandler()->handler()->attribute( node, "xmlns" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, remove_attribute ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "prop1" ) );
	docHandler()->handler()->removeAttribute( node, "prop1");
	ASSERT_FALSE( docHandler()->handler()->hasAttribute( node, "prop1" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, remove_inexistent_attribute ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "prop1" ) );
	docHandler()->handler()->removeAttribute( node, "prop2");
	ASSERT_TRUE( docHandler()->handler()->hasAttribute( node, "prop1" ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, append_child ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	util::xml::dom::Node newElement = docHandler()->createElement( "prueba" );
	docHandler()->handler()->appendChild( node, newElement );
	ASSERT_EQ( 1, docHandler()->handler()->childrenCount( node ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, append_children ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	util::xml::dom::Node newElement = docHandler()->createElement( "prueba" );
	util::xml::dom::Node newElement2 = docHandler()->createElement( "prueba2" );
	docHandler()->handler()->appendChild( node, newElement );
	docHandler()->handler()->appendChild( node, newElement2 );
	ASSERT_EQ( 2, docHandler()->handler()->childrenCount( node ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, append_child_check_tag ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test1.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	util::xml::dom::Node newElement = docHandler()->createElement( "prueba" );
	docHandler()->handler()->appendChild( node, newElement );

	std::vector< util::xml::dom::Node > children;
	docHandler()->handler()->children( node, children );

	ASSERT_EQ( "prueba", docHandler()->handler()->tagName( children.at( 0 ) ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, check_no_parent ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasParent( node ) );
	ASSERT_EQ( NULL, docHandler()->handler()->parent( node ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, check_parent ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasParent( node ) );
	ASSERT_EQ( NULL, docHandler()->handler()->parent( node ) );

	util::xml::dom::Node child = docHandler()->handler()->child(node, 0);
	ASSERT_TRUE( docHandler()->handler()->hasParent( child ) );
	ASSERT_EQ( node, docHandler()->handler()->parent( child ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, create_text_node ) {
	ASSERT_TRUE( docHandler()->createDocument("prueba") );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	util::xml::dom::Node newElement = docHandler()->createTextNode( "texto de prueba" );
	docHandler()->handler()->appendChild( node, newElement );
	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );
	ASSERT_EQ( "texto de prueba", docHandler()->handler()->textContent(node) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attributes ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test6.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasAttributes( node ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, has_attributes2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttributes( node ) );

	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test6.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_FALSE( docHandler()->handler()->hasAttributes( node ) );
	std::vector<util::xml::dom::Attribute> attrs;
	docHandler()->handler()->attributes( node, attrs );
	ASSERT_EQ(0, attrs.size());

	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute2 ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test7.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttributes( node ) );
	std::vector<util::xml::dom::Attribute> attrs;
	docHandler()->handler()->attributes( node, attrs );
	ASSERT_EQ(2, attrs.size());

	BOOST_FOREACH( util::xml::dom::Attribute attr, attrs ) {
		if (attr.name == "prop1") {
			ASSERT_EQ("pepe", attr.value);
		}
		else if (attr.name == "attr") {
			ASSERT_EQ("1", attr.value);
		}
		else {
			ASSERT_TRUE( false );
		}
	}

	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_and_textcontent_from_string ) {
	const char *xml= "<root Hello=\"World\">Hello, World</root>";
	ASSERT_TRUE( docHandler()->loadXML( xml ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttributes( node ) );
	std::vector<util::xml::dom::Attribute> attrs;
	docHandler()->handler()->attributes( node, attrs );
	ASSERT_EQ(1, attrs.size());
	ASSERT_EQ("Hello", attrs[0].name);
	ASSERT_EQ("World", attrs[0].value);
	ASSERT_EQ("Hello, World", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
}

TEST_P( NodeHandlerTest, attribute_and_textcontent_from_buffer ) {
	const char *xml= "<root Hello=\"World\">Hello, World</root>";
	util::Buffer *buf = new util::Buffer(xml, strlen(xml));
	ASSERT_TRUE( docHandler()->loadXML( buf ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_TRUE( docHandler()->handler()->hasAttributes( node ) );
	std::vector<util::xml::dom::Attribute> attrs;
	docHandler()->handler()->attributes( node, attrs );
	ASSERT_EQ(1, attrs.size());
	ASSERT_EQ("Hello", attrs[0].name);
	ASSERT_EQ("World", attrs[0].value);
	ASSERT_EQ("Hello, World", docHandler()->handler()->textContent( node ) );
	docHandler()->close();
	DEL(buf);
}

TEST_P( NodeHandlerTest, remove_node ) {
	ASSERT_TRUE( docHandler()->loadDocument( util::getTestFile("xml/test5.xml") ) );
	util::xml::dom::Node node = docHandler()->root();
	ASSERT_TRUE( node );

	ASSERT_EQ( 1, docHandler()->handler()->childrenCount( node ) );
	util::xml::dom::Node child = docHandler()->handler()->child( node, "prueba" );
	docHandler()->handler()->remove( &child );
	ASSERT_EQ( NULL, child );
	ASSERT_EQ( 0, docHandler()->handler()->childrenCount( node ) );

	docHandler()->close();
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
INSTANTIATE_TEST_CASE_P( NodeHandlerTest, NodeHandlerTest, ::testing::ValuesIn(impls) );
#endif
