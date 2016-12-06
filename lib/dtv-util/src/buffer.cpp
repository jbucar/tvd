/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV implementation.

    DTV is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV.

    DTV es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "buffer.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>

#define MAKE_SPACE(l,c) if (l > _bufLen) { makeSpace( l, c ); }

namespace util {

Buffer::Buffer()
{
	_buf       = NULL;
	_bufLen    = 0;
	_length    = 0;
	_blockSize = 0;
	_owner     = true;
	_userData  = NULL;
}

Buffer::Buffer( const Buffer &other )
{
	//	Always do buffer owner
	_buf       = NULL;
	_bufLen    = 0;
	_length    = 0;
	_blockSize = other._blockSize;
	_owner     = true;
	_userData  = other._userData;

	if (other._length) {
		copy( other._buf, other._length );
	}
	else {
		MAKE_SPACE( 1, false );
	}
}

Buffer::Buffer( size_t blockSize )
{
	_buf       = NULL;
	_bufLen    = 0;
	_length    = 0;
	_blockSize = blockSize;
	_owner     = true;
	_userData  = NULL;
	MAKE_SPACE( 1, false );	//	Force buffer allocation
}

Buffer::Buffer( const void *buf, size_t len, bool owner/*=true*/ )
{
	_buf       = NULL;
	_bufLen    = 0;
	_length    = 0;
	_blockSize = 0;
	_owner     = true;
	_userData  = NULL;
	if (owner) {
		copy( buf, len );
	}
	else {
		swap( (void *)buf, len );
	}
}

Buffer::~Buffer()
{
	if (_owner) {
		free(_buf);
	}
}

//	Operators
Buffer &Buffer::operator=( const Buffer &other ) {
	_blockSize = other._blockSize;
	_userData  = other._userData;
	copy( other._buf, other._length );
	return *this;
}

bool Buffer::operator==( const Buffer &other ) const {
	return _length == other._length && !memcmp(_buf,other._buf,_length);
}

//	Getters
size_t Buffer::length() const {
	return _length;
}

bool Buffer::empty() const {
	return _length == 0;
}

size_t Buffer::capacity() const {
	return _bufLen;
}

size_t Buffer::blockSize() const {
	return _blockSize;
}

void *Buffer::data() const {
	return _buf;
}

BYTE *Buffer::bytes() const {
	return (BYTE *)_buf;
}

//	Operations
void Buffer::resize( size_t len ) {
	if (len > 0) {
		MAKE_SPACE( len, true );
	}
	else {
		len = 0;
	}
	_length = len;
}

void Buffer::append( const void *buf, size_t len ) {
	copy( _length, buf, len );
}

void Buffer::append( const Buffer &buf ) {
	copy( _length, buf._buf, buf._length );
}

void Buffer::copy( const void *buf, size_t len ) {
	MAKE_SPACE( len, false );
	memcpy(_buf,buf,len);
	_length = len;
}

void Buffer::copy( size_t pos, const void *buf, size_t len ) {
	size_t s = pos + len;
	MAKE_SPACE( s, true );
	BYTE *ptr = (BYTE *)_buf;
	memcpy( ptr+pos, buf, len );
	if (s > _length) {
		_length = s;
	}
}

void Buffer::swap( void *buf, size_t len ) {
	if (_owner) {
		free(_buf);
	}
	_buf    = buf;
	_bufLen = _length = len;
	_owner  = false;
	_blockSize = 0;
}

void Buffer::swap( Buffer &other ) {
	std::swap(_buf,other._buf);
	std::swap(_bufLen,other._bufLen);
	std::swap(_length,other._length);
	std::swap(_blockSize,other._blockSize);
	std::swap(_owner,other._owner);
}

//	User data
void Buffer::setUserData( void *data ) {
	_userData = data;
}

void *Buffer::getUserData() const {
	return _userData;
}

class ConvertToHex {
public:
	ConvertToHex( std::string &result ) : _result(result) {}

	ConvertToHex &operator=( const ConvertToHex &other ) {
		memcpy(_temp,other._temp,16);
		_result = other._result;
		return *this;
	}

	void operator()( BYTE ch ) {
		_temp[0] = 0;
		DWORD x=0x00000000FF & ch;
		sprintf( _temp, "%02X", x );
		if (_result.length()) {
			_result += ":";
		}
		_result += _temp;
	}

	std::string &_result;
private:
	char _temp[16];
};

std::string Buffer::asHexa() const {
	std::string result;
	ConvertToHex oper( result );
	format( oper );
	return result;
}

//	Aux
void Buffer::makeSpace( size_t len, bool copy ) {
	//	Get bytes to allocate
	if (_blockSize) {
		size_t blocks = (len < _blockSize) ? 1 : (len / _blockSize) + 1;
		_bufLen = blocks*_blockSize;
	}
	else {
		_bufLen = len;
	}

	if (copy && _length) {
		//	Translate memory
		void *tmp = malloc( _bufLen );
		memcpy(tmp,_buf,_length);
		if (_owner) {
			free(_buf);
		}
		_buf = tmp;
	}
	else if (_owner) {
		_buf = realloc( _buf, _bufLen );
	}
	else {
		_buf = malloc( _bufLen );
	}
}

}
