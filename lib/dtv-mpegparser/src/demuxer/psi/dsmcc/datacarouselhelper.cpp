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
#include "datacarouselhelper.h"
#include "../../descriptors/demuxers.h"
#include "../../../service/service.h"
#include <util/log.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {

DataCarouselHelper::DataCarouselHelper( void )
{
}
	
DataCarouselHelper::~DataCarouselHelper( void )
{
}

bool DataCarouselHelper::collectTags( Service *srv, bool (*cmp)( BYTE type ) ) {
	bool find=false;
	Tags &tags = _tags[srv->id()];

	//	Clear tags of service
	tags.clear();
	
	//	Attach on DSMCC streams
	BOOST_FOREACH( const ElementaryInfo &elem, srv->elements() ) {
		if ((*cmp)(elem.streamType)) {
			//	ABNT 15606-3 Section 6.3.1
			//	Check for:
			//		association_tag_descriptor   (0x14)
			//		stream_identifier_descriptor (0x52)

			{	//	Find association_tag_descriptor
				desc::AssociationTagDescriptor assDesc;
				if (DESC_PARSE( elem.descriptors, association_tag, assDesc )) {
					find |= addTag( tags, (util::BYTE) assDesc.tag, elem.pid );
				}
			}

			{	//	Find stream_identifier_descriptor
				util::BYTE tag;
				if (DESC_PARSE( elem.descriptors, stream_identifier, tag )) {
					find |= addTag( tags, tag, elem.pid );
				}
			}
		}
	}
	return find;
}

void DataCarouselHelper::clearTags() {
	MapOfTags::iterator it=_tags.begin();
	for (it=_tags.begin(); it != _tags.end(); ++it) {
		(*it).second.clear();
	}
}

ID DataCarouselHelper::findTag( BYTE tag ) const {
	ID pid=TS_PID_NULL;
	LDEBUG( "DataCarouselHelper", "Find Tag: tag=%02x", tag );
	MapOfTags::const_iterator it=_tags.begin();
	for (it=_tags.begin(); it != _tags.end(); ++it) {
		pid=findTag( (*it).second, tag );
		if (pid != TS_PID_NULL) {
			break;
		}
	}	
	return pid;
}

struct TagFinder {
	TagFinder( BYTE tag ) : _tag(tag) {}
	bool operator()( const DataCarouselHelper::Tag &dsmccTag ) const {
		return dsmccTag.first == _tag;
	}
	BYTE _tag;
};

bool DataCarouselHelper::addTag( Tags &tags, BYTE tag, ID pid ) {
	bool result=false;
	
	//	Ignore if tag already exist
	ID findPID = findTag( tags, tag );
	if (findPID == TS_PID_NULL) {
		LDEBUG( "DataCarouselHelper", "Add tag: tag=%02x, pid=%04x", tag, pid );
		tags.push_back( std::make_pair(tag,pid) );
		result=true;
	}
	return result;
}

ID DataCarouselHelper::findTag( const Tags &tags, BYTE tag ) const {
	Tags::const_iterator it=std::find_if( tags.begin(), tags.end(), TagFinder(tag) );
	return (it != tags.end()) ? (*it).second : (ID) TS_PID_NULL;
}

ID DataCarouselHelper::findTag( ID serviceID, BYTE tag ) const {
	ID pid=TS_PID_NULL;
	LDEBUG( "DataCarouselHelper", "Find Tag: service=%04x, tag=%02x", serviceID, tag );
	MapOfTags::const_iterator it=_tags.find( serviceID );
	if (it != _tags.end()) {
		pid = findTag( (*it).second, tag );
	}
	return pid;
}

std::vector<BYTE> DataCarouselHelper::clearTags( Service *srv ) {
	Tags &tags = _tags[srv->id()];
	std::vector<BYTE> ret;
	BOOST_FOREACH( Tag &tag, tags ) {
		ret.push_back( tag.first );
	}
	tags.clear();
	return ret;
}
	
}
}
