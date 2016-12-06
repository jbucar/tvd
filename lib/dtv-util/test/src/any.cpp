/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "../../src/any.h"
#include "../../src/types.h"
#include <gtest/gtest.h>


namespace any_test {
	static int constructor=0;
	static int destructor=0;
}

class AnyTest : public testing::Test
{
public:
	AnyTest()          {}
	virtual ~AnyTest() {}

	// Remember that SetUp() is run immediately before a test starts.
	virtual void SetUp() {
		_destructors = 0;
		any_test::constructor=0;
		any_test::destructor=0;
	}

	// TearDown() is invoked immediately after a test finishes.
	virtual void TearDown() {
		ASSERT_TRUE( any_test::destructor == _destructors );
	}
	int _destructors;
};


class PruebaAnySmall {
public:
	PruebaAnySmall( int x )
	{
		any_test::constructor++;
		_x = x;
	}

	PruebaAnySmall( void )
	{
		_x = 0;
		any_test::constructor++;
	}

	PruebaAnySmall( const PruebaAnySmall &other ) {
		any_test::constructor++;
		_x = other._x;
	}

	~PruebaAnySmall( void ) {
		any_test::destructor++;
	}

	PruebaAnySmall &operator=( const PruebaAnySmall &other ) {
		any_test::constructor++;
		_x = other._x;
		return *this;
	}

	PruebaAnySmall &operator=( const int &x ) {
		_x = x;
		return *this;
	}

	bool operator==( const int &value ) const {
		return _x == value;
	}

	bool operator==( const PruebaAnySmall &other ) const {
		return _x == other._x;
	}

private:
	int _x;
};

class PruebaAnyLarge {
public:
	PruebaAnyLarge( void )
	{
		memset(_data,'a',100);
		any_test::constructor++;
	}

	PruebaAnyLarge( int car )
	{
		memset(_data,car,100);
		any_test::constructor++;
	}

	PruebaAnyLarge( const PruebaAnyLarge &other ) {
		any_test::constructor++;
		memcpy(_data,other._data,100);
	}

	~PruebaAnyLarge( void ) {
		any_test::destructor++;
	}

	PruebaAnyLarge &operator=( const PruebaAnyLarge &other ) {
		any_test::constructor++;
		memcpy(_data,other._data,100);
		return *this;
	}

	void fill( int car ) {
		memset(_data,car,100);
	}

	bool operator==( const PruebaAnyLarge &other ) const {
		return !memcmp(_data,other._data,100);
	}

private:
	char _data[100];
};


TEST_F( AnyTest, template_constructors_small ) {
	PruebaAnySmall s(2);
	ASSERT_TRUE( any_test::constructor == 1 );

	//	Call constructor
	util::Any x(s);
	ASSERT_TRUE( any_test::constructor == 2 );

	//	Not empty
	ASSERT_FALSE( x.empty() );

	//	Check value not by reference!
	ASSERT_TRUE( s == 2 );
	s = 4;
	ASSERT_FALSE( x == 2 );

	ASSERT_TRUE( any_test::constructor == 2 );
	_destructors = 2;
}

TEST_F( AnyTest, template_constructors_large ) {
	PruebaAnyLarge data('c');
	ASSERT_TRUE( any_test::constructor == 1 );

	//	Call constructor
	util::Any x( data );
	ASSERT_TRUE( any_test::constructor == 2 );

	//	Not empty
	ASSERT_FALSE( x.empty() );
	ASSERT_TRUE( any_test::constructor == 2 );

	//	Check value not by reference!
	data.fill( 'd' );
	ASSERT_FALSE( x == data );

	ASSERT_TRUE( any_test::constructor == 2 );
	_destructors = 2;
}

TEST_F( AnyTest, default_constructors ) {
	util::Any x;
	ASSERT_TRUE( x.empty() );
	ASSERT_TRUE( any_test::constructor == 0 );
	_destructors = 0;
}

TEST_F( AnyTest, copy_constructors_small ) {
	PruebaAnySmall s;
	ASSERT_TRUE( any_test::constructor == 1 );

	util::Any x(s);
	ASSERT_TRUE( any_test::constructor == 2 );
	ASSERT_FALSE( x.empty() );

	util::Any y(x);
	ASSERT_TRUE( any_test::constructor == 3 );
	ASSERT_FALSE( y.empty() );

	ASSERT_TRUE( y == s );

	ASSERT_TRUE( any_test::constructor == 3 );
	_destructors = 3;
}

TEST_F( AnyTest, copy_constructors_large ) {
	PruebaAnyLarge s;
	ASSERT_TRUE( any_test::constructor == 1 );

	util::Any x(s);
	ASSERT_TRUE( any_test::constructor == 2 );
	ASSERT_FALSE( x.empty() );

	util::Any y(x);
	ASSERT_TRUE( any_test::constructor == 3 );
	ASSERT_FALSE( y.empty() );

	ASSERT_TRUE( y == s );

	ASSERT_TRUE( any_test::constructor == 3 );
	_destructors = 3;
}

TEST_F( AnyTest, queries_empty ) {
	util::Any x, y;
	ASSERT_TRUE( any_test::constructor == 0 );
	ASSERT_TRUE( x.empty() );	//	empty
	ASSERT_TRUE( y.empty() );	//	empty

	ASSERT_TRUE( x.type() == typeid(void *) );	//	type
	ASSERT_FALSE( x == 10 );	//	operator==( const ValueType &val )
	ASSERT_THROW( x.get<int>(), util::bad_any_cast );	//	get

	ASSERT_TRUE( any_test::constructor == 0 );
	_destructors = 0;
}

TEST_F( AnyTest, queries_simple_small ) {
	PruebaAnySmall s(10);
	ASSERT_TRUE( any_test::constructor == 1 );

	util::Any x;
	ASSERT_TRUE( any_test::constructor == 1 );
	ASSERT_TRUE( x.empty() );	//	empty

	x = s;
	ASSERT_TRUE( any_test::constructor == 2 );
	ASSERT_FALSE( x.empty() );	//	empty
	ASSERT_TRUE( x.type() == typeid(s) );	//	type

	ASSERT_TRUE( any_test::constructor == 2 );
	_destructors = 2;
}

TEST_F( AnyTest, queries_simple_large ) {
	PruebaAnyLarge s('a');
	ASSERT_TRUE( any_test::constructor == 1 );

	util::Any x;
	ASSERT_TRUE( any_test::constructor == 1 );
	ASSERT_TRUE( x.empty() );	//	empty

	x = s;
	ASSERT_TRUE( any_test::constructor == 2 );
	ASSERT_FALSE( x.empty() );	//	empty
	ASSERT_TRUE( x.type() == typeid(s) );	//	type

	ASSERT_TRUE( any_test::constructor == 2 );
	_destructors = 2;
}

//	operator==( const ValueType &val )
//		a) e s
//		b) e l
//		c) s s-equal
//		d) s s-nequal
//		e) l s
//		f) l l-equal
//		g) l l-nequal

//		a) e s
TEST_F( AnyTest, queries_equal_type_a ) {
	int cant=1;
	PruebaAnySmall t( 12 );
	util::Any e;

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_FALSE( e == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		b) e l
TEST_F( AnyTest, queries_equal_type_b ) {
	int cant=1;
	PruebaAnyLarge t( '5' );
	util::Any e;

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_FALSE( e == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		c) s s-equal
TEST_F( AnyTest, queries_equal_type_c ) {
	int cant=2;
	PruebaAnySmall t( 4 );
	util::Any a(t);

	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_TRUE( a == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		d) s s-nequal
TEST_F( AnyTest, queries_equal_type_d ) {
	int cant=3;
	PruebaAnySmall t1( 4 );
	PruebaAnySmall t2( 5 );
	util::Any a(t1);

	ASSERT_TRUE( a == t1 );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_FALSE( a == t2 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		e) l s
TEST_F( AnyTest, queries_equal_type_e ) {
	int cant=3;
	PruebaAnySmall t1( 4 );
	PruebaAnyLarge t2( 5 );
	util::Any a(t2);

	ASSERT_TRUE( a == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_FALSE( a == t1 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		f) l l-equal
TEST_F( AnyTest, queries_equal_type_f ) {
	int cant=2;
	PruebaAnyLarge t( 4 );
	util::Any a(t);

	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_TRUE( a == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		g) l l-nequal
TEST_F( AnyTest, queries_equal_type_g ) {
	int cant=3;
	PruebaAnyLarge t1( 4 );
	PruebaAnyLarge t2( 5 );
	util::Any a(t2);

	ASSERT_TRUE( a == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	ASSERT_FALSE( a == t1 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//	operator=( const Any &other )
//		a) e	e
//		b) e	s
//		c) e	l
//		d) s	e
//		e) s	s
//		f) s	l
//		g) l	e
//		h) l	s
//		i) l	l

//		a) e	e
TEST_F( AnyTest, copy_assign_any_a ) {
	int cant=0;
	util::Any e1, e2;

	ASSERT_TRUE( e1.empty() );
	ASSERT_TRUE( e2.empty() );

	e1 = e2;
	ASSERT_TRUE( e1.empty() );
	ASSERT_TRUE( e2.empty() );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		b) e	s
TEST_F( AnyTest, copy_assign_any_b ) {
	int cant=2;
	PruebaAnySmall t(11);
	util::Any e, a(t);

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	e = a;
	ASSERT_TRUE( e == t );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//		c) e	l
TEST_F( AnyTest, copy_assign_any_c ) {
	int cant=2;
	PruebaAnyLarge t(11);
	util::Any e, a(t);

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	e = a;
	ASSERT_TRUE( e == t );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//		d) s	e
TEST_F( AnyTest, copy_assign_any_d ) {
	int cant=2;
	PruebaAnySmall t(11);
	util::Any e, a(t);

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	a = e;
	ASSERT_TRUE( a.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	_destructors = cant;
}

//		e) s	s
TEST_F( AnyTest, copy_assign_any_e ) {
	int cant=4;
	PruebaAnySmall t1(11);
	PruebaAnySmall t2(12);
	util::Any a1(t1), a2(t2);

	ASSERT_TRUE( a1 == t1 );
	ASSERT_TRUE( a2 == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	a1 = a2;
	ASSERT_TRUE( a1 == t2 );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//		f) s	l
TEST_F( AnyTest, copy_assign_any_f ) {
	int cant=4;
	PruebaAnySmall t1(11);
	PruebaAnyLarge t2(12);
	util::Any a1(t1), a2(t2);

	ASSERT_TRUE( a1 == t1 );
	ASSERT_TRUE( a2 == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	a1 = a2;
	ASSERT_TRUE( a1 == t2 );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//		g) l	e
TEST_F( AnyTest, copy_assign_any_g ) {
	int cant=2;
	PruebaAnyLarge t(11);
	util::Any e, a(t);

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( a == t );
	ASSERT_TRUE( any_test::constructor == cant );

	a = e;
	ASSERT_TRUE( a.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	_destructors = cant;
}

//		h) l	s
TEST_F( AnyTest, copy_assign_any_h ) {
	int cant=4;
	PruebaAnySmall t1(11);
	PruebaAnyLarge t2(12);
	util::Any a1(t1), a2(t2);

	ASSERT_TRUE( a1 == t1 );
	ASSERT_TRUE( a2 == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	a2 = a1;
	ASSERT_TRUE( a2 == t1 );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//		i) l	l
TEST_F( AnyTest, copy_assign_any_i ) {
	int cant=4;
	PruebaAnyLarge t1(11);
	PruebaAnyLarge t2(12);
	util::Any a1(t1), a2(t2);

	ASSERT_TRUE( a1 == t1 );
	ASSERT_TRUE( a2 == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	a1 = a2;
	ASSERT_TRUE( a1 == t2 );
	ASSERT_TRUE( any_test::constructor == ++cant );

	_destructors = cant;
}

//	operator=( const T &x )
//		a) e   ts
//		b) e   tl
//		c) s   ts
//		d) s   tl
//		e) l   ts
//		f) l   tl

//		a) e   ts
TEST_F( AnyTest, copy_assign_type_a ) {
	int cant=1;
	PruebaAnySmall t( 11 );
	util::Any e;

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	e = t;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( e == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		b) e   tl
TEST_F( AnyTest, copy_assign_type_b ) {
	int cant=1;
	PruebaAnyLarge t( 11 );
	util::Any e;

	ASSERT_TRUE( e.empty() );
	ASSERT_TRUE( any_test::constructor == cant );

	e = t;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( e == t );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		c) s   ts
TEST_F( AnyTest, copy_assign_type_c ) {
	int cant=3;
	PruebaAnySmall t1( 11 );
	PruebaAnySmall t2( 13 );
	util::Any a(t1);

	ASSERT_TRUE( a == t1 );
	ASSERT_TRUE( any_test::constructor == cant );

	a = t2;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( a == t2 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		d) s   tl
TEST_F( AnyTest, copy_assign_type_d ) {
	int cant=3;
	PruebaAnySmall t1( 11 );
	PruebaAnyLarge t2( 13 );
	util::Any a(t1);

	ASSERT_TRUE( a == t1 );
	ASSERT_TRUE( any_test::constructor == cant );

	a = t2;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( a == t2 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		e) l   ts
TEST_F( AnyTest, copy_assign_type_e ) {
	int cant=3;
	PruebaAnySmall t1( 11 );
	PruebaAnyLarge t2( 13 );
	util::Any a(t2);

	ASSERT_TRUE( a == t2 );
	ASSERT_TRUE( any_test::constructor == cant );

	a = t1;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( a == t1 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

//		f) l   tl
TEST_F( AnyTest, copy_assign_type_f ) {
	int cant=3;
	PruebaAnyLarge t1( 11 );
	PruebaAnyLarge t2( 13 );
	util::Any a(t1);

	ASSERT_TRUE( a == t1 );
	ASSERT_TRUE( any_test::constructor == cant );

	a = t2;
	ASSERT_TRUE( any_test::constructor == ++cant );
	ASSERT_TRUE( a == t2 );

	ASSERT_TRUE( any_test::constructor == cant );
	_destructors = cant;
}

typedef std::pair<int,int> Pair;
TEST( AnyTypes, type_std_pair ) {
	Pair p(10,11);
	util::Any tmp( p );
	ASSERT_TRUE( tmp.get<Pair>() == p );
}

typedef std::vector<int> ListOfPairType;
TEST( AnyTypes, assign_list_other ) {
	ListOfPairType l1;
	l1.push_back( 2 );
	l1.push_back( 4 );
	util::Any a1( l1 );
	int value=15;
	util::Any a2( value );

	{
		const ListOfPairType &r1 = a1.get<ListOfPairType>();
		ASSERT_TRUE( r1.size() == 2 );
		ASSERT_TRUE( r1[0] == 2 );
		ASSERT_TRUE( r1[1] == 4 );

		const int &r2 = a2.get<int>();
		ASSERT_TRUE( r2 == 15 );
	}

	a2 = a1;
	{
		const ListOfPairType &r2 = a2.get<ListOfPairType>();
		ASSERT_TRUE( r2.size() == 2 );
		ASSERT_TRUE( r2[0] == 2 );
		ASSERT_TRUE( r2[1] == 4 );
	}
}

typedef std::vector<int> ListOfPairType;
TEST( AnyTypes, assign_list_same ) {
	ListOfPairType l1, l2;
	l1.push_back( 2 );
	l1.push_back( 4 );
	l2.push_back( 3 );
	l2.push_back( 5 );
	util::Any a1( l1 );
	util::Any a2( l2 );

	{
		const ListOfPairType &r1 = a1.get<ListOfPairType>();
		ASSERT_TRUE( r1.size() == 2 );
		ASSERT_TRUE( r1[0] == 2 );
		ASSERT_TRUE( r1[1] == 4 );

		const ListOfPairType &r2 = a2.get<ListOfPairType>();
		ASSERT_TRUE( r2.size() == 2 );
		ASSERT_TRUE( r2[0] == 3 );
		ASSERT_TRUE( r2[1] == 5 );
	}

	 a2 = a1;
	{
		const ListOfPairType &r2 = a2.get<ListOfPairType>();
		ASSERT_TRUE( r2.size() == 2 );
		ASSERT_TRUE( r2[0] == 2 );
		ASSERT_TRUE( r2[1] == 4 );
	}
}

