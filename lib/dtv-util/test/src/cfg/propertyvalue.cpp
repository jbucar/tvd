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

#include <string>
#include <gtest/gtest.h>
#include "../../../src/cfg/propertyvalue.h"

TEST( PropertyValue, constructor_with_desc ) {
	util::cfg::PropertyValue pval("Property", 1, "Desc");
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.description() == "Desc" );
	ASSERT_TRUE( pval.get<int> () == 1 );
}

TEST( PropertyValue, constructor_int_property ) {
	util::cfg::PropertyValue pval("Property", 1);
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.description() == "" );
	ASSERT_TRUE( pval.get<int> () == 1 );
}

TEST( PropertyValue, constructor_bool_property ) {
	util::cfg::PropertyValue pval("Property", false);
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.description() == "" );
	ASSERT_FALSE( pval.get<bool> () );
}

TEST( PropertyValue, constructor_string_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	ASSERT_TRUE( pval.description() == "" );
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.get<std::string> () == "val" );
}

TEST( PropertyValue, constructor_const_char_pointer ) {
	util::cfg::PropertyValue pval("Property", "val");
	ASSERT_TRUE( pval.description() == "" );
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.get<std::string> () == "val" );
}

TEST( PropertyValue, constructor_const_char_pointer_with_desc ) {
	util::cfg::PropertyValue pval("Property", "val","desc");
	ASSERT_TRUE( pval.description() == "desc" );
	ASSERT_TRUE( pval.name() == "Property" );
	ASSERT_TRUE( pval.get<std::string> () == "val" );
}

TEST( PropertyValue, get_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	ASSERT_TRUE( pval.get<int> () == 10 );
}

TEST( PropertyValue, get_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string ("str"));
	ASSERT_TRUE( pval.get<std::string> () == "str" );
}

TEST( PropertyValue, get_bool_property ) {
	util::cfg::PropertyValue pval("Property", true);
	ASSERT_TRUE( pval.get<bool> () );
}

TEST( PropertyValue, set_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	pval.set(11);
	ASSERT_TRUE( pval.get<int> () == 11 );
}

TEST( PropertyValue, set_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	pval.set(std::string("str"));
	ASSERT_TRUE( pval.get<std::string> () == "str" );
}

TEST( PropertyValue, set_bool_property ) {
	util::cfg::PropertyValue pval("Property", false);
	pval.set(true);
	ASSERT_TRUE( pval.get<bool> () );
}

TEST( PropertyValue, set_wrong_type_to_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	ASSERT_THROW( pval.set(std::string("str")), std::runtime_error);
	ASSERT_THROW( pval.set(true), std::runtime_error);
}

TEST( PropertyValue, set_wrong_type_to_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	ASSERT_THROW( pval.set(10), std::runtime_error);
	ASSERT_THROW( pval.set(true), std::runtime_error);
}

TEST( PropertyValue, set_wrong_type_to_bool_property ) {
	util::cfg::PropertyValue pval("Property", false);
	ASSERT_THROW( pval.set(std::string("str")), std::runtime_error);
	ASSERT_THROW( pval.set(10), std::runtime_error);
}

TEST( PropertyValue, get_wrong_type_for_bool_property ) {
	util::cfg::PropertyValue pval("Property", true);

	ASSERT_THROW(pval.get<float> (), std::runtime_error);
	ASSERT_THROW(pval.get<int> (), std::runtime_error);
	ASSERT_THROW(pval.get<std::string> (), std::runtime_error);
}

TEST( PropertyValue, get_wrong_type_for_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);

	ASSERT_THROW(pval.get<float> (), std::runtime_error);
	ASSERT_THROW(pval.get<bool> (), std::runtime_error);
	ASSERT_THROW(pval.get<std::string> (), std::runtime_error);
}

TEST( PropertyValue, get_wrong_type_for_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));

	ASSERT_THROW(pval.get<float> (), std::runtime_error);
	ASSERT_THROW(pval.get<int> (), std::runtime_error);
	ASSERT_THROW(pval.get<bool> (), std::runtime_error);
}

TEST( PropertyValue, asString_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	ASSERT_TRUE( pval.asString() == "10" );
	pval.set(20);
	ASSERT_TRUE( pval.asString() == "20" );
}

TEST( PropertyValue, asString_bool_property ) {
	util::cfg::PropertyValue pval("Property", true );
	ASSERT_TRUE( pval.asString() == "true" );
	pval.set(false);
	ASSERT_TRUE( pval.asString() == "false" );
}

TEST( PropertyValue, asString_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	ASSERT_TRUE( pval.asString() == "val" );
	pval.set(std::string("str"));
	ASSERT_TRUE( pval.asString() == "str" );
}

TEST( PropertyValue, asStringDefault_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	ASSERT_TRUE( pval.asStringDefault() == "10" );
	pval.set(20);
	ASSERT_TRUE( pval.asStringDefault() == "10" );
}

TEST( PropertyValue, asStringDefault_bool_property ) {
	util::cfg::PropertyValue pval("Property", true );
	ASSERT_TRUE( pval.asStringDefault() == "true" );
	pval.set(false);
	ASSERT_TRUE( pval.asStringDefault() == "true" );
}

TEST( PropertyValue, asStringDefault_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	ASSERT_TRUE( pval.asStringDefault() == "val" );
	pval.set(std::string("str"));
	ASSERT_TRUE( pval.asStringDefault() == "val" );
}

TEST( PropertyValue, setStr_bool_property ) {
	util::cfg::PropertyValue pval("Property", false);
	pval.setStr("true");
	ASSERT_TRUE( pval.get<bool> () );
	pval.setStr("false");
	ASSERT_FALSE( pval.get<bool> () );
	ASSERT_THROW(pval.setStr("anotherVal"), std::runtime_error);
}

TEST( PropertyValue, setStr_int_property ) {
	util::cfg::PropertyValue pval("Property", 10);
	pval.setStr("20");
	ASSERT_TRUE( pval.get<int> () == 20 );
	ASSERT_THROW(pval.setStr("anotherVal"), std::runtime_error);
}

TEST( PropertyValue, setStr_str_property ) {
	util::cfg::PropertyValue pval("Property", std::string("val"));
	pval.setStr("str");
	ASSERT_TRUE( pval.get<std::string> () == "str");
}

TEST( PropertyValue, isModified ) {
	util::cfg::PropertyValue pval("Property", 1);
	ASSERT_FALSE( pval.isModified() );
	pval.set(12);
	ASSERT_TRUE( pval.isModified() );

}

TEST( PropertyValue, reset ) {
	util::cfg::PropertyValue pval("Property", 1);
	pval.set(12);
	pval.reset();
	ASSERT_TRUE( pval.get<int> () == 1 );
}

struct Listener {
	Listener () {
		_cantCalls = 0;
	}

	void count( const std::string &str ) {
		_opt  = str;
		_cantCalls += 1;
	}
	int _cantCalls;
	std::string _opt;
};

TEST( PropertyValue, onChange ) {
	util::cfg::PropertyValue pval("Property",10);
	util::cfg::PropertyValue pval2("Property",10);

	Listener *l1 = new Listener();
	Listener *l2 = new Listener();

	pval.setStr("15");
	ASSERT_TRUE( l1->_cantCalls == 0 );

	pval.onChange( boost::bind( &Listener::count, l1, _1 ) );
	pval2.onChange( boost::bind( &Listener::count, l2, _1 ) );

	pval.setStr("15");
	ASSERT_TRUE( l1->_cantCalls == 1 );
	ASSERT_TRUE( l1->_opt == pval.name() );

	pval2.setStr("15");
	ASSERT_TRUE( l2->_cantCalls == 1 );
	ASSERT_TRUE( l2->_opt == pval2.name() );

	pval2.setStr("15");
	ASSERT_TRUE( l2->_cantCalls == 2 );
	ASSERT_TRUE( l2->_opt == pval2.name() );

	delete ( l1 );
	delete ( l2 );
}

struct IntValidator {
	IntValidator() {}
	~IntValidator() {}
	bool operator()( const int &value ) {
		printf("Int \n");
		bool res = false;
		if ( value == 10 || value == 20 || value == 35) {
			res = true;
		}
		return res;
	}
};

TEST( PropertyValue, set_int_with_validation ) {
	util::cfg::PropertyValue pval("Property", 5);

	IntValidator validator;
	pval.setValidator<IntValidator, int>( validator );

	pval.set(10);
	ASSERT_TRUE( pval.get<int>() == 10 );
	
	pval.set(20);
	ASSERT_TRUE( pval.get<int>() == 20 );
	
	pval.set(35);
	ASSERT_TRUE( pval.get<int>() == 35 );
	
	pval.set(40);
	ASSERT_FALSE( pval.get<int>() == 40 );
	
	pval.set(15);
	ASSERT_FALSE( pval.get<int>() == 15 );

}

struct StrValidator {
	StrValidator() {}
	~StrValidator() {}
	bool operator()( const std::string &value ) {
		printf("Pensando con %s \n", value.c_str());
		bool res = false;
		if ( value == "val1" || value == "val2" || value == "val3") {
			printf("Value %s : TRUE \n", value.c_str());
			res = true;
		}
		return res;
	}
};

TEST( PropertyValue, set_str_with_validation ) {
	util::cfg::PropertyValue pval("Property", "val");

	StrValidator validator;
	pval.setValidator<StrValidator, std::string>( validator );

	pval.set("val1");
	ASSERT_TRUE( pval.get<std::string>() == "val1" );
	
	pval.set("noAllowedVal");
	ASSERT_FALSE( pval.get<std::string>() == "noAllowedVal" );
	
	pval.set("val2");
	ASSERT_TRUE( pval.get<std::string>() ==  "val2" );
	
	pval.set("val3");
	ASSERT_TRUE( pval.get<std::string>() == "val3" );
	
	pval.set("val55");
	ASSERT_FALSE( pval.get<std::string>() == "val55" );

}

struct BoolValidator {
	BoolValidator() {}
	~BoolValidator() {}
	bool operator()( const bool &value ) {
		printf("Bool \n");
		// only the true value is allowed for the property
		return value;
	}
};

TEST( PropertyValue, set_bool_with_validation ) {
	util::cfg::PropertyValue pval("Property", true);

	BoolValidator validator;
	pval.setValidator<BoolValidator, bool>( validator );

	pval.set(false);
	ASSERT_TRUE( pval.get<bool>() );
	
	pval.set(true);
	ASSERT_TRUE( pval.get<bool>() );
}

