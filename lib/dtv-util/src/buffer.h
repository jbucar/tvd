/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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
#pragma once

#include <string>
#include <vector>
#include "types.h"

namespace util {

class Buffer {
public:
	//	Constructors
	Buffer();
	explicit Buffer( size_t blockSize );
	Buffer( const Buffer &other );
	Buffer( const void *buf, size_t len, bool owner=true );
	virtual ~Buffer();

	//	Operators
	Buffer &operator=( const Buffer &other );
	bool operator==( const Buffer &other ) const;

	//	Getters
	size_t length() const;
	bool empty() const;
	size_t capacity() const;
	size_t blockSize() const;
	void *data() const;
	BYTE *bytes() const;
	std::string asHexa() const;
	template<class Operand> inline void format( Operand &oper )const;

	//	Operations
	void resize( size_t len );
	void append( const void *buf, size_t len );
	void append( const Buffer &buf );
	void copy( const void *buf, size_t len );
	void copy( size_t pos, const void *buf, size_t len );
	void swap( void *buf, size_t len );
	void swap( Buffer &other );

	//	User data
	void setUserData( void *data );
	void *getUserData() const;

protected:
	void clear();
	void makeSpace( size_t len, bool copy );

private:
	void *_buf;
	bool _owner;
	size_t _length;
	size_t _bufLen;
	size_t _blockSize;
	void *_userData;
};

template<class Operand>
	inline void Buffer::format( Operand &oper ) const {
	BYTE *ptr = (BYTE *)_buf;
	for (size_t i=0; i<_length; ++i) {
		oper( ptr[i] );
	}
}

}

