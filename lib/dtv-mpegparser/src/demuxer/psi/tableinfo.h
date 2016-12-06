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
#pragma once

#include "../../types.h"
#include <vector>
#include <algorithm>

namespace tuner {

class TableInfo {
public:
	TableInfo();
	virtual ~TableInfo();

	bool versionChanged( Version newVersion ) const;
	Version current() const;

	virtual bool checkSection( BYTE *section );
	void markSection( int first, int last );
	bool wasParsed( BYTE num ) const;
	bool isComplete() const;
	virtual void update( Version ver );
	bool expire( ID pid );

protected:
	size_t sections() const;
	void markSection( int sec );
	virtual void reset();

private:
	Version _current;
	Version _lastExpired;
	int _sections;
	std::vector<bool> _secsParsed;
};

template<typename T>
class TableImpl : public TableInfo {
public:
	TableImpl( const T &id  ) : _id( id ) {}
	virtual ~TableImpl( void ) {}

	const T &id() const { return _id; }

private:
	T _id;
};


namespace psi {

template<typename T, typename I>
struct TableFinder {
	TableFinder( const I &id ) : _id(id) {}
	bool operator()( T *table ) const {
		return table->id() == _id;
	}
	const I &_id;

private:
	// Avoid VisualStudio warning C4512
	TableFinder &operator=( TableFinder &/*tf*/ ) { return *this; }
};

template<typename T, typename I>
T *getTable( std::vector<T *> &tables, const I &id ) {
	T *result;
	TableFinder<T,I> finder(id);
	typename std::vector<T *>::const_iterator it=std::find_if( tables.begin(), tables.end(), finder );
	if (it != tables.end()) {
		result=(*it);
	}
	else {
		result=new T( id );
		tables.push_back( result );
	}
	return result;
}

}

}

