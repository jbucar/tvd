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

#include "../../../src/cfg/propertynode.h"
#include "propertynode.h"
#include <boost/function.hpp>
#include <string>

PropertyNode::PropertyNode() {
}
PropertyNode::~PropertyNode() {
}

void PropertyNode::SetUp( void ) {
	_root
		.addValue("bool", "Desc", true)
		.addValue("int", "Desc", 0)
		.addValue("str", "Desc", std::string("val"))
		.addNode("node_1")
			.addValue("bool", "Desc", true)
			.addValue("int", "Desc", 1)
			.addValue("str", "Desc", std::string("val"))
			.addNode("node_1_1")
				.addValue("bool", "Desc", true)
				.addValue("int", "Desc", 12)
				.addValue("str", "Desc", std::string("val"));
	_root
		.addNode("node_2")
		.addValue("bool", "Desc", true)
		.addValue("int", "Desc", 2)
		.addValue("str", "Desc", std::string("val"))
			.addNode("node_2_1")
				.addValue("bool", "Desc", true)
				.addValue("int", "Desc", 2)
				.addValue("str", "Desc", std::string("val"))
				.addNode("node_2_1_1")
					.addValue("bool", "Desc", true)
					.addValue("int", "Desc", 2)
					.addValue("str", "Desc", std::string("val"));
	_root("node_2").addNode("node_2_2");
	_root
		.addNode("node_3")
			.addValue("bool", "Desc", true)
			.addValue("int", "Desc", 2)
			.addValue("str", "Desc", std::string("val"));
	_root.addNode("node_4");
}
void PropertyNode::TearDown( void ) {
	_root.removeNode("node_1");
	_root.removeNode("node_2");
	_root.removeNode("node_3");
	_root.removeNode("node_4");
}

TEST( BasicPropertyNode, constructor ) {
	util::cfg::PropertyNode root;
	util::cfg::PropertyNode node("node");

	ASSERT_TRUE( root.name() == "root" );
	ASSERT_TRUE( node.name() == "node" );
}

TEST( BasicPropertyNode, addNode ) {
	util::cfg::PropertyNode root;

	ASSERT_THROW( root("node_1"), std::runtime_error );
	root.addNode("node_1");
	ASSERT_NO_THROW({ root("node_1"); });
	ASSERT_THROW( root.addNode("node_1"), std::runtime_error );

	ASSERT_THROW( root("node_2"), std::runtime_error );
	root.addNode("node_2");
	ASSERT_NO_THROW({ root("node_2"); });
	ASSERT_THROW( root.addNode("node_2"), std::runtime_error );

	ASSERT_THROW( root("node_1.node_1_2"), std::runtime_error );
	root("node_1").addNode("node_1_2");
	ASSERT_NO_THROW({ root("node_1.node_1_2"); });
	ASSERT_THROW( root("node_1").addNode("node_1_2"), std::runtime_error );
	
	ASSERT_THROW( root("node_1.node_1_3"), std::runtime_error );
	root("node_1").addNode("node_1_3");
	ASSERT_NO_THROW({ root("node_1.node_1_3"); });
	ASSERT_THROW( root("node_1").addNode("node_1_3"), std::runtime_error );
	
	ASSERT_THROW( root("node_2.node_2_1"), std::runtime_error );
	root("node_2").addNode("node_2_1");
	ASSERT_NO_THROW({ root("node_2.node_2_1"); });
	ASSERT_THROW( root("node_2").addNode("node_2_1"), std::runtime_error );
	
	ASSERT_THROW( root("node_2.node_2_1.node_2_1_1"), std::runtime_error );
	root("node_2.node_2_1").addNode("node_2_1_1");
	ASSERT_NO_THROW({ root("node_2.node_2_1.node_2_1_1"); });
	ASSERT_THROW( root("node_2.node_2_1").addNode("node_2_1_1"), std::runtime_error );
	
	ASSERT_THROW( root("node_2.node_2_1.node_2_1_1.node_2_1_1_1"), std::runtime_error );
	root("node_2.node_2_1.node_2_1_1").addNode("node_2_1_1_1");
	ASSERT_NO_THROW({ root("node_2.node_2_1.node_2_1_1.node_2_1_1_1"); });
	ASSERT_THROW( root("node_2.node_2_1.node_2_1_1").addNode("node_2_1_1_1"), std::runtime_error );
}

TEST( BasicPropertyNode, getNode ) {
	util::cfg::PropertyNode root;

	root.addNode("node_1");
	ASSERT_TRUE( root("node_1").name() == "node_1");

	root.addNode("node_2");
	ASSERT_TRUE( root("node_2").name() == "node_2");

	root("node_1").addNode("node_1_2");
	ASSERT_TRUE( root("node_1.node_1_2").name() == "node_1_2");

	root("node_1").addNode("node_1_3");
	ASSERT_TRUE( root("node_1.node_1_3").name() == "node_1_3");
	
	root("node_2").addNode("node_2_1");
	ASSERT_TRUE( root("node_2.node_2_1").name() == "node_2_1");
	
	root("node_2.node_2_1").addNode("node_2_1_1");
	ASSERT_TRUE( root("node_2.node_2_1.node_2_1_1").name() == "node_2_1_1");
	
	root("node_2.node_2_1.node_2_1_1").addNode("node_2_1_1_1");
	ASSERT_TRUE( root("node_2.node_2_1.node_2_1_1.node_2_1_1_1").name() == "node_2_1_1_1");
}

TEST( BasicPropertyNode, get ) {
	util::cfg::PropertyNode root;

	root.addValue("int", "desc", 10);
	ASSERT_TRUE( root.get<int> ("int") == 10 );

	root
		.addNode("node_1")
		.addValue("int", "desc", 15);
	
	printf("Int = %d\n", root.get<int> ("node_1.int") );

	ASSERT_TRUE( root.get<int> ("node_1.int") == 15 );

	root
		.addNode("node_2")
		.addValue("int", "desc", 20);
	ASSERT_TRUE( root.get<int> ("node_2.int") == 20 );

	root("node_2")
		.addNode("node_2_1")
		.addValue("int", "desc", 25);
	ASSERT_TRUE( root.get<int> ("node_2.node_2_1.int") == 25 );
}

TEST( BasicPropertyNode, get_bad_path ) {
	util::cfg::PropertyNode root;

	ASSERT_THROW( root.get<int> ("noProp"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("noNode.int"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("noNode.noProp"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("noNode.noNode.int"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("noNode.noNode.noProp"), std::runtime_error);
	
	root
		.addValue("prop", 2)
		.addNode("node_1")
			.addValue("prop_1", 10);

	ASSERT_NO_THROW({ root.get<int> ("prop"); });
	ASSERT_NO_THROW({ root.get<int> ("node_1.prop_1"); });
	ASSERT_THROW( root.get<int> ("node_1.noProp"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("node_1.noNode.int"), std::runtime_error);
	
	root("node_1")
		.addNode("node_1_2")
		.addValue("prop_1_2", 10);

	ASSERT_NO_THROW({ root.get<int> ("node_1.node_1_2.prop_1_2"); });
	ASSERT_THROW( root.get<int> ("node_1.node_1_2.noProp"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("node_1.node_1_2.noNode.int"), std::runtime_error);
}

TEST( BasicPropertyNode, get_bad_type ) {
	util::cfg::PropertyNode root;
	root
		.addValue("str", "string")
		.addValue("bool", false)
		.addValue("int", 15);

	ASSERT_THROW( root.get<int> ("str"), std::runtime_error);
	ASSERT_THROW( root.get<int> ("bool"), std::runtime_error);
	
	ASSERT_THROW( root.get<std::string> ("int"), std::runtime_error);
	ASSERT_THROW( root.get<std::string> ("bool"), std::runtime_error);
	
	ASSERT_THROW( root.get<bool> ("str"), std::runtime_error);
	ASSERT_THROW( root.get<bool> ("int"), std::runtime_error);
}

TEST( BasicPropertyNode, addValue_int ) {
	util::cfg::PropertyNode root;

	ASSERT_NO_THROW({ root.addValue("int", "desc", 10); });
	ASSERT_TRUE( root.get<int> ("int") == 10 );
	ASSERT_THROW( root.addValue("int", "desc", 10), std::runtime_error);
	
	ASSERT_NO_THROW({ root.addValue("int2", 10); });
	ASSERT_TRUE( root.get<int> ("int2") == 10 );
	ASSERT_THROW( root.addValue("int2", 10), std::runtime_error);
}

TEST( BasicPropertyNode, addValue_string ) {
	util::cfg::PropertyNode root;

	ASSERT_NO_THROW({ root.addValue("str", "desc", std::string("string")); });
	ASSERT_TRUE( root.get<std::string> ("str") == "string" );
	ASSERT_THROW( root.addValue("str", "desc", std::string("string")), std::runtime_error);
	
	ASSERT_NO_THROW({ root.addValue("str2", std::string("string")); });
	ASSERT_TRUE( root.get<std::string> ("str2") == "string" );
	ASSERT_THROW( root.addValue("str2", std::string("string")), std::runtime_error);
}

TEST( BasicPropertyNode, addValue_char_pointer ) {
	util::cfg::PropertyNode root;

	ASSERT_NO_THROW({ root.addValue("charPointer", "desc", "string"); });
	ASSERT_TRUE( root.get<std::string> ("charPointer") == "string" );
	ASSERT_THROW( root.addValue("charPointer", "desc", "string"), std::runtime_error);

	ASSERT_NO_THROW({ root.addValue("charPointer2", "string"); });
	ASSERT_TRUE( root.get<std::string> ("charPointer2") == "string" );
	ASSERT_THROW( root.addValue("charPointer2", "string"), std::runtime_error);
}

TEST( BasicPropertyNode, addValue_bool ) {
	util::cfg::PropertyNode root;

	ASSERT_NO_THROW({ root.addValue("bool", "desc", false); });
	ASSERT_FALSE( root.get<bool> ("bool") );
	ASSERT_THROW( root.addValue("bool", "desc", true), std::runtime_error);

	ASSERT_NO_THROW({ root.addValue("bool2", true); });
	ASSERT_TRUE( root.get<bool> ("bool2") );
	ASSERT_THROW( root.addValue("bool2", false), std::runtime_error);
}

TEST( BasicPropertyNode, removeNode ) {
	util::cfg::PropertyNode root;
	root.addNode("child1");
	root.addNode("child2");
	root.addNode("child3");

	root.removeNode("child1");
	ASSERT_TRUE( !root.hasChild("child1") );
	ASSERT_TRUE( root.hasChild("child2") );
	ASSERT_TRUE( root.hasChild("child3") );

	root.removeNode("child2");
	ASSERT_TRUE( !root.hasChild("child1") );
	ASSERT_TRUE( !root.hasChild("child2") );
	ASSERT_TRUE( root.hasChild("child3") );

	root.removeNode("child3");
	ASSERT_TRUE( !root.hasChild("child1") );
	ASSERT_TRUE( !root.hasChild("child2") );
	ASSERT_TRUE( !root.hasChild("child3") );
}

TEST( BasicPropertyNode, removeProp ) {
	util::cfg::PropertyNode root;
	root.addValue("val1","desc",false);
	root.addValue("val2","desc",false);
	root.addValue("val3","desc",false);

	root.removeProp("val1");
	ASSERT_TRUE( !root.existsValue("val1") );
	ASSERT_TRUE( root.existsValue("val2") );
	ASSERT_TRUE( root.existsValue("val3") );

	root.removeProp("val2");
	ASSERT_TRUE( !root.existsValue("val1") );
	ASSERT_TRUE( !root.existsValue("val2") );
	ASSERT_TRUE( root.existsValue("val3") );

	root.removeProp("val3");
	ASSERT_TRUE( !root.existsValue("val1") );
	ASSERT_TRUE( !root.existsValue("val2") );
	ASSERT_TRUE( !root.existsValue("val3") );
}

TEST_F( PropertyNode, set_str ) {
	_root.set("str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("str") == "someValue");

	_root.set("node_1.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_1.str") == "someValue");

	_root.set("node_1.node_1_1.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_1.node_1_1.str") == "someValue");
	
	_root.set("node_2.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_2.str") == "someValue");

	_root.set("node_2.node_2_1.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_2.node_2_1.str") == "someValue");

	_root.set("node_2.node_2_1.node_2_1_1.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_2.node_2_1.node_2_1_1.str") == "someValue");

	_root.set("node_3.str",std::string("someValue"));
	ASSERT_TRUE( _root.get<std::string> ("node_3.str") == "someValue");
}

TEST_F( PropertyNode, set_bool ) {
	_root.set("bool", true);
	ASSERT_TRUE( _root.get<bool> ("bool") );

	_root.set("node_1.bool", true);
	ASSERT_TRUE( _root.get<bool> ("node_1.bool") );

	_root.set("node_1.node_1_1.bool", false);
	ASSERT_FALSE( _root.get<bool> ("node_1.node_1_1.bool"));
	
	_root.set("node_2.bool", true);
	ASSERT_TRUE( _root.get<bool> ("node_2.bool") );

	_root.set("node_2.node_2_1.bool", true);
	ASSERT_TRUE( _root.get<bool> ("node_2.node_2_1.bool") );

	_root.set("node_2.node_2_1.node_2_1_1.bool", false);
	ASSERT_FALSE( _root.get<bool> ("node_2.node_2_1.node_2_1_1.bool") );

	_root.set("node_3.bool", false);
	ASSERT_FALSE( _root.get<bool> ("node_3.bool") );
}

TEST_F( PropertyNode, set_int ) {
	_root.set("int", 20);
	ASSERT_TRUE( _root.get<int> ("int") == 20 );

	_root.set("node_1.int", 20);
	ASSERT_TRUE( _root.get<int> ("node_1.int") == 20 );

	_root.set("node_1.node_1_1.int", 14);
	ASSERT_TRUE( _root.get<int> ("node_1.node_1_1.int") == 14 );
	
	_root.set("node_2.int", 20);
	ASSERT_TRUE( _root.get<int> ("node_2.int") == 20 );

	_root.set("node_2.node_2_1.int", 20);
	ASSERT_TRUE( _root.get<int> ("node_2.node_2_1.int") == 20 );

	_root.set("node_2.node_2_1.node_2_1_1.int", 14);
	ASSERT_TRUE( _root.get<int> ("node_2.node_2_1.node_2_1_1.int") == 14);

	_root.set("node_3.int", 14);
	ASSERT_TRUE( _root.get<int> ("node_3.int") == 14 );
}

TEST_F( PropertyNode, set_bad_type_int ) {
	ASSERT_THROW ( _root.set("str",10), std::runtime_error );
	ASSERT_THROW ( _root.set("bool",10), std::runtime_error );
}

TEST_F( PropertyNode, set_bad_type_str ) {
	ASSERT_THROW ( _root.set("int",std::string("val")), std::runtime_error );
	ASSERT_THROW ( _root.set("bool",std::string("val")), std::runtime_error );
}

TEST_F( PropertyNode, set_bad_type_bool ) {
	ASSERT_THROW ( _root.set("int",false), std::runtime_error );
	ASSERT_THROW ( _root.set("str",false), std::runtime_error );
}

TEST_F( PropertyNode, hasChild ) {
	ASSERT_TRUE( _root.hasChild("node_1") );
	ASSERT_TRUE( _root.hasChild("node_2") );
	ASSERT_TRUE( _root.hasChild("node_3") );
	ASSERT_FALSE( _root.hasChild("noNode") );

	ASSERT_TRUE( _root("node_1").hasChild("node_1_1") );
	ASSERT_FALSE( _root("node_1").hasChild("noNode") );

	ASSERT_TRUE( _root("node_2").hasChild("node_2_1") );
	ASSERT_TRUE( _root("node_2").hasChild("node_2_2") );
	ASSERT_FALSE( _root("node_2").hasChild("noNode") );

	ASSERT_FALSE( _root("node_3").hasChild("noNode") );
}

TEST_F( PropertyNode, hasValue ) {
	ASSERT_TRUE( _root.hasValue("str") );
	ASSERT_TRUE( _root.hasValue("int") );
	ASSERT_TRUE( _root.hasValue("bool") );
	ASSERT_FALSE( _root.hasValue("noProp") );
}

TEST_F( PropertyNode, existsNode ) {
	ASSERT_FALSE( _root.existsNode("noNode") );
	ASSERT_TRUE( _root.existsNode("node_1") );
	ASSERT_TRUE( _root.existsNode("node_1.node_1_1") );
	ASSERT_FALSE( _root.existsNode("node_1.noNode") );
	ASSERT_TRUE( _root.existsNode("node_2") );
	ASSERT_TRUE( _root.existsNode("node_2") );
	ASSERT_TRUE( _root.existsNode("node_2.node_2_1") );
	ASSERT_TRUE( _root.existsNode("node_2.node_2_2") );
	ASSERT_TRUE( _root.existsNode("node_2.node_2_1.node_2_1_1") );
	ASSERT_TRUE( _root.existsNode("node_3") );
	ASSERT_FALSE( _root.existsNode("node_3.noNode") );
	ASSERT_TRUE( _root.existsNode("node_4") );
	ASSERT_FALSE( _root.existsNode("node_4.noNode") );
}

TEST_F( PropertyNode, existsValue ) {
	ASSERT_TRUE( _root.existsValue("str") );
	ASSERT_TRUE( _root.existsValue("int") );
	ASSERT_TRUE( _root.existsValue("bool") );
	ASSERT_FALSE( _root.existsValue("noValue") );
	
	ASSERT_TRUE( _root.existsValue("node_1.str") );
	ASSERT_TRUE( _root.existsValue("node_1.int") );
	ASSERT_TRUE( _root.existsValue("node_1.bool") );
	ASSERT_FALSE( _root.existsValue("node_1.noValue") );
	
	ASSERT_TRUE( _root.existsValue("node_1.node_1_1.str") );
	ASSERT_TRUE( _root.existsValue("node_1.node_1_1.bool") );
	ASSERT_TRUE( _root.existsValue("node_1.node_1_1.int") );
	ASSERT_FALSE( _root.existsValue("node_1.node_1_1.noValue") );

	ASSERT_TRUE( _root.existsValue("node_2.str") );
	ASSERT_TRUE( _root.existsValue("node_2.bool") );
	ASSERT_TRUE( _root.existsValue("node_2.int") );
	ASSERT_FALSE( _root.existsValue("node_2.noValue") );

	ASSERT_TRUE( _root.existsValue("node_2.str") );
	ASSERT_TRUE( _root.existsValue("node_2.bool") );
	ASSERT_TRUE( _root.existsValue("node_2.int") );
	ASSERT_FALSE( _root.existsValue("node_2.noValue") );
	
	ASSERT_FALSE( _root.existsValue("node_2.node_2_2.str") );

	ASSERT_TRUE( _root.existsValue("node_2.node_2_1.node_2_1_1.str") );
	ASSERT_TRUE( _root.existsValue("node_2.node_2_1.node_2_1_1.bool") );
	ASSERT_TRUE( _root.existsValue("node_2.node_2_1.node_2_1_1.int") );
	ASSERT_FALSE( _root.existsValue("node_2.node_2_1.node_2_1_1.noValue") );

	ASSERT_TRUE( _root.existsValue("node_3.str") );
	ASSERT_TRUE( _root.existsValue("node_3.bool") );
	ASSERT_TRUE( _root.existsValue("node_3.int") );
	ASSERT_FALSE( _root.existsValue("node_3.noValue") );

	ASSERT_FALSE( _root.existsValue("node_4.noValue") );
}

TEST_F( PropertyNode, setStr_string ) {
	_root.setStr("str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("str") == "someValue");

	_root.setStr("node_1.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_1.str") == "someValue");

	_root.setStr("node_1.node_1_1.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_1.node_1_1.str") == "someValue");
	
	_root.setStr("node_2.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_2.str") == "someValue");

	_root.setStr("node_2.node_2_1.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_2.node_2_1.str") == "someValue");

	_root.setStr("node_2.node_2_1.node_2_1_1.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_2.node_2_1.node_2_1_1.str") == "someValue");

	_root.setStr("node_3.str", "someValue");
	ASSERT_TRUE( _root.get<std::string> ("node_3.str") == "someValue");
}

TEST_F( PropertyNode, setStr_bool ) {
	_root.setStr("bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("bool") );

	_root.setStr("node_1.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_1.bool") );

	_root.setStr("node_1.node_1_1.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_1.node_1_1.bool") );
	
	_root.setStr("node_2.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_2.bool") );

	_root.setStr("node_2.node_2_1.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_2.node_2_1.bool") );

	_root.setStr("node_2.node_2_1.node_2_1_1.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_2.node_2_1.node_2_1_1.bool") );

	_root.setStr("node_3.bool", "false");
	 ASSERT_FALSE( _root.get<bool> ("node_3.bool") );

	_root.setStr("bool", "true");
	ASSERT_TRUE( _root.get<bool> ("bool") );

	_root.setStr("node_1.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_1.bool") );

	_root.setStr("node_1.node_1_1.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_1.node_1_1.bool") );
	
	_root.setStr("node_2.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_2.bool") );

	_root.setStr("node_2.node_2_1.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_2.node_2_1.bool") );

	_root.setStr("node_2.node_2_1.node_2_1_1.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_2.node_2_1.node_2_1_1.bool") );

	_root.setStr("node_3.bool", "true");
	ASSERT_TRUE( _root.get<bool> ("node_3.bool") );
}

TEST_F( PropertyNode, setStr_int ) {
	_root.setStr("int", "10");
	 ASSERT_TRUE( _root.get<int> ("int")  == 10 );

	_root.setStr("node_1.int", "10");
	 ASSERT_TRUE( _root.get<int> ("node_1.int") == 10 );

	_root.setStr("node_1.node_1_1.int", "10");
	 ASSERT_TRUE( _root.get<int> ("node_1.node_1_1.int") == 10 );
	
	_root.setStr("node_2.int", "30");
	 ASSERT_TRUE( _root.get<int> ("node_2.int") == 30 );

	_root.setStr("node_2.node_2_1.int", "120");
	 ASSERT_TRUE( _root.get<int> ("node_2.node_2_1.int") == 120 );

	_root.setStr("node_2.node_2_1.node_2_1_1.int", "10");
	 ASSERT_TRUE( _root.get<int> ("node_2.node_2_1.node_2_1_1.int") == 10 );

	_root.setStr("node_3.int", "10");
	 ASSERT_TRUE( _root.get<int> ("node_3.int") == 10);
}

TEST_F( PropertyNode, setStr_bad_cast ) {
	ASSERT_THROW ( _root.setStr("node_3.bool", "20"), std::runtime_error );
	ASSERT_TRUE( _root.get<bool> ("node_3.bool"));

	ASSERT_THROW ( _root.setStr("node_3.int", "true"), std::runtime_error );
	ASSERT_TRUE( _root.get<int>("node_3.int") != 0 );
}

TEST_F( PropertyNode, getDefault ) {
	ASSERT_TRUE( _root.getDefault<bool> ("bool") );
	_root.set("bool", false);
	ASSERT_TRUE( _root.getDefault<bool> ("bool") );
}

TEST_F( PropertyNode, asString_bool ) {
	ASSERT_TRUE( _root.asString("bool") == "true" );
	_root.set("bool", false);
	ASSERT_TRUE( _root.asString("bool") == "false" );
}

TEST_F( PropertyNode, asString_str ) {
	ASSERT_TRUE( _root.asString("str") == "val" );
	_root.set("str", "someValue");
	ASSERT_TRUE( _root.asString("str") == "someValue" );
}

TEST_F( PropertyNode, asString_int ) {
	ASSERT_TRUE( _root.asString("int") == "0" );
	_root.set("int", 20);
	ASSERT_TRUE( _root.asString("int") == "20" );
}

TEST_F( PropertyNode, defaultAsString_int ) {
	ASSERT_TRUE( _root.asString("int") == "0" );
	_root.set("int", 20);
	ASSERT_TRUE( _root.defaultAsString("int") == "0" );
}

TEST_F( PropertyNode, defaultAsString_bool ) {
	ASSERT_TRUE( _root.asString("bool") == "true" );
	_root.set("bool", false);
	ASSERT_TRUE( _root.defaultAsString("bool") == "true" );
}

TEST_F( PropertyNode, defaultAsString_str ) {
	ASSERT_TRUE( _root.asString("str") == "val" );
	_root.set("str", "someValue");
	ASSERT_TRUE( _root.defaultAsString("str") == "val" );
}

TEST_F( PropertyNode, reset_bool ) {
	_root.set("bool", false);
	_root.reset("bool");
	ASSERT_TRUE( _root.get<bool> ("bool") );
}

TEST_F( PropertyNode, reset_int ) {
	_root.set("int", 10);
	_root.reset("int");
	ASSERT_TRUE( _root.get<int> ("int") == 0 );
}

TEST_F( PropertyNode, reset_str ) {
	_root.set("str", "str");
	_root.reset("str");
	ASSERT_TRUE( _root.get<std::string> ("str") == "val");
}

struct ValVisitor {
	ValVisitor() {
		_cantVisits = 0;
	}

	void operator()( const util::cfg::PropertyValue *val ) {
		_path+= val->name();
		_path+= ".";
		_cantVisits += 1;
	}
	int _cantVisits;
	std::string _path;
};

TEST_F( PropertyNode, visitVals ) {
	ValVisitor *v = new ValVisitor();

	ASSERT_TRUE( v->_cantVisits == 0 );

	_root.visitValues( *v );

	ASSERT_TRUE( v->_cantVisits == 3 );
	ASSERT_TRUE( v->_path == "bool.int.str." );

	delete ( v );
}

struct Visitor3 {
	Visitor3 () {
		_cantVisits = 0;
	}

	void operator()( const util::cfg::PropertyNode * /*node*/ ) {
		_cantVisits += 1;
	}
	int _cantVisits;
};

TEST_F( PropertyNode, visitNodes ) {
	Visitor3 *v = new Visitor3();

	ASSERT_TRUE( v->_cantVisits == 0 );

	_root.visitNodes(*v);
	ASSERT_TRUE( v->_cantVisits == 4 );

	delete ( v );
}

struct NodeListener {
	NodeListener () {
		_cantCalls = 0;
	}

	void count( const std::string &str ) {
		_opt  = str;
		_cantCalls += 1;
	}
	int _cantCalls;
	std::string _opt;
};

TEST_F( PropertyNode, listen ) {
	NodeListener *l1 = new NodeListener();
	NodeListener *l2 = new NodeListener();

	_root.setStr("bool","false");
	ASSERT_TRUE( l1->_cantCalls == 0 );

	_root.listen( "bool", boost::bind( &NodeListener::count, l1, _1 ) );
	_root.listen( "str", boost::bind( &NodeListener::count, l2, _1 ) );
	_root.listen( "int", boost::bind( &NodeListener::count, l2, _1 ) );

	_root.setStr("bool","true");
	ASSERT_TRUE( l1->_cantCalls == 1 );
	ASSERT_TRUE( l1->_opt == "bool" );

	_root.setStr("str","val1");
	ASSERT_TRUE( l2->_cantCalls == 1 );
	ASSERT_TRUE( l2->_opt == "str" );

	_root.setStr("str", "val2");
	ASSERT_TRUE( l2->_cantCalls == 2 );
	ASSERT_TRUE( l2->_opt == "str" );

	_root.setStr("int", "20");
	ASSERT_TRUE( l2->_cantCalls == 3 );
	ASSERT_TRUE( l2->_opt == "int" );

	delete ( l1 );
	delete ( l2 );
}


struct PNBoolValidator {
	PNBoolValidator() {}
	~PNBoolValidator() {}
	bool operator()( const bool &value ) {
		// only the true value is allowed for the property
		return value;
	}
};

TEST_F( PropertyNode, set_bool_with_validation ) {
	PNBoolValidator validator;
	_root.setValidator<PNBoolValidator, bool>("bool", validator );
	_root.setValidator<PNBoolValidator, bool>("node_1.bool", validator );

	_root.set("bool",false);
	ASSERT_TRUE( _root.get<bool>("bool") );
	_root.set("node_1.bool",false);
	ASSERT_TRUE( _root.get<bool>("node_1.bool") );

	_root.set("bool",true);
	ASSERT_TRUE( _root.get<bool>("bool") );
	_root.set("node_1.bool",true);
	ASSERT_TRUE( _root.get<bool>("node_1.bool") );
}

struct PNStrValidator {
	PNStrValidator() {}
	~PNStrValidator() {}
	bool operator()( const std::string &value ) {
		bool res = false;
		if (value == "valid" || value == "str") {
			res = true;
		}
		return res;
	}
};

TEST_F( PropertyNode, set_str_with_validation ) {
	PNStrValidator validator;
	_root.setValidator<PNStrValidator, std::string>("str", validator );
	_root.setValidator<PNStrValidator, std::string>("node_1.str", validator );

	_root.set("str","valid");
	ASSERT_TRUE( _root.get<std::string>("str") == "valid" );
	_root.set("node_1.str","valid");
	ASSERT_TRUE( _root.get<std::string>("node_1.str") == "valid" );

	_root.set("str","str");
	ASSERT_TRUE( _root.get<std::string>("str") == "str" );
	_root.set("node_1.str","str");
	ASSERT_TRUE( _root.get<std::string>("node_1.str") == "str" );
	
	_root.set("str","invalid");
	ASSERT_FALSE( _root.get<std::string>("str") == "invalid" );
	_root.set("node_1.str","invalid");
	ASSERT_FALSE( _root.get<std::string>("node_1.str") == "invalid" );
}

struct PNIntValidator {
	PNIntValidator() {}
	~PNIntValidator() {}
	bool operator()( const int &value ) {
		bool res = false;
		if (value == 12 || value == 16) {
			res = true;
		}
		return res;
	}
};

TEST_F( PropertyNode, set_int_with_validation ) {
	PNIntValidator validator;
	_root.setValidator<PNIntValidator, int>("int", validator );
	_root.setValidator<PNIntValidator, int>("node_1.int", validator );

	_root.set("int",12);
	ASSERT_TRUE( _root.get<int>("int") == 12 );
	_root.set("node_1.int",12);
	ASSERT_TRUE( _root.get<int>("node_1.int") == 12 );

	_root.set("int",16);
	ASSERT_TRUE( _root.get<int>("int") == 16 );
	_root.set("node_1.int",16);
	ASSERT_TRUE( _root.get<int>("node_1.int") == 16 );
	
	_root.set("int",15);
	ASSERT_FALSE( _root.get<int>("int") == 15 );
	_root.set("node_1.int",15);
	ASSERT_FALSE( _root.get<int>("node_1.int") == 15 );
}
