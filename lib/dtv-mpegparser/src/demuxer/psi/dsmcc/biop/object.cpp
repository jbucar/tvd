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
#include "object.h"
#include "biop.h"
#include "file.h"
#include "directory.h"
#include "streamevent.h"
#include "../module.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <stdexcept>

namespace tuner {
namespace dsmcc {
namespace biop {

namespace fs = boost::filesystem;

size_t Object::objectKey( const BYTE *data, size_t len, ObjectKeyType &key ) {
    size_t off=0;

    //  Object key len
    BYTE objectKeyLen = RB(data,off);
    if (objectKeyLen > 4) {
        throw std::overflow_error( "ObjectKey is greater than 4" );
    }
    DTV_ASSERT(len >= size_t(1+objectKeyLen));
    UNUSED(len);
	
    //  Object key
	key = RDW( data, off );

	//LDEBUG( "dsmcc::Object", "Parsing object key: len=%08lx, key=%08lx", len, key );

    return off;
}

Object *Object::parseObject( ResourceManager *resMgr, Module *module, size_t &off ) {
    Object *obj;
	ObjectLocation loc;
    BYTE buf[BIOP_MAX_HEADER];

    size_t offset = 0;
    size_t len = module->read( off, buf, BIOP_MAX_HEADER );
    if (len < BIOP_MAX_HEADER) {
        LWARN( "dsmcc::Object", "Not enough buffer: len=%ld, offset=%ld", len, offset );
        return NULL;
    }

    DWORD magic = RDW(buf,offset);
    if (magic != BIOP_MAGIC) {
        LWARN( "dsmcc::Object", "Bad magic number: magic=%x, found=%x", BIOP_MAGIC, magic );
        return NULL;
    }

    WORD biopVersion = RW(buf,offset);
    if (biopVersion != BIOP_VER) {
        LWARN( "dsmcc::Object", "Invalid version" );
        return NULL;
    }

    BYTE byteOrder = RB(buf,offset);
    if (byteOrder) {
        LWARN( "dsmcc::Object", "Invalid byte order" );
        return NULL;
    }

    BYTE msgType = RB(buf,offset);
    if (msgType) {
        LWARN( "dsmcc::Object", "Invalid type" );
        return NULL;
    }

    //  Check BIOP message size
    DWORD msgSize = RDW(buf,offset);
    if (module->size()-off-offset < msgSize) {
        LWARN( "dsmcc::Object", "Not enough bytes for BIOP message: msgSize=%d, rest%ld", msgSize, module->size()-off-offset );
        return NULL;
    }
    //  12 bytes parsed = BIOP_MIN_HEADER

    offset += objectKey( buf+offset, len-offset, loc.keyID );

    DWORD objectKindLen = RDW(buf,offset);
    if (objectKindLen != 4) {
        LWARN( "dsmcc::Object", "Invalid object kind" );
        return NULL;
    }

	//	Chapter 11: The downloadID field of the DownloadDataBlock messages shall have the same value
	//	as the carouselID field of the U-U Object Carousel
	loc.carouselID = module->downloadID();
	loc.moduleID   = module->id();

    DWORD objectKind = RDW(buf,offset);
    switch (objectKind) {
        case BIOP_OBJECT_FILE:
            obj = new File( loc );
            break;
        case BIOP_OBJECT_DIR:
        case BIOP_OBJECT_GW:
            obj = new Directory( loc );
            break;
		// case BIOP_OBJECT_STR:
		// 	break;
		case BIOP_OBJECT_STE:
			obj = new StreamEvent( loc );
			break;
	    default: {
            obj = NULL;
            LWARN( "dsmcc::Object", "BIOP object kind %x not processed", objectKind );
	    }
    }

	WORD objectInfoLen = RW(buf,offset);
	
    //  Update offset (27 bytes parsed = BIOP_MAX_HEADER)
    off += offset;
    if (obj && !obj->parse( resMgr, module, off, objectInfoLen )) {
		delete obj;
		obj = NULL;
	}
	
    return obj;
}

Object::Object( const ObjectLocation &loc )
	: _loc( loc )
{
}

Object::~Object()
{
}

//  Getters
const std::string &Object::name() const {
    return _name;
}

void Object::name( const std::string &name ) {
    _name = name;
}

bool Object::operator==( const ObjectLocation &loc ) {
	return _loc == loc;
}

void Object::show() const {
	LDEBUG( "dsmcc::Object", "\t %s, type=%01x, name=%s",
		_loc.asString().c_str(), type(), _name.c_str() );
}

//	Parser
void Object::getEvents( std::vector<Event *> &/*events*/ ) {
}

void Object::freeResources( void ) {
}

void Object::onObject( std::list<Object *> & /*unprocessed*/ ) {
}

bool Object::parse( ResourceManager *resMgr, Module *module, size_t &off, WORD objectInfoLen ) {
	//	Need parse Object Info
	if (objectInfoLen && needObjectInfo()) {
		//	Parse ObjectInfo
		if (!parseObjectInfo( module, off, objectInfoLen )) {
			LWARN( "dsmcc::Object", "Cannot parse biop::ObjectInfo" );
			return false;
		}
	}
	//	Update offset to skip objectInfo
	off += objectInfoLen;

	//	Read Service Context List Count
	BYTE serviceContextListCount;	
	if (!module->readB( off, serviceContextListCount )) {
		LWARN( "dsmcc::Object", "Cannot read bytes to parse ServiceContextList" );
		return false;
	}

	//	Parse Service Context List
	for (BYTE context=0; context<serviceContextListCount; context++) {
		if (!parseContext( module, off )) {
			LWARN( "dsmcc::Object", "Cannot parse Service Context List: context=%d", context );
			return false;
		}
	}

	//	Skip body len
	DWORD bodyLen;
	if (!module->readDW( off, bodyLen )) {
		LWARN( "dsmcc::Object", "Cannot parse body len" );
		return false;
	}
	size_t old = off;
	off += bodyLen;
	
	//	Parse body
	return parseBody( resMgr, module, old, bodyLen );
}

bool Object::needObjectInfo( void ) const {
	return false;
}

bool Object::parseObjectInfo( Module * /*module*/, size_t /*offset*/, WORD /*objectInfoLen*/ ) {
	return false;
}

bool Object::parseContext( Module * /*module*/, size_t & /*offset*/ ) {
	return false;
}

//  Aux
std::string Object::getPath( Object *parent, const std::string &name ) {
	fs::path p;

    //  Make object name
    if (parent) {
        p = parent->name();
    }
    p /= name;
    return p.string();
}

}
}
}
