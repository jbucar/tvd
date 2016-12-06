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
#include "streamevent.h"
#include "../module.h"
#include "../event.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {
namespace biop {

StreamEvent::StreamEvent( const ObjectLocation &loc )
	: Object( loc ), _buf( 1024 )
{
}

StreamEvent::~StreamEvent()
{
}

bool StreamEvent::needObjectInfo( void ) const {
	return true;
}

bool StreamEvent::parseObjectInfo( Module *module, size_t offset, WORD objInfoLen ) {
	//	Read all object info
	_buf.resize( objInfoLen );
	if (module->read( offset, _buf.bytes(), objInfoLen ) != objInfoLen) {
		LERROR( "biop::StreamEvent", "Cannot read ObjectInfo" );
		return false;
	}
	size_t off = 0;
	BYTE *ptr  = _buf.bytes();

	//	Parse DSM::Stream::Info_T
	BYTE descLen = RB(ptr,off);
	off += descLen;

	off += sizeof(DWORD)+sizeof(DWORD)+3;
	// DWORD secs  = RDW(ptr,off);
	// DWORD micro = RDW(ptr,off);
	// BYTE audio  = RB(ptr,off);
	// BYTE video  = RB(ptr,off);
	// BYTE data   = RB(ptr,off);

	//	Parse DSM::Event::EventList_T
	WORD eventNames = RW(ptr,off);
	for (WORD i=0; i<eventNames; i++) {
		EventName ev;

		BYTE nameLen = RB(ptr,off);
		ev.name = std::string( (char *)(ptr+off), nameLen-1 );
		off += nameLen;

		_events.push_back( ev );
	}
	return true;
}

bool StreamEvent::parseBody( ResourceManager * /*resMgr*/, Module *module, size_t offset, DWORD bodyLen ) {
	//	Read all body
	_buf.resize( bodyLen );
	if (module->read( offset, _buf.bytes(), bodyLen ) != bodyLen) {
		LWARN( "biop::StreamEvent", "cannot read object body" );
		return false;
	}

	BYTE *ptr = _buf.bytes();

	//	Parse taps
	size_t off = parseTaps( ptr, _taps );

	//	Read events
	BYTE eventIDs = RB(ptr,off);

	//	Check count of events
	if (eventIDs != _events.size()) {
		LWARN( "biop::StreamEvent", "events names and ids not deffers" );
		return false;
	}

	for (int i=0; i<eventIDs; i++) {
		_events[i].id = RW(ptr,off);
	}

	return true;
}

void StreamEvent::process( Object *parent, const std::string &objName, std::list<Object *> & /*unprocessed*/ ) {
	//	Not parent dependent!
    name( objName );

    LDEBUG( "dsmcc::StreamEvent", "Object name %s processed", getPath(parent,objName).c_str() );
    //  Do nothing
}

void StreamEvent::getEvents( std::vector<dsmcc::Event *> &events ) {
	//	find assocTag
	BOOST_FOREACH( const Tap &tap, _taps ) {
		if (tap.use == BIOP_TAP_STR_EVENT_USE) {
			events.push_back( new dsmcc::Event( name(), (util::BYTE) tap.assocTag, _events ) );
		}
	}
}

BYTE StreamEvent::type() const {
    return 1;
}

bool StreamEvent::isComplete() const {
	return true;
}

}
}
}
