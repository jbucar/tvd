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
#include "descriptors.h"
#include "descriptors/generic.h"
#include <util/mcr.h>
#include <util/log.h>
#include <util/string.h>
#include <util/assert.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define GET_DESCRIPTOR_LENGTH(ptr) ((((ptr)[0] & 0x0F) << 8) | (ptr)[1]);

namespace tuner {
namespace desc {

Descriptors::Descriptors()
	: _buf( USER_PRIVATE_DESCRIPTOR )
{
}

Descriptors::Descriptors( const Descriptors &other )
	: _buf(other._buf)
{
}

Descriptors::~Descriptors()
{
}

void Descriptors::clear() {
	_buf.resize(0);
}

bool Descriptors::empty() const {
	return _buf.length() > 0 ? false : true;
}

util::BYTE *Descriptors::bytes() const {
	return _buf.bytes();
}

size_t Descriptors::length() const {
	return _buf.length();
}

void Descriptors::operator+=( const Descriptors &other ) {
	_buf.append( other._buf );
}

size_t Descriptors::append( util::BYTE *ptr, size_t len ) {
	//  Get descriptor length
	size_t dLen = GET_DESCRIPTOR_LENGTH(ptr);

	LTRACE( "desc", "Add descriptors: bufLen=%d, dLen=%d", len, dLen );
	size_t off = 2;
	while (off < len && off < dLen) {
		off += DESC_SIZE(ptr+off);
	}
	off-=2;

	if (dLen != off) {
		LWARN( "desc", "Parsing descriptor error: bufLen=%d, dLen=%d, off=%d",
			len, dLen, off );
		return len;
	}

	//	Append data
	_buf.append( (ptr+2), dLen );

	return 2+dLen;
}

void Descriptors::addDescriptor( util::BYTE *ptr, size_t len ) {
	_buf.append( ptr, len );
}

bool Descriptors::find( util::BYTE tag ) const {
	size_t off=0;
	return get( tag, off ) ? true : false;
}

util::BYTE *Descriptors::get( util::BYTE tag, size_t &offset ) const {
	util::BYTE *ptr = bytes();
	size_t len = length();
	while (offset < len) {
		if (DESC_TAG(ptr+offset) == tag) {
			util::BYTE *tmp=(ptr+offset);
			offset += DESC_SIZE(ptr+offset);
			return tmp;
		}
		offset += DESC_SIZE(ptr+offset);
	}
	return NULL;
}

size_t Descriptors::getTags( std::vector<util::BYTE> &tags ) const {
	size_t offset = 0;
	util::BYTE *ptr = bytes();
	size_t len = length();
	while (offset < len) {
		tags.push_back( DESC_TAG(ptr+offset) );
		offset += DESC_SIZE(ptr+offset);
	}
	return tags.size();
}

std::string Descriptors::show() const {
	util::BYTE *ptr = bytes();
	size_t len = length();
	size_t offset = 0;

	int i=0;
	std::string tmp;
	while (offset < len) {
		tmp += util::format( i ? ",%02X" : "%02X", DESC_TAG(ptr+offset) );
		offset += DESC_SIZE(ptr+offset);
		i++;
	}

	if (i) {
		return util::format( "%d(%s)", i, tmp.c_str() );
	}
	else {
		return util::format( "%d", i );
	}
}

std::string Descriptors::showBytes() const {
	return _buf.asHexa().c_str();
}

}
}
