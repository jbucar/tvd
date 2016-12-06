/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once 

#include "types.h"
#include "../lua.h"
#include <util/serializer/serializer.h>
#include <string>
#include <vector>
#include <sstream>

namespace player {

namespace persistent {
	class NilData;
	class TableData;
}

namespace lua {

void pushValue( lua_State *st, const std::string &index, persistent::NilData & /*data*/ );
void pushValue( lua_State *st, const std::string &index, persistent::TableData &table );

} // end namespace lua

namespace persistent {

class DBDataLoader;
class DBDataVisitor;

class DBData {
public:
	DBData( dbdata::type type, const std::string &key );
	virtual ~DBData();

	virtual void push( lua_State * st )=0;
	virtual DBData *getCopy() const =0;
	virtual void visit( DBDataVisitor &visitor )=0;

	const std::string &key() const;
	std::string &mutableKey();
	const dbdata::type &type() const;

	static DBData *create( dbdata::type type, const std::string &key );

private:
	dbdata::type _type;
	std::string _key;
};

template<typename T>
class DBDataImpl : public DBData {
public:
	DBDataImpl( dbdata::type type, const std::string &key, const T &value ) : DBData(type, key), _data(value) {}
	virtual ~DBDataImpl() {}

	virtual void push( lua_State * st ) {
		player::lua::pushValue( st, key(), _data );
	}

	virtual DBData *getCopy() const {
		return new DBDataImpl<T>( type(), key(), _data );
	}

	virtual void visit( DBDataVisitor &visitor ) {
		visitor( _data );
	}

	T &data() {
		return _data;
	}

private:
	T _data;
};

class NilData {
public:
	template<class Marshall>
	void serialize( Marshall &m ) {
		int n = 0;
		m.serialize( "nil", n );
	}
};

class TableData {
public:
	TableData();
	TableData( const TableData &other );
	TableData( lua_State *st, int idx );
	virtual ~TableData();

	TableData &operator=( const TableData &other );
	DBData *operator[]( int pos );

	virtual void push( lua_State * st, const std::string &key );

	std::vector<DBDataLoader*> &table();
	void table( const std::vector<DBDataLoader*> &t );

	unsigned size() const;

	void add( DBData *data );

private:
	std::vector<DBDataLoader*> _table;
};

class DBDataVisitor {
public:
	virtual void operator()( int &data )=0;
	virtual void operator()( bool &data )=0;
	virtual void operator()( std::string &data )=0;
	virtual void operator()( NilData &data )=0;
	virtual void operator()( TableData &data )=0;
};

template<class Marshall>
class DBDataVisitorImpl : public DBDataVisitor {
public:
	DBDataVisitorImpl( Marshall &serializer ) : _serializer(serializer) {}

	virtual void operator()( int &data ) {
		_serializer.serialize( "data", data );
	}

	virtual void operator()( bool &data ) {
		_serializer.serialize( "data", data );
	}

	virtual void operator()( std::string &data ) {
		_serializer.serialize( "data", data );
	}

	virtual void operator()( NilData &data ) {
		_serializer.serialize( "data", data );
	}

	virtual void operator()( TableData &data ) {
		_serializer.serialize( "data", data.table() );
	}

private:
	Marshall &_serializer;
	DBDataVisitorImpl & operator=( const DBDataVisitorImpl & ) {}
};

class DBDataLoader {
public:
	DBDataLoader() : _data(NULL) {}
	virtual ~DBDataLoader() { _data = NULL; }

	DBData *data()  { return _data; }
	void data( DBData *data ) { _data = data; }

	virtual DBDataLoader *getCopy() const {
		DBDataLoader *tmp = new DBDataLoader();
		tmp->data( _data->getCopy() );
		return tmp;
	}
private:
	DBData *_data;
};

} // end namespace persistent
} // end namespace player

namespace util {
namespace serialize {

template<class Marshall>
struct Serialize<Marshall, player::persistent::DBDataLoader> {
	void operator()( Marshall &m, player::persistent::DBDataLoader &loader ) {
		player::persistent::DBDataVisitorImpl<Marshall> visitor( m );
		if (loader.data()) {
			// Save
			std::string type( player::persistent::dbdata::getName(loader.data()->type()) );
			m.serialize( "type", type );
			m.serialize( "key", loader.data()->mutableKey() );
		} else {
			// Load
			std::string key, typeName;
			m.serialize( "type", typeName );
			m.serialize( "key", key );
			player::persistent::dbdata::type type = player::persistent::dbdata::getType( typeName );
			loader.data( player::persistent::DBData::create(type, key) );
		}
		loader.data()->visit( visitor );
	}
};

template<class Marshall>
struct Serialize<Marshall, player::persistent::TableData> {
	void operator()( Marshall &m, player::persistent::TableData &table ) {
		m.serialize("table", table.table());
	}
};

} // end namespace serialize
} // end namespace util
