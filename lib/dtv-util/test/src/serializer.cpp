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

#include "../../src/serializer/binary.h"
#include "../../src/serializer/xml.h"
#include "../../src/buffer.h"
#include "../../src/mcr.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#define SERIALIZER_XML_TEST "serializer_test.xml"
namespace fs = boost::filesystem;

static util::Buffer _buf;

class TestXMLToBuffer : public util::serialize::XML {
public:
	explicit TestXMLToBuffer( util::Buffer *buffer ) :util::serialize::XML(buffer) {}
	virtual ~TestXMLToBuffer() {}
};

template <class T>
T* create();

template<>
TestXMLToBuffer *create<TestXMLToBuffer>() {
	_buf.resize(0);
	return new TestXMLToBuffer( &_buf );
}

template<>
util::serialize::XML *create<util::serialize::XML>() {
	return new util::serialize::XML( SERIALIZER_XML_TEST );
}

template<>
util::serialize::Binary *create<util::serialize::Binary>() {
	_buf.resize(0);
	return new util::serialize::Binary(_buf);
}

template <typename T>
class SerializerTest : public ::testing::Test {
public:
	SerializerTest() {}
	virtual ~SerializerTest() {}

protected:
	virtual void SetUp() {
		_s = create<T>();
	}

	virtual void TearDown() {
		DEL(_s);
		if (fs::exists( SERIALIZER_XML_TEST )) {
			fs::remove( SERIALIZER_XML_TEST );
		}
	}

	T *_s;
};

TYPED_TEST_CASE_P(SerializerTest);

namespace impl {

enum type {
	uno,
	dos
};

typedef int AnInt;

class Integer {
public:
	Integer() {
		data_int = 0;
		data_bool = false;
		data_typedef = 0;
		data_enum = uno;
		data_uchar = 0;
		data_char = 0;
	}

	bool operator==( const Integer &other ) const {
		return
		data_string == other.data_string &&
		data_int == other.data_int &&
		data_bool == other.data_bool &&
		data_typedef == other.data_typedef &&
		data_enum  == other.data_enum &&
		data_uchar == other.data_uchar &&
		data_char == other.data_char;		
	}

	template<typename S>
	void serialize( S &s ) {
		s.serialize( "data_string", data_string );
		s.serialize( "data_int", data_int );
		s.serialize( "data_bool", data_bool );
		s.serialize( "data_typedef", data_typedef );
		s.serialize( "data_enum", data_enum );
		s.serialize( "data_uchar", data_uchar );
		s.serialize( "data_char", data_char );		
	}

	void show() {
		LDEBUG( "test", "Integer: str=%s, int=%d, bool=%d, typedef=%d, enum=%d, uchar=%d, char=%d",
			data_string.c_str(), data_int, data_bool, data_typedef, data_enum, data_uchar, data_char );
	}

	//	data
	std::string data_string;
	int data_int;
	bool data_bool;
	AnInt data_typedef;
	enum type data_enum;
	unsigned char data_uchar;
	char data_char;	
};

class Object {
public:
	void show() {
		LDEBUG( "test", "Object: str=%s, int=%d, bool=%d, typedef=%d, enum=%d, uchar=%d, char=%d",
			data_string.c_str(), data_int, data_bool, data_typedef, data_enum, data_uchar, data_char );
	}

	bool operator==( const Object &other ) const {
		return
		data_string == other.data_string &&
		data_int == other.data_int &&
		data_bool == other.data_bool &&
		data_typedef == other.data_typedef &&
		data_enum  == other.data_enum &&
		data_uchar == other.data_uchar &&
		data_char == other.data_char;		
	}

	//	data
	std::string data_string;
	int data_int;
	bool data_bool;
	AnInt data_typedef;
	enum type data_enum;
	unsigned char data_uchar;
	char data_char;	
};

class Nested {
public:
	Nested() {
		obj.data_string = "";
		obj.data_int = 0;
		obj.data_bool = false;
		obj.data_typedef = 0;
		obj.data_enum  = uno;
		obj.data_uchar = 0;
		obj.data_char = 0;
	}

	template<typename S>
	void serialize( S &s ) {
		s.serialize( "nested", obj );
	}

	bool operator==( const Nested &other ) const {
		return obj == other.obj;
	}

	Object obj;
};

bool comp( const int &a, const int &b ) {
	return a == b;
}

bool comp( const std::string &a, const std::string &b ) {
	return a == b;
}

bool comp( const Object *a, const Object *b ) {
	return (*a) == (*b);
}

template<typename T>
class Vector {
public:
	template<typename S>
	void serialize( S &s ) {
		s.serialize( "vector", vec );
	}

	bool operator==( const Vector &other ) const {
		bool ret = vec.size() == other.vec.size();
		if (ret) {
			for (size_t i=0; i<vec.size(); i++) {
				ret &= comp(vec[i], other.vec[i]);
			}
		}
		return ret;
	}

	std::vector<T> vec;
};

}

namespace util {
namespace serialize {

template<class Marshall>
struct Serialize<Marshall, impl::Object> {
	void operator()( Marshall &m, impl::Object &data ) {
		m.serialize( "data_string", data.data_string );
		m.serialize( "data_int", data.data_int );
		m.serialize( "data_bool", data.data_bool );
		m.serialize( "data_typedef", data.data_typedef );
		m.serialize( "data_enum", data.data_enum );
		m.serialize( "data_uchar", data.data_uchar );
		m.serialize( "data_char", data.data_char );		
	}
};

}
}

TYPED_TEST_P( SerializerTest, with_serializer) {
	impl::Integer write;
	write.data_int = 5;
	write.data_string = "pepe";
	write.data_bool = true;
	write.data_enum = impl::dos;
	write.data_typedef = 15;
	ASSERT_TRUE( this->_s->save(write) );

	impl::Integer read;
	ASSERT_TRUE( this->_s->load(read) );

	// write.show();
	// read.show();

	ASSERT_EQ( write, read );
}

TYPED_TEST_P( SerializerTest, without_serializer ) {
	impl::Object write;
	write.data_int = 5;
	write.data_string = "pepe";
	write.data_bool = true;
	write.data_enum = impl::dos;
	write.data_typedef = 15;
	ASSERT_TRUE( this->_s->save(write) );

	impl::Object read;
	ASSERT_TRUE( this->_s->load(read) );

	// write.show();
	// read.show();

	ASSERT_EQ( write, read );
}

#define MAX_ITEMS 10
TYPED_TEST_P( SerializerTest, vector_with_serializer) {
	std::vector<impl::Integer*> write;
	for (int i=0; i<MAX_ITEMS; i++) {
		impl::Integer *tmp = new impl::Integer();
		tmp->data_int = i;
		write.push_back( tmp );
	}

	ASSERT_TRUE( this->_s->save(write) );

	std::vector<impl::Integer*> read;
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( MAX_ITEMS, read.size() );
	for (size_t i=0; i<MAX_ITEMS; i++) {
		// write[i]->show();
		// read[i]->show();
		ASSERT_EQ( *write[i], *read[i] );
	}
}

TYPED_TEST_P( SerializerTest, vector_without_serializer) {
	std::vector<impl::Object*> write;
	for (int i=0; i<MAX_ITEMS; i++) {
		impl::Object *tmp = new impl::Object();
		tmp->data_int = i;
		write.push_back( tmp );
	}

	ASSERT_TRUE( this->_s->save(write) );

	std::vector<impl::Object*> read;
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( MAX_ITEMS, read.size() );
	for (size_t i=0; i<MAX_ITEMS; i++) {
		// write[i]->show();
		// read[i]->show();
		ASSERT_EQ( *write[i], *read[i] );
	}
}

TYPED_TEST_P( SerializerTest, nested_objects ) {
	impl::Nested write, read;
	write.obj.data_string = "Nested";
	write.obj.data_int = 1;
	write.obj.data_bool = true;
	write.obj.data_typedef = 2;
	write.obj.data_enum  = impl::dos;
	write.obj.data_uchar = 3;
	write.obj.data_char = 4;

	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write, read );
}

TYPED_TEST_P( SerializerTest, vector_fundamental ) {
	impl::Vector<int> write, read;

	for (int i=0; i<10; i++) {
		write.vec.push_back(i);
	}
	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write, read );
}

TYPED_TEST_P( SerializerTest, vector_string ) {
	impl::Vector<std::string> write, read;

	write.vec.push_back("uno");
	write.vec.push_back("dos");
	write.vec.push_back("tres");
	write.vec.push_back("cuatro");
	write.vec.push_back("cinco");
	write.vec.push_back("seis");
	write.vec.push_back("siete");
	write.vec.push_back("ocho");
	write.vec.push_back("nueve");

	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write, read );
}

TYPED_TEST_P( SerializerTest, vector_object_ptr ) {
	impl::Vector<impl::Object*> write, read;

	for (int i=0; i<10; i++) {
		impl::Object *obj = new impl::Object();
		obj->data_string = std::string("obj");
		obj->data_int = i;
		obj->data_bool = (i % 2) == 0;
		obj->data_typedef = i;
		obj->data_enum  = impl::dos;
		obj->data_uchar = 3;
		obj->data_char = 4;
		write.vec.push_back(obj);
	}

	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write, read );

	CLEAN_ALL(impl::Object*, write.vec);
	CLEAN_ALL(impl::Object*, read.vec);
}

TYPED_TEST_P( SerializerTest, save_load_vector_fundamental ) {
	std::vector<int> write, read;
	for (int i=0; i<10; i++) {
		write.push_back(i);
	}
	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write.size(), read.size() );
	for (size_t i=0; i<read.size(); i++) {
		ASSERT_EQ(write[i], read[i]);
	}
}

TYPED_TEST_P( SerializerTest, save_load_vector_string ) {
	std::vector<std::string> write, read;
	write.push_back("uno");
	write.push_back("dos");
	write.push_back("tres");
	write.push_back("cuatro");
	write.push_back("cinco");
	write.push_back("seis");
	write.push_back("siete");
	write.push_back("ocho");
	write.push_back("nueve");

	ASSERT_TRUE( this->_s->save(write) );
	ASSERT_TRUE( this->_s->load(read) );

	ASSERT_EQ( write.size(), read.size() );
	for (size_t i=0; i<read.size(); i++) {
		ASSERT_EQ(write[i], read[i]);
	}
}

REGISTER_TYPED_TEST_CASE_P(	SerializerTest,
	with_serializer,
	without_serializer,
	vector_with_serializer,
	vector_without_serializer,
	nested_objects,
	vector_fundamental,
	vector_string,
	vector_object_ptr,
	save_load_vector_fundamental,
	save_load_vector_string
);

typedef ::testing::Types<util::serialize::XML, util::serialize::Binary, TestXMLToBuffer> SerializerTypes;
INSTANTIATE_TYPED_TEST_CASE_P(
	Instances,      // Instance name
	SerializerTest, // Test case name
	SerializerTypes // Type list
);
