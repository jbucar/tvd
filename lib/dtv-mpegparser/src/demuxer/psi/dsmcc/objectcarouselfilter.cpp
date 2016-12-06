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
#include "objectcarouselfilter.h"
#include "dsmccdemuxer.h"
#include "module.h"
#include "biop/ior.h"
#include "biop/object.h"
#include "../psi.h"
#include "../../../resourcemanager.h"
#include <util/fs.h>
#include <util/mcr.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace tuner {
namespace dsmcc {

namespace fs = boost::filesystem;

ObjectCarouselFilter::ObjectCarouselFilter( ID tag, DSMCCDemuxer *demux )
	: DSMCCFilter( demux )
{
	_tag   = tag;
	_root  = NULL;
	_rootPath = demux->resourceMgr()->mkTempFileName( "oc_%%%%%%" );

	//	filter DSI table
	filterDSI();
}

ObjectCarouselFilter::~ObjectCarouselFilter()
{
	cleanup();
	util::safeRemoveDirectory( _rootPath );
}

//	Notifications
void ObjectCarouselFilter::onMounted( const OnMounted &callback ) {
	_onMounted = callback;
}

//	Generic aux
void ObjectCarouselFilter::cleanup( bool freeResources/*=true*/ )  {
	//	Cleanup mounted carousel
	if (_root) {
		if (freeResources) {
			_root->freeResources();
		}
		delete _root;
		_root = NULL;
	}

	//	Delete temporary objects
	BOOST_FOREACH( dsmcc::biop::Object *obj, _objects ) {
		if (freeResources) {
			obj->freeResources();
		}
		delete obj;
	}
	_objects.clear();
}

//	Tables
void ObjectCarouselFilter::processDSI( const boost::shared_ptr<dsmcc::DSI> &dsi ) {
    //  Check if object carousel is present (Chapter 7 - ISO DSM-CC)
    int len = dsi->privateData().length();
    if (len > 0) {
        BYTE *payload = dsi->privateData().bytes();

        dsmcc::biop::Ior ior;
        ior.parse( payload, len );
        if (ior.isValid()) {
			//	Identify root directory
			_rootLocation = ior.location();
			LDEBUG( "ObjectCarouselFilter", "Root location: %s", _rootLocation.asString().c_str() );

			//	Check tag is correct
			if (_tag == ior.associationTag()) {
				filterDII();
			}
			else {
				LWARN( "ObjectCarouselFilter", "Tag incorrect or invalid. Ignoring DSI: tag=%x", ior.associationTag() );
			}
        }
    }
}

void ObjectCarouselFilter::processDII( const boost::shared_ptr<dsmcc::DII> &dii ) {
	filterModules( dii );
	//	TODO: Object carousel isn't using DownloadID???
}

void ObjectCarouselFilter::processModule( const boost::shared_ptr<dsmcc::Module> &module ) {
    //  Goto begin of module
    size_t offset=0;
	dsmcc::biop::ModuleInfo biopInfo;

	//	Parse module info as biop::ModuleInfo
	biop::parseModuleInfo( module->info(), biopInfo );

	//	Parse user data as module descriptors
	if (biopInfo.userInfo.length()) {
		dsmcc::module::Descriptors descs;
		dsmcc::module::parseDescriptors( biopInfo.userInfo, descs );

		//	Check if module is compressed
		dsmcc::module::Descriptors::const_iterator it=descs.find( MODULE_DESC_COMPRESSED );
		bool isCompressed = (it != descs.end());
		if (!isCompressed) {
			it=descs.find( MODULE_DESC_COMPRESSION );
			isCompressed = (it != descs.end());
		}

		if (isCompressed) {
			const dsmcc::module::CompressionTypeDescriptor &compressDesc = (*it).second.get<dsmcc::module::CompressionTypeDescriptor>();
			module->inflate( compressDesc.type, compressDesc.originalSize );
		}
	}

    while (offset < module->size()) {
        //  parse BIOP object
	    dsmcc::biop::Object *object=dsmcc::biop::Object::parseObject( resourceMgr(), module.get(), offset );
        if (object) {
            //  Update carousel with object parsed
            updateCarousel( object );
        }
        else {
			//	Object not parsed, so carousel cannot be mounted, so abort ...
            LWARN( "ObjectCarouselFilter", "BIOP object not parsed: moduleID=%d, moduleSize=%d, offset=%ld",
				module->id(), module->size(), offset );
			break;
        }
    }

    //LDEBUG( "ObjectCarouselFilter", "Module %d parsed complete!", module->id() );
}

void ObjectCarouselFilter::updateCarousel( dsmcc::biop::Object *object ) {
	//object->show();

	//	LDEBUG( "ObjectCarouselFilter", "root=%p", _root );
	if (*object == _rootLocation) {
		if (util::safeRemoveDirectory( _rootPath )) {
			object->process( NULL, _rootPath, _objects );
			_root = object;
		}
    }
    else {
        _objects.push_back( object );
        if (_root) {
            _root->onObject( _objects );
        }
    }

    if (_root && _root->isComplete() && !_onMounted.empty()) {
		mountCarousel();
    }
}

void ObjectCarouselFilter::mountCarousel() {
	LDEBUG( "ObjectCarouselFilter", "Mounted called" );

	//	Notify to dependent
	dsmcc::Events events;
	_root->getEvents( events );

	//	Notify
	ObjectCarousel *oc = new ObjectCarousel( _rootPath, events );
	demux()->dispatchNotify<ObjectCarousel>( _onMounted, oc );

	//	Cleanup, so if a update arrive, process all object again!
	cleanup( false );
}

}
}

